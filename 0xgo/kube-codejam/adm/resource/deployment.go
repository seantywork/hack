package resource

type Jam struct {
	APIVersion string `yaml:"apiVersion"`
	Kind       string `yaml:"kind"`
	Metadata   struct {
		Name string `yaml:"name"`
	} `yaml:"metadata"`
	Spec struct {
		Selector struct {
			MatchLabels struct {
				App string `yaml:"app"`
			} `yaml:"matchLabels"`
		} `yaml:"selector"`
		Replicas int `yaml:"replicas"`
		Template struct {
			Metadata struct {
				Labels struct {
					App string `yaml:"app"`
				} `yaml:"labels"`
			} `yaml:"metadata"`
			Spec struct {
				ImagePullSecrets []Jam_ImagePullSecrets `yaml:"imagePullSecrets"`
				Containers       []Jam_Containers       `yaml:"containers"`
				Volumes          []Jam_Volumes          `yaml:"volumes"`
			} `yaml:"spec"`
		} `yaml:"template"`
	} `yaml:"spec"`
}

type Jam_ImagePullSecrets struct {
	Name string `yaml:"name"`
}

type Jam_Containers struct {
	Name            string                        `yaml:"name"`
	Image           string                        `yaml:"image"`
	VolumeMounts    []Jam_Containers_VolumeMounts `yaml:"volumeMounts"`
	ImagePullPolicy string                        `yaml:"imagePullPolicy"`
	Ports           []Jam_Containers_Ports        `yaml:"ports"`
}

type Jam_Containers_VolumeMounts struct {
	MountPath string `yaml:"mountPath"`
	Name      string `yaml:"name"`
}

type Jam_Containers_Ports struct {
	ContainerPort int `yaml:"containerPort"`
}

type Jam_Volumes struct {
	Name      string `yaml:"name"`
	ConfigMap struct {
		Name  string              `yaml:"name"`
		Items []Jam_Volumes_Items `yaml:"items"`
	} `yaml:"configMap"`
}

type Jam_Volumes_Items struct {
	Key  string `yaml:"key"`
	Path string `yaml:"path"`
}
