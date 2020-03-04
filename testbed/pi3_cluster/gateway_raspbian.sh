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
apt install iptables-persistent #to save rules

##################
## Time
##################

timedatectl set-timezone Europe/Oslo

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

#####################
## Teacup changes
######################
nano /home/teacup/teacup-1.1/hostsettup.py
#LINUX
#ctrl + w search for rmem, change 87380 to 873800 in this line
#run('sysctl net.ipv4.tcp_rmem=\'4096 87380 6291456\'')
#and 65535 to 655350 in this line
#run('sysctl net.ipv4.tcp_wmem=\'4096 655350 4194304\'')
#for some reason these default recive and send buffer values prevent cwind from
#going over that value, so multipling by 10 solves this.
#FREEBSD
#QUICK HACK FOR RAISING RECV/SEND BUFFER, multiplying by 10 from default.
#run('sysctl net.inet.tcp.recvspace=655360') 
#run('sysctl net.inet.tcp.sendspace=327680') 