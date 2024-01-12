#!/bin/bash

THISUSER="$(whoami)"

sudo kind create cluster --name kindcluster --config ./kindcluster.yaml --image=kindest/node:v1.27.2

sudo cp /root/.kube/config .

sudo chown "$THISUSER:$THISUSE" config

