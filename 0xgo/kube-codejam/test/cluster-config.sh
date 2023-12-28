#!/bin/bash

set -euxo pipefail

if [ "$EUID" -ne 0 ]
then 
    echo "run as root"
    exit 1
fi

if [ -z "$1" ]
then
    echo "feed target"
    echo "-- import"
    echo "-- set"
    echo "-- block"
    echo "-- debug"
    exit 1
fi




if [ "$1" = "import" ]
then

    TARGET_USER=""

    TARGET_ADDRESS=""

    TARGET_PORT=""

    read -p "Target User: " TARGET_USER

    read -p "Target Address: " TARGET_ADDRESS

    read -p "Target Port: " TARGET_PORT
 
    echo "moving to importable path..."

    ssh -t "$TARGET_USER"@"$TARGET_ADDRESS" -p "$TARGET_PORT" "sudo /bin/cp -Rf /root/.kube/config /tmp/config && sudo chown $TARGET_USER:$TARGET_USER /tmp/config"

    echo "importing profile..."

    scp -P "$TARGET_PORT" "$TARGET_USER"@"$TARGET_ADDRESS":/tmp/config .

    echo "removing imported profile from remote host..."

    ssh -t "$TARGET_USER"@"$TARGET_ADDRESS" -p "$TARGET_PORT" "sudo rm -r /tmp/config"

    echo "moving imported profile to an effective location"

    mkdir -p /root/.kube

    /bin/cp -Rf config /root/.kube/config

    rm -r config

    echo "success"

elif [ "$1" = "set" ]
then

    docker login harbor.mipllab.com

    kubectl create namespace run-payload-jam

    kubectl -n run-payload-jam create secret generic harbor-secret \
            --from-file=.dockerconfigjson=/root/.docker/config.json \
            --type=kubernetes.io/dockerconfigjson 


elif [ "$1" = "block" ]
then

    kubectl -n run-payload-jam apply -f ./cluster/net-block.yaml


elif [ "$1" = "debug" ]
then

    kubectl -n run-payload-jam create configmap debugger-mount --from-file=./cluster/usercontent.c --from-file=./cluster/usercontent.input

    kubectl -n run-payload-jam apply -f ./cluster/debugger.yaml

    kubectl -n run-payload-jam get pods

    echo "kubectl -n run-payload-jam exec -it <debugger-pod-id> -- /bin/bash"

elif [ "$1" = "jam-test" ]
then

    kubectl -n run-payload-jam create configmap jam-test-mount --from-file=./cluster/usercontent.c --from-file=./cluster/usercontent.input

    kubectl -n run-payload-jam apply -f ./cluster/jam-test.yaml

    kubectl -n run-payload-jam get pods

    echo "kubectl -n run-payload-jam exec -it <jam-test-pod-id> -- /bin/bash"


else

    echo "wrong target: $1"
    exit 1
fi