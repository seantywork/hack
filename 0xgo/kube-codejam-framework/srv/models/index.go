package models

import (
	"0xgo/kube-codejam-framework/adm/utils"
	"database/sql"
	"fmt"
	"time"

	_ "github.com/go-sql-driver/mysql"
)

var RUNDB *sql.DB

type RowInterface map[string]interface{}

func InitDBConn() {

	var err error

	RUNDB, err = sql.Open("mysql", "runpayload:youdonthavetoknow@tcp(runpayloaddb:3306)/runpayload")

	if err != nil {
		panic(err.Error())
	}

	RUNDB.SetConnMaxLifetime(time.Second * 100)
	RUNDB.SetConnMaxIdleTime(time.Second * 100)
	RUNDB.SetMaxOpenConns(100)
	RUNDB.SetMaxIdleConns(100)

	utils.EventLogger("DB Connected")

}

func DbQuery(query string, args []any) (*sql.Rows, error) {

	var empty_row *sql.Rows

	results, err := RUNDB.Query(query, args[0:]...)

	if err != nil {

		return empty_row, err

	}

	return results, err

}

func ScannerDynamic(list *sql.Rows) []RowInterface {

	fields, _ := list.Columns()

	rows := make([]RowInterface, 0)

	for list.Next() {

		scans := make([]interface{}, len(fields))
		row := make(map[string]interface{})

		for i := range scans {
			scans[i] = &scans[i]
		}
		list.Scan(scans...)
		for i, v := range scans {
			var value interface{}
			if v != nil {
				value = v
			}
			row[fields[i]] = value
		}
		rows = append(rows, row)
	}

	return rows

}

func CheckRegistryAvailability(p_email string) error {

	q :=
		`
	SELECT p_index 
	FROM payload_progress 
	WHERE
		effective = 1 
	`
	a := []any{}

	res, err := DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("check reg availability: %s", err.Error())

	}

	containers := ScannerDynamic(res)

	res.Close()

	if len(containers) > 10 {
		return fmt.Errorf("check reg availiability: %s", "full queue")
	}

	q =
		`
	SELECT p_index, creation_time 
	FROM payload_progress 
	WHERE
		effective = 1 
		AND p_email = ?
	`
	a = []any{p_email}

	res, err = DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("check reg availability: %s", err.Error())

	}

	containers = ScannerDynamic(res)

	res.Close()

	if len(containers) != 0 {

		latest_idx := len(containers) - 1

		t_now := time.Now()

		t, _ := time.Parse("2006-01-02 15:04:05", string(containers[latest_idx]["creation_time"].([]uint8)))

		diff := t_now.Sub(t)

		if diff.Milliseconds() > 60000 {

			err := Pardon(p_email)

			if err != nil {
				return fmt.Errorf("check reg availability: %s", err.Error())
			}

		} else {

			return fmt.Errorf("check reg availability: %s", "already queued")

		}

	}

	return nil
}

func RegisterSubmitRecord(email string, lang string, version string, code string, input string) error {

	q :=
		`
	INSERT INTO 
		payload_progress (
			p_email,
			submit_lang,
			submit_version,
			submit_code,
			submit_input,
			creation_time,
			effective,
			verified
		)
		VALUES (
			?,
			?,
			?,
			?,
			?,
			CURRENT_TIMESTAMP(),
			1,
			0
		) 
	`
	a := []any{
		email,
		lang,
		version,
		code,
		input,
	}

	res, err := DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("reg submit record: %s", err.Error())

	}

	res.Close()

	return nil

}

func RegisterVerificationFlagAndOwner(p_email string, p_owner string) error {

	q := `
	

	SELECT p_index
	FROM payload_progress
	WHERE
		p_owner IS NULL
		AND creation_time IS NOT NULL
		AND effective = 1
		AND verified = 0
		AND exec_start_time IS NULL
		AND termination_time IS NULL
		AND p_email = ?
	
	`

	a := []any{
		p_email,
	}

	res, err := DbQuery(q, a)

	if err != nil {
		return fmt.Errorf("reg verification: %s", err.Error())
	}

	containers := ScannerDynamic(res)

	res.Close()

	if len(containers) != 1 {
		return fmt.Errorf("reg verification: %s", "no record")
	}

	p_index := containers[0]["p_index"].(int64)

	q = `
	
	UPDATE payload_progress
	SET
		p_owner = ?,
		verified = 1
	WHERE
		p_index = ?
	
	`

	a = []any{
		p_owner,
		p_index,
	}

	res, err = DbQuery(q, a)

	if err != nil {
		return fmt.Errorf("reg verification: %s", err.Error())
	}

	res.Close()

	return nil

}

func Pardon(p_email string) error {

	q := `
	
	UPDATE payload_progress
	SET
		effective = 0
	WHERE
		p_email = ?
	
	`

	a := []any{
		p_email,
	}

	res, err := DbQuery(q, a)

	if err != nil {
		return fmt.Errorf("pardoning: %s", err.Error())
	}

	res.Close()

	return nil
}
