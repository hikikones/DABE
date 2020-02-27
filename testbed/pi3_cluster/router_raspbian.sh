# OS: Raspbian Buster Lite (Linux)

##################
## General Setup
##################

# Keyboard layout
sudo dpkg-reconfigure keyboard-configuration

# Create root user
sudo passwd root
#reboot

# Remove pi user after reboot and login as root
deluser --remove-home pi

##################
## SSH Setup
##################

# Enable SSH
systemctl enable ssh

# Enable SSH root login
echo 'PermitRootLogin yes' >> /etc/ssh/sshd_config

##################
## Network Setup
##################

# Change hostname
nano /etc/hostname
nano /etc/hosts

# Static IP address
nano /etc/network/interfaces
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

# Enable IP forwarding
echo 'net.ipv4.ip_forward=1' >> /etc/sysctl.conf

##################
## TEACUP Setup
##################

# Tools needed
apt install -y ntp tcpdump

##################
## TCP Setup
##################