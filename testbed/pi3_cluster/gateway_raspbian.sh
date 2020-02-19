# OS: Raspbian Buster (Linux)

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
# iface eth0 inet dhcp

# auto eth0:1
# iface eth0:1 inet static
# address 10.10.10.254
# netmask 255.255.255.0

# Enable IP forwarding
echo 'net.ipv4.ip_forward=1' >> /etc/sysctl.conf

# NAT
update-alternatives --set iptables /usr/sbin/iptables-legacy
iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE

##################
## TEACUP Setup
##################

# R
apt install -y r-base

# PDFJAM
apt install -y texlive-extra-utils

# SPP
apt install -y mercurial libpcap-dev build-essential
hg clone https://bitbucket.org/caia-swin/spp
cd spp
make
mkdir /usr/local/man/man1
make install
cd ..

# Fabric
apt install -y fabric python-pip python-dev libffi-dev libssl-dev
pip install fabric3
pip install -Iv pexpect==3.2

# TEACUP
wget https://sourceforge.net/projects/teacup/files/teacup-1.1.tar.gz
tar -xf teacup-1.1.tar.gz
# mkdir experiment
# cp teacup-1.1/example_configs/config-scenario1.py experiment/config.py
# cp teacup-1.1/run.sh experiment/
# cp teacup-1.1/fabfile.py experiment/

# fab run_experiment_single
# fab run_experiment_multiple

##################
## TCP Setup
##################

