#!/bin/bash



sudo ip netns add net0

sudo ip link add dev veth01 type veth peer name veth02 netns net0

sudo ip link set up veth01

sudo ip netns exec net0 ip link set up veth02

