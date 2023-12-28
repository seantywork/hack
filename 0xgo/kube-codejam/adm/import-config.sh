#!/bin/bash

set -euxo pipefail

if [ "$EUID" -ne 0 ]
then 
    echo "run as root"
    exit 1
fi




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


echo "success"
