package apply

import (
	"context"
	"fmt"
	"log"
	"time"

	appsv1 "k8s.io/api/apps/v1"
	apiv1 "k8s.io/api/core/v1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/tools/clientcmd"
)

func OutCluster_ApplyDeploymentSample() {

	var kubeconfig *string

	config_path := "config"

	kubeconfig = &config_path

	var replica int32 = 1
	var deployment_name string = "demo-deployment"

	config, err := clientcmd.BuildConfigFromFlags("", *kubeconfig)
	if err != nil {
		panic(err)
	}
	clientset, err := kubernetes.NewForConfig(config)
	if err != nil {
		panic(err)
	}

	deployment_client := clientset.AppsV1().Deployments(apiv1.NamespaceDefault)

	deployment := &appsv1.Deployment{
		ObjectMeta: metav1.ObjectMeta{
			Name: deployment_name,
		},
		Spec: appsv1.DeploymentSpec{
			Replicas: &replica,
			Selector: &metav1.LabelSelector{
				MatchLabels: map[string]string{
					"app": "demo",
				},
			},
			Template: apiv1.PodTemplateSpec{
				ObjectMeta: metav1.ObjectMeta{
					Labels: map[string]string{
						"app": "demo",
					},
				},
				Spec: apiv1.PodSpec{
					Containers: []apiv1.Container{
						{
							Name:  "web",
							Image: "nginx:1.12",
							Ports: []apiv1.ContainerPort{
								{
									Name:          "http",
									Protocol:      apiv1.ProtocolTCP,
									ContainerPort: 80,
								},
							},
						},
					},
				},
			},
		},
	}

	fmt.Println("Creating deployment...")
	result, err := deployment_client.Create(context.TODO(), deployment, metav1.CreateOptions{})
	if err != nil {
		log.Fatalln(err.Error())
	}
	fmt.Printf("Created deployment %q.\n", result.GetObjectMeta().GetName())

	running := 0

	for running < 15 {

		log.Println("checking deployment status...")

		d_name, err := deployment_client.Get(context.TODO(), deployment_name, metav1.GetOptions{})

		if err != nil {
			log.Fatalln(err.Error())
		}

		status := d_name.Status.ReadyReplicas

		log.Println(status)

		if status == replica {

			running = 0

			break

		}

		running += 1

		time.Sleep(time.Second * 1)
	}

	if running == 15 {

		log.Println("watching timeout after creation")

	}

	fmt.Println("Deleting deployment...")

	delete_policy := metav1.DeletePropagationForeground

	err = deployment_client.Delete(
		context.TODO(),
		deployment_name,
		metav1.DeleteOptions{
			PropagationPolicy: &delete_policy,
		})

	if err != nil {
		log.Fatalln(err)
	} else {
		log.Println("operation successful")
	}
}
