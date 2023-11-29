package querydb

import (
	"fmt"
)

func GetPindexByPowner(p_owner string) ([]RowInterface, error) {

	var container []RowInterface

	q :=
		`
	SELECT p_index, p_owner 
	FROM payload_progress 
	WHERE
		effective = 1 
		AND verified = 1
		AND creation_time IS NOT NULL
		AND exec_start_time IS NULL
		AND termination_time IS NULL
		AND p_owner IS NOT NULL
		AND p_owner = ?
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

func GetSubmitRecordByPowner(p_owner string) ([]RowInterface, error) {

	var container []RowInterface

	q :=
		`
	SELECT 
		submit_lang, 
		submit_version, 
		submit_code, 
		submit_input,
		p_owner 
	FROM payload_progress 
	WHERE
		effective = 1 
		AND verified = 1
		AND creation_time IS NOT NULL
		AND exec_start_time IS NULL
		AND termination_time IS NULL
		AND p_owner IS NOT NULL
		AND p_owner = ?
	`
	a := []any{p_owner}

	res, err := DbQuery(q, a)

	defer res.Close()

	if err != nil {

		return container, fmt.Errorf("failed get submit code: %s", err.Error())

	}

	raw_container := ScannerDynamic(res)

	if len(raw_container) != 1 {

		return container, fmt.Errorf("failed get submit code: %s", "length")

	}

	container = raw_container

	return container, nil

}

func GetCreationTimeByPowner(p_owner string) ([]RowInterface, error) {

	var container []RowInterface

	q :=
		`
	SELECT creation_time, p_owner 
	FROM payload_progress 
	WHERE
		effective = 1 
		AND verified = 1
		AND creation_time IS NOT NULL
		AND exec_start_time IS NULL
		AND termination_time IS NULL
		AND p_owner IS NOT NULL
		AND p_owner = ?
	`
	a := []any{p_owner}

	res, err := DbQuery(q, a)

	defer res.Close()

	if err != nil {

		return container, fmt.Errorf("failed get creation time: %s", err.Error())

	}

	raw_container := ScannerDynamic(res)

	if len(raw_container) != 1 {

		return container, fmt.Errorf("failed get creation time: %s", "length")

	}

	container = raw_container
	return container, nil

}

func GetPindexAndVerifiedFlag(p_owner string) ([]RowInterface, error) {

	var container []RowInterface

	q :=
		`
	SELECT p_index, verified, p_owner 
	FROM payload_progress 
	WHERE
		effective = 1 
		AND verified = 1
		AND creation_time IS NOT NULL
		AND exec_start_time IS NULL
		AND termination_time IS NULL
		AND p_owner IS NOT NULL
		AND p_owner = ?
	`
	a := []any{p_owner}

	res, err := DbQuery(q, a)

	defer res.Close()

	if err != nil {

		return container, fmt.Errorf("failed get verified flag: %s", err.Error())

	}

	raw_container := ScannerDynamic(res)

	if len(raw_container) != 1 {

		return container, fmt.Errorf("failed get verified flag: %s", "length")

	}
	container = raw_container
	return container, nil
}

func SetJamDeploymentName(p_owner string, dpl_name string) error {

	q :=
		`
	SELECT p_index
	FROM payload_progress 
	WHERE
		effective = 1 
		AND verified = 1
		AND creation_time IS NOT NULL
		AND exec_start_time IS NULL
		AND termination_time IS NULL
		AND p_owner IS NOT NULL
		AND p_owner = ?
	`
	a := []any{p_owner}

	res, err := DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed set jam dpl name: %s", err.Error())

	}

	raw_container := ScannerDynamic(res)

	if len(raw_container) != 1 {

		return fmt.Errorf("failed set jam dpl name: %s", "length")

	}

	p_index := raw_container[0]["p_index"].(int64)

	if err != nil {

		return fmt.Errorf("failed set jam dpl name: %s", err.Error())

	}

	res.Close()

	q =
		`
	UPDATE payload_progress
	SET 
		jam_deployment_name = ?
	WHERE
		p_index = ?
	`

	a = []any{dpl_name, p_index}

	res, err = DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed set jam dpl name: %s", err.Error())

	}

	res.Close()

	return nil

}

func SetJamDeploymentYaml(p_owner string, dpl_yaml string) error {

	q :=
		`
	SELECT p_index
	FROM payload_progress 
	WHERE
		effective = 1 
		AND verified = 1
		AND creation_time IS NOT NULL
		AND exec_start_time IS NULL
		AND termination_time IS NULL
		AND p_owner IS NOT NULL
		AND p_owner = ?
	`
	a := []any{p_owner}

	res, err := DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed set jam dpl yaml: %s", err.Error())

	}

	raw_container := ScannerDynamic(res)

	if len(raw_container) != 1 {

		return fmt.Errorf("failed set jam dpl yaml: %s", "length")

	}

	p_index := raw_container[0]["p_index"].(int64)

	if err != nil {

		return fmt.Errorf("failed set jam dpl yaml: %s", err.Error())

	}

	res.Close()

	q =
		`
	UPDATE payload_progress
	SET 
		jam_deployment_yaml = ?
	WHERE
		p_index = ?
	`

	a = []any{dpl_yaml, p_index}

	res, err = DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed set jam dpl yaml: %s", err.Error())

	}

	res.Close()

	return nil

}

func SetJamConfigmapName(p_owner string, config_name string) error {

	q :=
		`
	SELECT p_index
	FROM payload_progress 
	WHERE
		effective = 1 
		AND verified = 1
		AND creation_time IS NOT NULL
		AND exec_start_time IS NULL
		AND termination_time IS NULL
		AND p_owner IS NOT NULL
		AND p_owner = ?
	`
	a := []any{p_owner}

	res, err := DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed set jam config name: %s", err.Error())

	}

	raw_container := ScannerDynamic(res)

	if len(raw_container) != 1 {

		return fmt.Errorf("failed set jam config name: %s", "length")

	}

	p_index := raw_container[0]["p_index"].(int64)

	if err != nil {

		return fmt.Errorf("failed set jam config name: %s", err.Error())

	}

	res.Close()

	q =
		`
	UPDATE payload_progress
	SET 
		jam_configmap_name = ?
	WHERE
		p_index = ?
	`

	a = []any{config_name, p_index}

	res, err = DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed set jam config name: %s", err.Error())

	}

	res.Close()

	return nil

}

func SetExecStartTime(p_owner string) error {

	q :=
		`
	SELECT p_index
	FROM payload_progress 
	WHERE
		effective = 1 
		AND verified = 1
		AND creation_time IS NOT NULL
		AND exec_start_time IS NULL
		AND termination_time IS NULL
		AND p_owner IS NOT NULL
		AND p_owner = ?
	`
	a := []any{p_owner}

	res, err := DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed set exec start time: %s", err.Error())

	}

	raw_container := ScannerDynamic(res)

	if len(raw_container) != 1 {

		return fmt.Errorf("failed set exec start time: %s", "length")

	}

	p_index := raw_container[0]["p_index"].(int64)

	if err != nil {

		return fmt.Errorf("failed set exec start time: %s", err.Error())

	}

	res.Close()

	q =
		`
	UPDATE payload_progress
	SET 
		exec_start_time = CURRENT_TIMESTAMP()
	WHERE
		p_index = ?
	`

	a = []any{p_index}

	res, err = DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed set exec start time: %s", err.Error())

	}

	res.Close()

	return nil

}

func SetLogs(p_owner string, resource string, stdout string, stderr string) error {

	q :=
		`
	SELECT p_index
	FROM payload_progress 
	WHERE
		effective = 1 
		AND verified = 1
		AND creation_time IS NOT NULL
		AND exec_start_time IS NOT NULL
		AND termination_time IS NULL
		AND p_owner IS NOT NULL
		AND p_owner = ?
	`
	a := []any{p_owner}

	res, err := DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed set logs: %s", err.Error())

	}

	raw_container := ScannerDynamic(res)

	if len(raw_container) != 1 {

		return fmt.Errorf("failed set logs: %s", "length")

	}

	p_index := raw_container[0]["p_index"].(int64)

	if err != nil {

		return fmt.Errorf("failed set logs: %s", err.Error())

	}

	res.Close()

	q =
		`
	UPDATE payload_progress
	SET 
		log_resource = ?,
		log_stdout = ?,
		log_stderr = ?
	WHERE
		p_index = ?
	`

	a = []any{resource, stdout, stderr, p_index}

	res, err = DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed set exec start time: %s", err.Error())

	}

	res.Close()

	return nil

}

func SetIllegalFlag(p_owner string, illegal_flag string) error {

	q :=
		`
	SELECT p_index
	FROM payload_progress 
	WHERE
		effective = 1 
		AND verified = 1
		AND creation_time IS NOT NULL
		AND exec_start_time IS NOT NULL
		AND termination_time IS NULL
		AND p_owner IS NOT NULL
		AND p_owner = ?
	`
	a := []any{p_owner}

	res, err := DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed set illegal flag: %s", err.Error())

	}

	raw_container := ScannerDynamic(res)

	if len(raw_container) != 1 {

		return fmt.Errorf("failed set illegal flag: %s", "length")

	}

	p_index := raw_container[0]["p_index"].(int64)

	if err != nil {

		return fmt.Errorf("failed set illegal flag: %s", err.Error())

	}

	res.Close()

	q =
		`
	UPDATE payload_progress
	SET 
		illegal_flag = ?
	WHERE
		p_index = ?
	`

	a = []any{illegal_flag, p_index}

	res, err = DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed set illegal flag: %s", err.Error())

	}

	res.Close()

	return nil

}

func SetTerminationTime(p_owner string) error {

	q :=
		`
	SELECT p_index
	FROM payload_progress 
	WHERE
		effective = 1 
		AND verified = 1
		AND creation_time IS NOT NULL
		AND exec_start_time IS NOT NULL
		AND termination_time IS NULL
		AND p_owner IS NOT NULL
		AND p_owner = ?
	`
	a := []any{p_owner}

	res, err := DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed set termination time: %s", err.Error())

	}

	raw_container := ScannerDynamic(res)

	if len(raw_container) != 1 {

		return fmt.Errorf("failed set termination time: %s", "length")

	}

	p_index := raw_container[0]["p_index"].(int64)

	if err != nil {

		return fmt.Errorf("failed set termination time: %s", err.Error())

	}

	res.Close()

	q =
		`
	UPDATE payload_progress
	SET 
		termination_time = CURRENT_TIMESTAMP()
	WHERE
		p_index = ?
	`

	a = []any{p_index}

	res, err = DbQuery(q, a)

	if err != nil {

		return fmt.Errorf("failed set termination time: %s", err.Error())

	}

	res.Close()

	return nil

}
