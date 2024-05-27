#!/bin/bash

sudo ip link add br-dumm type bridge 

sudo ip link add dev vmdumm1 type veth peer name vmdumm2

sudo ip link set vmdumm1 master br-dumm

sudo ip addr add 10.0.0.1/24 dev br-dumm

sudo ip addr add 10.0.0.2/24 dev vmdumm2

sudo ip link set br-dumm up

sudo ip link set vmdumm1 up

sudo ip link set vmdumm2 up

sudo ip netns add dummnet

sudo ip link set vmdumm2 netns dummnet 

sudo ip netns exec dummnet ip link set dev lo up

sudo iptables -t nat -A POSTROUTING -o br-dumm -j MASQUERADE

sudo iptables -t nat -A POSTROUTING -o enp1s0 -j MASQUERADE

sudo ip netns exec dummnet ip addr add 10.0.0.2/24 dev vmdumm2

sudo ip netns exec dummnet ip link set dev vmdumm2 up

sudo ip netns exec dummnet ip route add default via 10.0.0.1

sudo ip netns exec dummnet /bin/bash

#echo "nameserver 8.8.8.8" >> /etc/resolv.conf
#echo "nameserver 8.8.4.4" >> /etc/resolv.conf