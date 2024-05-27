#!/bin/bash


sudo ip addr add 192.168.0.1/24 dev dummethx0

sudo ip addr add 192.168.1.2/24 dev dummethx1

sudo ip link set dev dummethx0 up

sudo ip link set dev dummethx1 up

