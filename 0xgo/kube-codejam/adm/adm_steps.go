package main

import (
	"fmt"
	"os/exec"
	"strings"

	"github.com/seantywork/0xgo/kube-codejam/adm/querydb"
	"github.com/seantywork/0xgo/kube-codejam/adm/resource"
	"github.com/seantywork/0xgo/kube-codejam/adm/utils"
)

func OperationStepForwarder(STEP *int, stepbin *resource.StepBin) (int, string, error) {

	var term = 0
	var res string = ""

	step_val := (*STEP)

	switch step_val {

	// db register
	case 0:

		p_owner := stepbin.P_OWNER

		jam_dpl_name := stepbin.JAM_DEPLOYMENT_NAME
		jam_dpl_yaml := stepbin.JAM_DEPLOYMENT_YAML
		jam_cmap_name := stepbin.JAM_CONFIGMAP_NAME

		err := querydb.SetJamDeploymentName(p_owner, jam_dpl_name)

		if err != nil {
			utils.EventLogger("step 0: " + err.Error())
			return term, res, fmt.Errorf("Error 1008: %s", "internal error")
		}

		err = querydb.SetJamDeploymentYaml(p_owner, jam_dpl_yaml)

		if err != nil {
			utils.EventLogger("step 0: " + err.Error())
			return term, res, fmt.Errorf("Error 1009: %s", "internal error")
		}

		err = querydb.SetJamConfigmapName(p_owner, jam_cmap_name)

		if err != nil {
			utils.EventLogger("step 0: " + err.Error())
			return term, res, fmt.Errorf("Error 1010: %s", "internal error")
		}

		res = "Deploying in progress... creating code jam..."

		(*STEP)++

	// deploy and set exec time
	case 1:

		p_owner := stepbin.P_OWNER
		jam_dpl_name := stepbin.JAM_DEPLOYMENT_NAME
		jam_cmap_name := stepbin.JAM_CONFIGMAP_NAME

		f_dpl := stepbin.USERCONTENT_DPL
		f_code := stepbin.USERCONTENT_CODE
		f_input := stepbin.USERCONTENT_INPUT

		cmd := exec.Command("kubectl", "-n", "run-payload-jam", "create", "configmap", jam_cmap_name, "--from-file="+f_code, "--from-file="+f_input)

		_, err := cmd.Output()

		if err != nil {
			utils.EventLogger("step 1: " + err.Error())
			return term, res, fmt.Errorf("Error 1011: %s", "internal error")
		}

		cmd = exec.Command("kubectl", "-n", "run-payload-jam", "apply", "-f", f_dpl)

		_, err = cmd.Output()

		if err != nil {
			utils.EventLogger("step 1: " + err.Error())
			return term, res, fmt.Errorf("Error 1012: %s", "internal error")
		}

		cmd = exec.Command("kubectl", "-n", "run-payload-jam", "get", "pods", "--no-headers")

		out, err := cmd.Output()

		if err != nil {
			utils.EventLogger("step 1: " + err.Error())
			return term, res, fmt.Errorf("Error 1013: %s", "internal error")
		}

		podnm, err := ParseRawJamPodInfo(out, jam_dpl_name)

		if err != nil {
			utils.EventLogger("step 1: " + err.Error())
			return term, res, fmt.Errorf("Error 1014: %s", err.Error())
		}

		stepbin.JAM_POD_NAME = podnm

		err = querydb.SetExecStartTime(p_owner)

		if err != nil {
			utils.EventLogger("step 1: " + err.Error())
			return term, res, fmt.Errorf("Error 1015: %s", "internal error")
		}

		res = "Deployed! Your program will be run shortly..."

		(*STEP)++

	// watch if running

	case 2:

		pod_id := stepbin.JAM_POD_NAME

		cmd := exec.Command("kubectl", "-n", "run-payload-jam", "get", "pods", pod_id, "--no-headers")

		out, err := cmd.Output()

		if err != nil {
			utils.EventLogger("step 2: " + err.Error())
			return term, res, fmt.Errorf("Error 1115: %s", "internal error")
		}

		_, err = ParseRawJamPodInfo(out, "Running")

		if err != nil {
			res := "Creating..."
			return term, res, nil
		}

		utils.EventLogger("created pod id:" + pod_id)

		(*STEP)++

	// watch if finished

	case 3:

		pod_id := stepbin.JAM_POD_NAME

		cmd := exec.Command("kubectl", "-n", "run-payload-jam", "exec", "-it", pod_id, "--", "cat", "/home/prog")

		out, err := cmd.Output()

		if err != nil {
			utils.EventLogger("step 3: " + err.Error())
			return term, res, fmt.Errorf("Error 1016: %s", "internal error")
		}

		_, err = ParseRawJamProgInfo(out, "Completed")

		if err != nil {
			res := "Running..."
			return term, res, nil
		}

		res = "Completed! Collecting results..."

		(*STEP)++

	// get logs, save, send
	case 4:

		p_owner := stepbin.P_OWNER
		pod_id := stepbin.JAM_POD_NAME

		cmd := exec.Command("kubectl", "-n", "run-payload-jam", "exec", "-it", pod_id, "--", "cat", "/home/run")

		runout, err := cmd.Output()

		if err != nil {
			utils.EventLogger("step 4: " + err.Error())
			return term, res, fmt.Errorf("Error 1017: %s", "internal error")
		}

		cmd = exec.Command("kubectl", "-n", "run-payload-jam", "exec", "-it", pod_id, "--", "cat", "/home/out")

		outout, err := cmd.Output()

		if err != nil {
			utils.EventLogger("step 4: " + err.Error())
			return term, res, fmt.Errorf("Error 1018: %s", "internal error")
		}

		cmd = exec.Command("kubectl", "-n", "run-payload-jam", "exec", "-it", pod_id, "--", "cat", "/home/err")

		errout, err := cmd.Output()

		if err != nil {
			utils.EventLogger("step 4: " + err.Error())
			return term, res, fmt.Errorf("Error 1019: %s", "internal error")
		}

		stepbin.RUNOUT = string(runout)
		stepbin.OUTOUT = string(outout)
		stepbin.ERROUT = string(errout)

		err = querydb.SetLogs(p_owner, stepbin.RUNOUT, stepbin.OUTOUT, stepbin.ERROUT)

		if err != nil {
			utils.EventLogger("step 4: " + err.Error())
			return term, res, fmt.Errorf("Error 1020: %s", "internal error")
		}

		err = querydb.SetTerminationTime(p_owner)

		if err != nil {
			utils.EventLogger("step 4: " + err.Error())
			return term, res, fmt.Errorf("Error 1021: %s", "internal error")
		}

		res = "Successfully ran payload"

		term = 1
	}

	return term, res, nil
}

func ParseRawJamProgInfo(out_b []byte, key string) (string, error) {

	var res = ""

	out_list := strings.Split(string(out_b), "\n")

	hit := 0

	for i := 0; i < len(out_list); i++ {

		if strings.Contains(out_list[i], key) {
			res = out_list[i]
			hit = 1
			break

		}

	}

	if hit != 1 {
		return res, fmt.Errorf("identifier not found")
	}

	return res, nil
}

func ParseRawJamPodInfo(out_b []byte, key string) (string, error) {

	var res = ""

	out_list := strings.Split(string(out_b), "\n")

	hit := 0

	for i := 0; i < len(out_list); i++ {

		if out_list[i] == "" || out_list[i] == "\n" {
			continue
		}

		elems := strings.Split(out_list[i], " ")

		for j := 0; j < len(elems); j++ {

			if strings.Contains(elems[j], key) {
				res = elems[j]
				hit = 1
				break

			}

		}

		if hit == 1 {
			break
		}

	}

	if hit != 1 {
		return res, fmt.Errorf("identifier not found")
	}

	return res, nil
}
