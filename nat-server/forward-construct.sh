#!/bin/bash

# create d_if

sudo modprobe dummy

sudo ip link add deth0 type dummy

sudo ip link set dev deth0 address C8:D7:4A:4E:47:50

sudo ip addr add 192.168.1.100/24 brd + dev deth0 # label deth0:0

sudo ip link set dev deth0 up


# ephemeral
echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward

sudo iptables -A FORWARD -i wlo1 -o deth0 -p tcp --syn --dport 8888 -m conntrack --ctstate NEW -j ACCEPT

sudo iptables -A FORWARD -i wlo1 -o deth0 -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT

sudo iptables -A FORWARD -i deth0 -o wlo1 -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT

# sudo iptables -P FORWARD DROP

sudo iptables -t nat -A PREROUTING -i wlo1 -p tcp --dport 8888 -j DNAT --to-destination 192.168.1.100:8000

sudo iptables -t nat -A POSTROUTING -o deth0 -p tcp --dport 8000 -d 192.168.1.100 -j SNAT --to-source 192.168.50.24:8888