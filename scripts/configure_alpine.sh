# bin/bash
apk add build-base

# A:
# ip addr add 100.1.0.1/24 dev eth0
# apk add build-base

# B:
# ip addr add 192.168.0.1/16 dev eth0
# apk add build-base

# Rendzvous:
# ip addr add 100.1.0.3/24 dev eth0
# apk add build-base

# NAT:
# ip addr add 100.1.0.2/24 dev eth0
# ip addr add 192.168.0.2/16 dev eth1

# iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
# iptables -A INPUT -i eth0 -m state --state ESTABLISHED,RELATED -j ACCEPT
# iptables -A INPUT -i eth0 -j DROP
