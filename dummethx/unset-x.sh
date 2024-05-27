#!/bin/bash


sudo ip link set dev dummethx0 down

sudo ip link set dev dummethx1 down

sudo ip addr del 192.168.0.1/24 dev dummethx0

sudo ip addr del 192.168.1.2/24 dev dummethx1



