package get

import (
	"context"
	"flag"
	"fmt"
	"log"
	"strconv"

	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/client-go/kubernetes"

	"k8s.io/client-go/tools/clientcmd"
)

func OutCluseter_GetNodeSample() {

	var kubeconfig *string

	config_path := "config"

	kubeconfig = &config_path

	flag.Parse()

	config, err := clientcmd.BuildConfigFromFlags("", *kubeconfig)
	if err != nil {
		log.Fatalln(err.Error())
	}

	clientset, err := kubernetes.NewForConfig(config)
	if err != nil {
		log.Fatalln(err.Error())
	}

	nodes, err := clientset.CoreV1().Nodes().List(context.TODO(), metav1.ListOptions{})

	if err != nil {
		log.Fatalln(err.Error())
	}

	for k, n := range nodes.Items {

		fmt.Println("nodenm: " + strconv.Itoa(k) + "." + n.Name)

	}

}
