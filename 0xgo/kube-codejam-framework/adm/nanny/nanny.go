package nanny

import (
	"os/exec"
	"strconv"
	"time"

	"0xgo/kube-codejam-framework/adm/querydb"
	"0xgo/kube-codejam-framework/adm/utils"
)

type RowInterface map[string]interface{}

func BetterListenToMe(p_index int64, SIGABRT chan int, SIGCONT chan int) {

	police_flag := 0

	ineffective_flag := 0

	for {

		container, err := querydb.GetOrphanedRecordByPowner(p_index)

		if err != nil {

			utils.EventLogger("nanny error: " + err.Error())

			police_flag = 1
		}

		t_now := time.Now()

		t, _ := time.Parse("2006-01-02 15:04:05", string(container[0]["creation_time"].([]uint8)))

		diff := t_now.Sub(t)

		if diff.Milliseconds() > 15000 {
			police_flag = 1
		}

		if container[0]["effective"].(int64) != 1 {
			ineffective_flag = 1
		}

		if ineffective_flag != 0 {

			return

		} else if police_flag != 0 {

			SIGABRT <- 1

			return

		} else {

			SIGCONT <- 1

		}
	}

}

func OrDie(p_index int64) {

	pidx_str := strconv.Itoa(int(p_index))

	utils.EventLogger("nanny: or die: pindex: " + pidx_str)

	container, err := querydb.GetOrphanedRecordByPowner(p_index)

	if err != nil {

		utils.EventLogger("WARN::GETQUERY::" + pidx_str)
		utils.EventLogger(err.Error())
	}

	utils.EventLogger("nanny: or die: total length: " + strconv.Itoa(len(container)))

	for i := 0; i < len(container); i++ {

		utils.EventLogger("nanny: or die: container: " + strconv.Itoa(i))

		p_index := container[i]["p_index"].(int64)

		jam_deployment_name := string(container[i]["jam_deployment_name"].([]uint8))

		jam_configmap_name := string(container[i]["jam_configmap_name"].([]uint8))

		err = querydb.CleanUpOrphanedRecord(p_index)

		if err != nil {

			utils.EventLogger("WARN::SETQUERY::" + pidx_str)
			utils.EventLogger(err.Error())

		}

		cmd := exec.Command("kubectl", "-n", "run-payload-jam", "delete", "deployment", jam_deployment_name)

		_, err := cmd.Output()

		if err != nil {
			utils.EventLogger("WARN::JAMDPL::" + pidx_str)
			utils.EventLogger(err.Error())
		}

		cmd = exec.Command("kubectl", "-n", "run-payload-jam", "delete", "configmap", jam_configmap_name)

		_, err = cmd.Output()

		if err != nil {
			utils.EventLogger("WARN::JAMCFG::" + pidx_str)
			utils.EventLogger(err.Error())
		}

	}

}
