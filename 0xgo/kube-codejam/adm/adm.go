package main

import (
	"net/http"
	"os"
	"os/exec"
	"strconv"
	"time"

	"gopkg.in/yaml.v3"

	"0xgo/kube-codejam/adm/nanny"
	_ "0xgo/kube-codejam/adm/nanny"
	"0xgo/kube-codejam/adm/querydb"
	"0xgo/kube-codejam/adm/resource"
	"0xgo/kube-codejam/adm/utils"

	"github.com/gorilla/websocket"
)

func RunPayloadAdm(w http.ResponseWriter, r *http.Request) {

	CONFIG_JSON := resource.GetConfig()

	var STEP int = 0
	var STEPBIN resource.StepBin

	SIGABRT := make(chan int)
	SIGCONT := make(chan int)

	var P_INDEX int64
	var P_OWNER string
	var JAM_DEPLOYMENT_NAME string
	var JAM_DEPLOYMENT_YAML string
	var JAM_CONFIGMAP_NAME string

	var JAM_IMAGEPULLSECRET = CONFIG_JSON["SECRET_NAME"]
	var JAM_IMAGE = CONFIG_JSON["REGISTRY_ADDRESS"]
	var JAM_USERCONTENT = "usercontent."

	// jam deployment name : dpl-{lang}-{version}-{pidx}
	// jam configmap name: cmap-{lang}-{version}-{pidx}

	utils.EventLogger("run-payload access")

	UPGRADER.CheckOrigin = func(r *http.Request) bool { return true }

	c, err := UPGRADER.Upgrade(w, r, nil)
	if err != nil {
		utils.EventLogger("upgrade:" + err.Error())
		return
	}

	c.SetReadDeadline(time.Time{})

	var req resource.FrontRequest

	err = c.ReadJSON(&req)
	if err != nil {
		utils.EventLogger("auth:" + err.Error())
		return
	}

	P_OWNER = req.CODE_OWNER

	res, err := querydb.GetPindexAndVerifiedFlag(P_OWNER)

	if err != nil {
		utils.EventLogger("verify:" + err.Error())

		resp := resource.FrontResponse{
			STATUS:  "N",
			MESSAGE: "Error 1000: illegal access",
		}

		_ = c.WriteJSON(&resp)

		_ = c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, "Connection Close"))

		return

	}

	P_INDEX = res[0]["p_index"].(int64)

	go nanny.BetterListenToMe(P_INDEX, SIGABRT, SIGCONT)

	res, err = querydb.GetSubmitRecordByPowner(P_OWNER)

	if err != nil {
		utils.EventLogger("submit code:" + err.Error())

		resp := resource.FrontResponse{
			STATUS:  "N",
			MESSAGE: "Error 1002: illegal access",
		}

		_ = c.WriteJSON(&resp)

		_ = c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, "Connection Close"))

		nanny.OrDie(P_INDEX)

		return

	}

	resp := resource.FrontResponse{
		STATUS:  "Y",
		MESSAGE: "Verified! Preparing your jam...",
	}

	err = c.WriteJSON(&resp)

	if err != nil {
		utils.EventLogger("error sending verification:" + err.Error())
		_ = c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, "Connection Close"))
		nanny.OrDie(P_INDEX)
		return
	}

	submit_lang := string(res[0]["submit_lang"].([]uint8))

	submit_version := string(res[0]["submit_version"].([]uint8))

	submit_code := string(res[0]["submit_code"].([]uint8))

	submit_input := string(res[0]["submit_input"].([]uint8))

	JAM_DEPLOYMENT_NAME = "dpl-" + submit_lang + "-" + submit_version + "-" + strconv.FormatInt(P_INDEX, 10)

	JAM_CONFIGMAP_NAME = "cmap-" + submit_lang + "-" + submit_version + "-" + strconv.FormatInt(P_INDEX, 10)

	JAM_IMAGE += submit_lang + "-" + submit_version

	JAM_USERCONTENT_CODE := JAM_USERCONTENT + submit_lang

	JAM_USERCONTENT_INPUT := JAM_USERCONTENT + "input"

	var jam resource.Jam

	var jam_imagepullsecrets resource.Jam_ImagePullSecrets

	var jam_containers resource.Jam_Containers

	var jam_containers_volumemounts resource.Jam_Containers_VolumeMounts

	var jam_containers_ports resource.Jam_Containers_Ports

	var jam_volumes resource.Jam_Volumes

	// var jam_volumes_items resource.Jam_Volumes_Items

	jam.APIVersion = "apps/v1"
	jam.Kind = "Deployment"
	jam.Metadata.Name = JAM_DEPLOYMENT_NAME
	jam.Spec.Selector.MatchLabels.App = JAM_DEPLOYMENT_NAME
	jam.Spec.Replicas = 1
	jam.Spec.Template.Metadata.Labels.App = JAM_DEPLOYMENT_NAME

	// 1
	jam_imagepullsecrets.Name = JAM_IMAGEPULLSECRET

	// 2

	jam_containers.Name = JAM_DEPLOYMENT_NAME
	jam_containers.Image = JAM_IMAGE
	jam_containers.ImagePullPolicy = "Always"

	jam_containers_volumemounts.MountPath = "/tmp"
	jam_containers_volumemounts.Name = JAM_CONFIGMAP_NAME

	jam_containers_ports.ContainerPort = 6700

	jam_containers.VolumeMounts = append(jam_containers.VolumeMounts, jam_containers_volumemounts)
	jam_containers.Ports = append(jam_containers.Ports, jam_containers_ports)

	// 3

	jam_volumes.Name = JAM_CONFIGMAP_NAME
	jam_volumes.ConfigMap.Name = JAM_CONFIGMAP_NAME

	jam_volumes.ConfigMap.Items = append(jam_volumes.ConfigMap.Items, resource.Jam_Volumes_Items{
		Key:  JAM_USERCONTENT_CODE,
		Path: JAM_USERCONTENT_CODE,
	})

	jam_volumes.ConfigMap.Items = append(jam_volumes.ConfigMap.Items, resource.Jam_Volumes_Items{
		Key:  JAM_USERCONTENT_INPUT,
		Path: JAM_USERCONTENT_INPUT,
	})

	// 1+2+3

	jam.Spec.Template.Spec.ImagePullSecrets = append(jam.Spec.Template.Spec.ImagePullSecrets, jam_imagepullsecrets)

	jam.Spec.Template.Spec.Containers = append(jam.Spec.Template.Spec.Containers, jam_containers)

	jam.Spec.Template.Spec.Volumes = append(jam.Spec.Template.Spec.Volumes, jam_volumes)

	jam_b, err := yaml.Marshal(jam)

	if err != nil {
		utils.EventLogger("prepare jam source:" + err.Error())

		resp := resource.FrontResponse{
			STATUS:  "N",
			MESSAGE: "Error 1003: illegal content",
		}

		_ = c.WriteJSON(&resp)

		_ = c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, "Connection Close"))

		nanny.OrDie(P_INDEX)

		return
	}

	jam_yaml := string(jam_b)

	JAM_DEPLOYMENT_YAML = jam_yaml

	user_dir := USERCONTENT_DIR + strconv.FormatInt(P_INDEX, 10)

	cmd := exec.Command("mkdir", user_dir)

	_, err = cmd.Output()

	if err != nil {

		utils.EventLogger("prepare jam dir" + err.Error())

		resp := resource.FrontResponse{
			STATUS:  "N",
			MESSAGE: "Error 1004: illegal content",
		}

		_ = c.WriteJSON(&resp)

		_ = c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, "Connection Close"))

		nanny.OrDie(P_INDEX)

		return

	}

	user_dir += "/"

	user_deployment := user_dir + "deployment.yaml"
	usercontent_code := user_dir + JAM_USERCONTENT_CODE
	usercontent_input := user_dir + JAM_USERCONTENT_INPUT

	err = os.WriteFile(user_deployment, []byte(JAM_DEPLOYMENT_YAML), 0644)

	if err != nil {

		utils.EventLogger("prepare jam deployment:" + err.Error())

		resp := resource.FrontResponse{
			STATUS:  "N",
			MESSAGE: "Error 1005: illegal content",
		}

		_ = c.WriteJSON(&resp)

		_ = c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, "Connection Close"))

		nanny.OrDie(P_INDEX)

		return

	}

	err = os.WriteFile(usercontent_code, []byte(submit_code), 0644)

	if err != nil {

		utils.EventLogger("prepare jam configmap code:" + err.Error())

		resp := resource.FrontResponse{
			STATUS:  "N",
			MESSAGE: "Error 1006: illegal content",
		}

		_ = c.WriteJSON(&resp)

		_ = c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, "Connection Close"))

		nanny.OrDie(P_INDEX)

		return

	}

	err = os.WriteFile(usercontent_input, []byte(submit_input), 0644)

	if err != nil {

		utils.EventLogger("prepare jam configmap input:" + err.Error())

		resp := resource.FrontResponse{
			STATUS:  "N",
			MESSAGE: "Error 10061: illegal content",
		}

		_ = c.WriteJSON(&resp)

		_ = c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, "Connection Close"))

		nanny.OrDie(P_INDEX)

		return

	}

	resp = resource.FrontResponse{
		STATUS:  "Y",
		MESSAGE: "Prepared! Deploying your jam...",
	}

	err = c.WriteJSON(&resp)

	if err != nil {
		utils.EventLogger("error sending preparation:" + err.Error())
		_ = c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, "Connection Close"))
		nanny.OrDie(P_INDEX)
		return
	}

	STEPBIN.P_INDEX = P_INDEX
	STEPBIN.P_OWNER = P_OWNER
	STEPBIN.JAM_DEPLOYMENT_NAME = JAM_DEPLOYMENT_NAME
	STEPBIN.JAM_DEPLOYMENT_YAML = JAM_DEPLOYMENT_YAML
	STEPBIN.JAM_CONFIGMAP_NAME = JAM_CONFIGMAP_NAME
	STEPBIN.USERCONTENT_DPL = user_deployment
	STEPBIN.USERCONTENT_CODE = usercontent_code
	STEPBIN.USERCONTENT_INPUT = usercontent_input

	for {

		select {

		case abrt := <-SIGABRT:

			utils.EventLogger("abort:" + strconv.Itoa(abrt))

			resp := resource.FrontResponse{
				STATUS:  "N",
				MESSAGE: "Error 1007: operation took too long",
			}

			_ = c.WriteJSON(&resp)

			_ = c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, "Connection Close"))

			nanny.OrDie(P_INDEX)

			return

		case <-SIGCONT:

			terminate, result, err := OperationStepForwarder(&STEP, &STEPBIN)

			if err != nil {

				utils.EventLogger("operation err:" + err.Error())

				resp := resource.FrontResponse{
					STATUS:  "N",
					MESSAGE: err.Error(),
				}

				_ = c.WriteJSON(&resp)

				_ = c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, "Connection Close"))

				nanny.OrDie(P_INDEX)

				return

			}

			if terminate != 1 {

				resp := resource.FrontResponse{
					STATUS:  "Y",
					MESSAGE: result,
				}

				_ = c.WriteJSON(&resp)

			} else {

				utils.EventLogger("operation finished")

				resp := resource.FrontResponse{
					STATUS:   "BYE",
					MESSAGE:  result,
					RESOURCE: STEPBIN.RUNOUT,
					STDOUT:   STEPBIN.OUTOUT,
					STDERR:   STEPBIN.ERROUT,
				}

				_ = c.WriteJSON(&resp)

				_ = c.WriteMessage(websocket.CloseMessage, websocket.FormatCloseMessage(websocket.CloseNormalClosure, "Connection Close"))

				nanny.OrDie(P_INDEX)

				return

			}

		}

	}

}
