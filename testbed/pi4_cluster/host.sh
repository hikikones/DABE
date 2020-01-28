# Keyboard layout
sudo dpkg-reconfigure keyboard-configuration

# SSH
sudo systemctl enable ssh

# Hostname
sudo nano /etc/hostname
sudo nano /etc/hosts

# Static IP
#sudo nano /etc/dhcpcd.conf # ip: 192.168.xx.x   gw: 192.168.xx.1
sudo nano /etc/network/interfaces
# auto eth0
# iface eth0 inet static
# address 10.0.0.x
# netmask 255.255.255.0
# gateway 10.0.0.254

# auto eth0:1
# iface eth0:1 inet static
# address 192.168.xx.x
# netmask 255.255.255.0
# gateway 192.168.xx.1

host route to other subnet
for host 2,3,4
	sudo ip route add 192.168.20.0/24 via 192.168.10.1
for host 5,6,7
	sudo ip route add 192.168.10.0/24 via 192.168.20.1

sudo nano /etc/resolve.conf
#domain ux.uis.no
#nameserver 152.94.1.9
#nameserver 152.94.1.10
#nameserver 152.94.1.39
#nameserver 152.94.1.11