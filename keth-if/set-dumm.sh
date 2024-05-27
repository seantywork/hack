#!/bin/bash



sudo ip addr add 192.168.10.1/24 dev dummeth0


sudo ip link set dev dummeth0 up

