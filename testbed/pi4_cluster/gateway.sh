# Keybaord layout
sudo dpkg-reconfigure keyboard-configuration

# SSH
sudo systemctl enable ssh

# Hostname
sudo nano /etc/hostname
sudo nano /etc/hosts

# IP forwarding
#sudo sysctl -w net.ipv4.ip_forward=1
sudo /etc/sysctl.conf # Enable ipv4 forwarding

# VLAN subinterface
# sudo apt install -y vlan
# sudo vconfig add eth0 10
# sudo ip addr add 10.0.0.1/24 dev eth0.10
# sudo ip link set up eth0.10
# sudo su -c 'echo "8021q" >> /etc/modules'

# Subinterface used as default gateway by router
#sudo ip addr add 10.0.0.1/24 dev eth0 label eth0:1
sudo nano /etc/network/interfaces
# auto eth0
# iface eth0 inet dhcp

# auto eth0:1
# iface eth0:1 inet static
# address 10.10.10.254
# netmask 255.255.255.0

# NAT
#sudo update-alternatives --set iptables /usr/sbin/iptables-legacy
#sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE