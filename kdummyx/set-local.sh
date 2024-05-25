#!/bin/bash

sudo ip addr add 192.168.10.1/24 dev dummyx0

sudo ip addr add 192.168.11.2/24 dev dummyx1

sudo ip link set dev dummyx0 up

sudo ip link set dev dummyx1 up