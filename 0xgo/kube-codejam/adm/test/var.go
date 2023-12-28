package main

import (
	"database/sql"
	"fmt"
	"os"
	"os/exec"
	"strconv"
	"strings"
	"time"

	"0xgo/kube-codejam/adm/querydb"
	"0xgo/kube-codejam/adm/resource"

	_ "github.com/go-sql-driver/mysql"
	"gopkg.in/yaml.v3"
)

var DB *sql.DB

func QueryTestGetPindex() {

	test_res, err := querydb.GetPindexByPowner("f@s")

	if err != nil {
		fmt.Println(err.Error())
		return
	}

	fmt.Println(test_res)

}

func QueryTestSubmitCode() {

	test_res, err := querydb.GetSubmitRecordByPowner("f@s")

	if err != nil {
		fmt.Println(err.Error())
		return
	}

	if test_res[0]["submit_code"] == nil {
		fmt.Println("nil")
	} else {
		fmt.Println(string(test_res[0]["submit_code"].([]uint8)))
	}

}

func QueryTestGetCreationTime() {

	test_res, err := querydb.GetCreationTimeByPowner("f@s")

	if err != nil {
		fmt.Println(err.Error())
		return
	}

	fmt.Println(test_res)

	t_now := time.Now()

	t, _ := time.Parse("2006-01-02 15:04:05", test_res[0]["creation_time"].(string))

	diff := t_now.Sub(t)

	fmt.Println(diff.Seconds())

}

func QueryTestSetDplName() {

	err := querydb.SetJamDeploymentName("f@s", "test-dpl-name-0")

	if err != nil {
		fmt.Println(err.Error())
		return
	}
}

func QueryTestExecStartTime() {

	err := querydb.SetExecStartTime("f@s")

	if err != nil {
		fmt.Println(err.Error())
		return
	}
}

func TestChannelSteps() {

	SIGTERM := make(chan int)

	SIGABRT := make(chan int)

	SIGCONT := make(chan int)

	go TestThread(SIGTERM, SIGABRT, SIGCONT)

	for {

		select {
		case abrt := <-SIGABRT:

			fmt.Println("received abrt: " + strconv.Itoa(abrt))

			SIGTERM <- 1

			fmt.Println("OUT")

			return

		case cont := <-SIGCONT:

			fmt.Println("received cont: " + strconv.Itoa(cont))

		}

	}

}

func TestThread(SIGTERM chan int, SIGABRT chan int, SIGCONT chan int) {

	ticker := time.NewTicker(time.Second)

	counter := 0

	for {

		select {

		case val := <-SIGTERM:

			fmt.Println("received term: " + strconv.Itoa(val))

			return

		case <-ticker.C:

			if counter < 10 {
				SIGCONT <- 1
				counter += 1
			} else {

				SIGABRT <- 1

			}

		}

	}

}

func TestJamResource() {

	submit_lang := "c"

	var P_INDEX int64 = 80303

	var JAM_IMAGE = "harbor.mipllab.com/library/jam-"

	var JAM_USERCONTENT = "usercontent."

	var JAM_IMAGEPULLSECRET = "harbor-secret"

	JAM_DEPLOYMENT_NAME := "dpl-" + submit_lang + "-" + strconv.FormatInt(P_INDEX, 10)

	JAM_CONFIGMAP_NAME := "cmap-" + submit_lang + "-" + strconv.FormatInt(P_INDEX, 10)

	JAM_IMAGE += submit_lang

	JAM_USERCONTENT += submit_lang

	var jam resource.Jam

	var jam_imagepullsecrets resource.Jam_ImagePullSecrets

	var jam_containers resource.Jam_Containers

	var jam_containers_volumemounts resource.Jam_Containers_VolumeMounts

	var jam_containers_ports resource.Jam_Containers_Ports

	var jam_volumes resource.Jam_Volumes

	var jam_volumes_items resource.Jam_Volumes_Items

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
	jam_containers.ImagePullPolicy = "IfNotPresent"

	jam_containers_volumemounts.MountPath = "/tmp"
	jam_containers_volumemounts.Name = JAM_CONFIGMAP_NAME

	jam_containers_ports.ContainerPort = 6700

	jam_containers.VolumeMounts = append(jam_containers.VolumeMounts, jam_containers_volumemounts)
	jam_containers.Ports = append(jam_containers.Ports, jam_containers_ports)

	// 3

	jam_volumes.Name = JAM_CONFIGMAP_NAME
	jam_volumes.ConfigMap.Name = JAM_CONFIGMAP_NAME

	jam_volumes_items.Key = JAM_USERCONTENT
	jam_volumes_items.Path = JAM_USERCONTENT

	jam_volumes.ConfigMap.Items = append(jam_volumes.ConfigMap.Items, jam_volumes_items)

	// 1+2+3

	jam.Spec.Template.Spec.ImagePullSecrets = append(jam.Spec.Template.Spec.ImagePullSecrets, jam_imagepullsecrets)

	jam.Spec.Template.Spec.Containers = append(jam.Spec.Template.Spec.Containers, jam_containers)

	jam.Spec.Template.Spec.Volumes = append(jam.Spec.Template.Spec.Volumes, jam_volumes)

	jam_b, err := yaml.Marshal(jam)

	if err != nil {
		fmt.Println(err.Error())
		return
	}

	fmt.Println(string(jam_b))

}

func TestMkdir() {

	msg := []byte("hello")

	base := "uc"

	cmd := exec.Command("mkdir", "-p", base)

	_, err := cmd.Output()

	if err != nil {
		fmt.Println(err.Error())
		return
	}

	attach := base + "/" + "test"

	cmd = exec.Command("mkdir", attach)

	_, err = cmd.Output()

	if err != nil {
		fmt.Println(err.Error())
		return
	}

	wfile := attach + "/" + "msg_f"

	_ = os.WriteFile(wfile, msg, 0644)

}

func TestGetPod() {

	cmd := exec.Command("kubectl", "get", "nodes", "--no-headers")

	out, err := cmd.Output()

	if err != nil {
		fmt.Println(err.Error())
		return
	}

	out_list := strings.Split(string(out), "\n")

	hit := 0

	res := ""

	for i := 0; i < len(out_list); i++ {

		elems := strings.Split(out_list[i], " ")

		for j := 0; j < len(elems); j++ {

			if strings.Contains(elems[j], "kindcluster-worker") {
				res = elems[j]
				hit = 1
				break

			}

		}

		if hit == 1 {
			break
		}

	}

	if hit == 1 {
		fmt.Println(res)
	} else {
		fmt.Println("not found")
	}

}

func ParseRawJamPodInfo(out_b []byte, key string) (string, error) {

	var res = ""

	out_list := strings.Split(string(out_b), "\n")

	hit := 0

	for i := 0; i < len(out_list); i++ {

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

func TestKubeExec() {

	pod_id := "clusterdebugger-66d784dd5c-vkjrm"

	cmd := exec.Command("kubectl", "get", "pod", pod_id, "--no-headers")

	out, err := cmd.Output()

	if err != nil {
		fmt.Println(err.Error())
		return
	}

	fmt.Println(string(out))

	cmd = exec.Command("kubectl", "exec", "-it", pod_id, "--", "echo", "hello yall!")

	out, err = cmd.Output()

	if err != nil {
		fmt.Println(err.Error())
		return
	}

	fmt.Println("result: " + string(out))

}

func main() {

	var err error = nil

	DB, _ = sql.Open("mysql", "runpayload:youdonthavetoknow@tcp(localhost:33306)/runpayload")

	if err != nil {
		fmt.Printf("db connection failed: %s\n", err.Error())
	}

	DB.SetConnMaxLifetime(time.Second * 100)
	DB.SetConnMaxIdleTime(time.Second * 100)
	DB.SetMaxOpenConns(100)
	DB.SetMaxIdleConns(100)

	/*
		var read string
		TestChannelSteps()

		fmt.Scanln(&read)
	*/

	//TestJamResource()

	TestKubeExec()
}
