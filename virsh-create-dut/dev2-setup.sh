#!/bin/bash


sudo ip route add 192.168.102.0/24 via 192.168.101.25

sudo ip netns add vnet2

sudo ip link set enp7s0 netns vnet2

sudo ip netns exec vnet2 ip link set up enp7s0

sudo ip netns exec vnet2 ip addr add 192.168.102.29/24 dev enp7s0

sudo ip netns exec vnet2 ip route add 192.168.101.0/24 via 192.168.102.26