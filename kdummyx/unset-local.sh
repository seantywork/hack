#!/bin/bash

sudo ip link set dev dummyx0 down

sudo ip link set dev dummyx1 down

sudo ip addr del 192.168.10.1/24 dev dummyx0

sudo ip addr del 192.168.11.2/24 dev dummyx1
