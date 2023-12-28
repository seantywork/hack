package querydb

import (
	_ "database/sql"
	"fmt"

	_ "github.com/go-sql-driver/mysql"
)

func GetOrphanedRecordByPowner(p_index int64) ([]RowInterface, error) {

	var container []RowInterface

	q :=
		`
	SELECT 
		p_index, 
		p_owner,
		creation_time,
		effective,
		jam_deployment_name,
		jam_configmap_name  
	FROM payload_progress 
	WHERE
		p_index = ?
	`
	a := []any{p_index}

	res, err := DbQuery(q, a)

	if err != nil {

		return container, fmt.Errorf("failed get orphaned record: %s", err.Error())

	}

	container = ScannerDynamic(res)

	res.Close()

	return container, nil

}

func GetPindexByPownerForDeletion(p_owner string) ([]RowInterface, error) {

	var container []RowInterface

	q :=
		`
	SELECT 
		p_index, 
		jam_deployment_name,
		jam_configmap_name,
		p_owner 
	FROM payload_progress 
	WHERE
		effective = 1 

	`
	a := []any{p_owner}

	res, err := DbQuery(q, a)

	defer res.Close()

	if err != nil {

		return container, fmt.Errorf("failed get pindex: %s", err.Error())

	}

	raw_container := ScannerDynamic(res)

	if len(raw_container) != 1 {

		return container, fmt.Errorf("failed get pindex: %s", "length")

	}

	container = raw_container

	return container, nil

}

func CleanUpOrphanedRecord(p_index int64) error {

	q :=
		`
	UPDATE payload_progress
	SET 
		effective = 0
	WHERE
		p_index = ?
	`

	a := []any{p_index}

	res, err := DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed cleanup orphaned record: %s", err.Error())

	}

	res.Close()

	return nil

}
