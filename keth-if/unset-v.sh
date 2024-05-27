#!/bin/bash


sudo iptables -t nat -D POSTROUTING -o br-dumm -j MASQUERADE

sudo iptables -t nat -D POSTROUTING -o enp1s0 -j MASQUERADE

sudo ip netns del dummnet

sudo ip link set br-dumm down

sudo ip link set vmdumm1 down

sudo ip link set vmdumm2 down

sudo ip link del vmdumm2

sudo ip link del vmdumm1

sudo ip link del br-dumm

