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

# Static IP
#sudo nano /etc/dhcpcd.conf # ip: 10.0.0.2/24   gw: 10.0.0.1

# Subinterfaces used as default gateway for VLANs
#sudo ip addr add 192.168.10.1/24 dev eth0 label eth0:10
#sudo ip addr add 192.168.20.1/24 dev eth0 label eth0:20

sudo nano /etc/network/interfaces
# auto eth0
# iface eth0 inet static
# address 10.0.0.1
# netmask 255.255.255.0
# gateway 10.0.0.254

# auto eth0:10
# iface eth0:10 inet static
# address 192.168.10.1
# netmask 255.255.255.0
# gateway 10.0.0.254

# auto eth0:20
# iface eth0:20 inet static
# address 192.168.20.1
# netmask 255.255.255.0
# gateway 10.0.0.254