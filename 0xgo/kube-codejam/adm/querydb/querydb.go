package querydb

import (
	"database/sql"

	"0xgo/kube-codejam/adm/utils"

	"time"

	_ "github.com/go-sql-driver/mysql"
)

type PayloadProg struct {
	P_INDEX             int
	P_EMAIL             string
	P_OWNER             string
	SUBMIT_LANG         string
	SUBMIT_VERSION      string
	SUBMIT_CODE         string
	SUBMIT_INPUT        string
	CREATION_TIME       time.Time
	EFFECTIVE           uint8
	VERIFIED            uint8
	JAM_DEPLOYMENT_NAME string
	JAM_DEPLOYMENT_YAML string
	JAM_CONFIGMAP_NAME  string
	EXEC_START_TIME     time.Time
	LOG_RESOURCE        string
	LOG_STDOUT          string
	LOG_STDERR          string
	ILLEGAL_FLAG        string
	TERMINATION_TIME    time.Time
}

var DB *sql.DB

type RowInterface map[string]interface{}

func InitDBConn() {

	var err error = nil

	DB, err = sql.Open("mysql", "runpayload:youdonthavetoknow@tcp(runpayloaddb:3306)/runpayload")

	if err != nil {
		panic(err.Error())
	}

	DB.SetConnMaxLifetime(time.Second * 100)
	DB.SetConnMaxIdleTime(time.Second * 100)
	DB.SetMaxOpenConns(100)
	DB.SetMaxIdleConns(100)

	utils.EventLogger("DB Connected")
}

func DbQuery(query string, args []any) (*sql.Rows, error) {

	var empty_row *sql.Rows

	results, err := DB.Query(query, args[0:]...)

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
