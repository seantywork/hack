package main

import (
	"log"
	"os"

	kubeapply "github.com/seantywork/0xgo/kube-api/apply"
	kubeget "github.com/seantywork/0xgo/kube-api/get"
)

func main() {

	if len(os.Args) != 2 {
		log.Fatalln("action not supplied")
	}

	opt := os.Args[1]

	switch opt {

	case "get-pod":

		kubeget.OutCluster_GetPodsSample()

	case "get-node":

		kubeget.OutCluseter_GetNodeSample()

	case "apply-service":

		kubeapply.OutCluster_ApplyServiceSample()

	case "apply-deployment":

		kubeapply.OutCluster_ApplyDeploymentSample()

	default:

		log.Fatalln("no such option: " + opt)

	}

}
