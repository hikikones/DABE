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
# address 192.168.xx.x
# netmask 255.255.255.0
# gateway 192.168.xx.1

# auto eth0:1
# iface eth0:1 inet static
# address 10.0.0.x
# netmask 255.255.255.0
# gateway 10.0.0.254