#!/bin/bash

set -euxo pipefail


######## wj


HOME="/root" 


sudo swapoff -a


(crontab -l 2>/dev/null; echo "@reboot /sbin/swapoff -a") | crontab - || true
sudo apt-get update -y



#OS="xUbuntu_20.04"

OS="$2"

#VERSION="1.25"

VERSION="$3"

cat <<EOF | sudo tee /etc/modules-load.d/crio.conf
overlay
br_netfilter
EOF

sudo modprobe overlay
sudo modprobe br_netfilter


cat <<EOF | sudo tee /etc/sysctl.d/99-kubernetes-cri.conf
net.bridge.bridge-nf-call-iptables  = 1
net.ipv4.ip_forward                 = 1
net.bridge.bridge-nf-call-ip6tables = 1
EOF

sudo sysctl --system


cat <<EOF | sudo tee /etc/apt/sources.list.d/devel:kubic:libcontainers:stable.list
deb https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/$OS/ /
EOF
cat <<EOF | sudo tee /etc/apt/sources.list.d/devel:kubic:libcontainers:stable:cri-o:$VERSION.list
deb http://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable:/cri-o:/$VERSION/$OS/ /
EOF

curl -L https://download.opensuse.org/repositories/devel:kubic:libcontainers:stable:cri-o:$VERSION/$OS/Release.key | sudo apt-key --keyring /etc/apt/trusted.gpg.d/libcontainers.gpg add -
curl -L https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/$OS/Release.key | sudo apt-key --keyring /etc/apt/trusted.gpg.d/libcontainers.gpg add -

sudo apt-get update
sudo apt-get install cri-o cri-o-runc -y

sudo systemctl daemon-reload
sudo systemctl enable crio --now

echo "Container runtime installed susccessfully"

sudo apt-get update
sudo apt-get install -y apt-transport-https ca-certificates curl

sudo mkdir -p /etc/apt/keyrings


sudo curl -fsSL "https://pkgs.k8s.io/core:/stable:/v$VERSION/deb/Release.key" | sudo gpg --dearmor -o /etc/apt/keyrings/kubernetes-apt-keyring.gpg

echo "deb [signed-by=/etc/apt/keyrings/kubernetes-apt-keyring.gpg] https://pkgs.k8s.io/core:/stable:/v$VERSION/deb/ /" | sudo tee /etc/apt/sources.list.d/kubernetes.list

sudo apt-get update -y
sudo apt-get install -y kubelet kubectl kubeadm
sudo apt-get update -y
sudo apt-get install -y jq



local_ip=$1
cat > /etc/default/kubelet << EOF
KUBELET_EXTRA_ARGS=--node-ip=$local_ip
EOF


MASTER_IP=$local_ip
LB_IP=$local_ip
IP_NO_DOT=$(echo "$local_ip" | sed "s/\./-/g")
PORT="6443"
NODENAME="$(hostname -s)"
NODENAME="lead-$NODENAME-$IP_NO_DOT-$(openssl rand -hex 8)"
POD_CIDR="10.10.0.0/16"

sudo kubeadm config images pull 

echo "Preflight Check Passed: Downloaded All Required Images"

sudo kubeadm init --apiserver-advertise-address=$MASTER_IP --apiserver-cert-extra-sans="$MASTER_IP,$LB_IP" --pod-network-cidr=$POD_CIDR --node-name "$NODENAME" --control-plane-endpoint "$LB_IP:$PORT" --ignore-preflight-errors Swap 
mkdir -p "$HOME"/.kube
sudo cp -i /etc/kubernetes/admin.conf "$HOME"/.kube/config
sudo chown "$(id -u)":"$(id -g)" "$HOME"/.kube/config



kubectl apply -f https://raw.githubusercontent.com/projectcalico/calico/v3.25.0/manifests/calico.yaml

kubectl apply -f https://raw.githubusercontent.com/kubernetes/ingress-nginx/main/deploy/static/provider/kind/deploy.yaml



kubectl label node $NODENAME ingress-ready=true 