#!/bin/bash


# delete rules 


sudo iptables -t nat -D POSTROUTING -o deth0 -p tcp --dport 8000 -d 192.168.1.100 -j SNAT --to-source 192.168.50.24:8888


sudo iptables -t nat -D PREROUTING -i wlo1 -p tcp --dport 8888 -j DNAT --to-destination 192.168.1.100:8000


sudo iptables -D FORWARD -i deth0 -o wlo1 -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT

sudo iptables -D FORWARD -i wlo1 -o deth0 -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT

sudo iptables -D FORWARD -i wlo1 -o deth0 -p tcp --syn --dport 8888 -m conntrack --ctstate NEW -j ACCEPT


echo 0 | sudo tee /proc/sys/net/ipv4/ip_forward



# destroy d_if

sudo ip link set dev deth0 down

sudo ip addr del 192.168.1.100/24 brd + dev deth0 # label deth0:0

sudo ip link delete deth0 type dummy

sudo modprobe -r dummy
