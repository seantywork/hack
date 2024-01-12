package apply

import (
	"context"
	"fmt"
	"log"
	"time"

	apiv1 "k8s.io/api/core/v1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/apimachinery/pkg/util/intstr"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/tools/clientcmd"
)

func OutCluster_ApplyServiceSample() {
	var kubeconfig *string

	config_path := "config"

	kubeconfig = &config_path

	var service_name string = "demo-service"

	config, err := clientcmd.BuildConfigFromFlags("", *kubeconfig)
	if err != nil {
		panic(err)
	}
	clientset, err := kubernetes.NewForConfig(config)
	if err != nil {
		panic(err)
	}

	service_client := clientset.CoreV1().Services("default")

	service := &apiv1.Service{
		ObjectMeta: metav1.ObjectMeta{
			Name: service_name,
			Labels: map[string]string{
				"app": "demo",
			},
		},
		Spec: apiv1.ServiceSpec{
			Type: apiv1.ServiceTypeClusterIP,
			Ports: []apiv1.ServicePort{
				{
					Port: 80,
					TargetPort: intstr.IntOrString{
						IntVal: 80,
					},
					Protocol: apiv1.ProtocolTCP,
				},
			},
			Selector: map[string]string{
				"app": "demo",
			},
		},
	}

	fmt.Println("Creating service..")

	result, err := service_client.Create(context.TODO(), service, metav1.CreateOptions{})

	if err != nil {
		log.Fatalln(err.Error())
	}
	fmt.Printf("Created service %q.\n", result.GetObjectMeta().GetName())

	running := 0

	for running < 10 {

		log.Println("checking service status...")

		s_name, err := service_client.Get(context.TODO(), service_name, metav1.GetOptions{})

		if err != nil {
			log.Fatalln(err.Error())
		}

		fmt.Println("service name and creation timestamp retrieved: \n\t\t" + s_name.Name + ": " + s_name.CreationTimestamp.String())

		running += 1

		time.Sleep(time.Second * 1)
	}

	fmt.Println("Deleting service...")

	delete_policy := metav1.DeletePropagationForeground

	err = service_client.Delete(
		context.TODO(),
		service_name,
		metav1.DeleteOptions{
			PropagationPolicy: &delete_policy,
		},
	)

	if err != nil {
		log.Fatalln(err)
	} else {
		log.Println("operation successful")
	}
}
