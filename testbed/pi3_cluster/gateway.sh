# Keyboard layout
echo 'keymap="no.kbd"' >> /etc/rc.conf

##################
## SSH Setup
##################

# SSH on FreeBSD is installed and enabled by default as seen by 'sshd_enable="YES"' from /etc/rc.conf.
# But you need to explicitly allow root login with:
echo 'PermitRootLogin yes' >> /etc/ssh/sshd_config

##################
## DHCP Server
##################

pkg install isc-dhcp44-server

# Setup dhcp configuration
cat > /usr/local/etc/dhcpd.conf

# Add the following to dhcpd.conf (exit 'cat' with CTRL+D on a new line):
option domain-name "ux.uis.no";
option domain-name-servers 152.94.1.9, 152.94.1.10, 152.94.1.11, 152.94.1.39;
option subnet-mask 255.255.255.0;

default-lease-time 3600;
max-lease-time 86400;
ddns-update-style none;
authoritative;

subnet 10.0.0.0 netmask 255.255.255.0 {
  range 10.0.0.1 10.0.0.7;
  option routers 10.0.0.254;
}

host pirouter { hardware ethernet b8:27:eb:ca:7b:af; fixed-address 10.0.0.1; }
host pihost2 { hardware ethernet b8:27:eb:07:7e:57; fixed-address 10.0.0.2; }
host pihost3 { hardware ethernet b8:27:eb:08:38:a1; fixed-address 10.0.0.3; }
host pihost4 { hardware ethernet b8:27:eb:98:9e:e0; fixed-address 10.0.0.4; }
host pihost5 { hardware ethernet b8:27:eb:9e:04:a3; fixed-address 10.0.0.5; }
host pihost6 { hardware ethernet b8:27:eb:4a:37:d9; fixed-address 10.0.0.6; }
host pihost7 { hardware ethernet b8:27:eb:86:d2:e0; fixed-address 10.0.0.7; }

# Enable dhcp server at boot
echo 'dhcpd_enable="YES"' >> /etc/rc.conf
echo 'dhcpd_ifaces="ue0"' >> /etc/rc.conf

# Start dhcp server
service isc-dhcpd start

##################
## Network Setup
##################

# Add an interface alias (subinterface)
echo 'ifconfig_ue0_alias0="inet 10.0.0.254 netmask 255.255.255.0"' >> /etc/rc.conf

# Change hostname
nano /etc/rc.conf

# NAT
echo 'pf_enable="YES"' >> /etc/rc.conf
echo 'pf_rules="/etc/pf.conf"' >> /etc/rc.conf
echo 'nat on ue0 inet from 10.0.0.0/24 to any -> ue0' >> /etc/pf.conf

#cat > /etc/pf.conf

# ext_if=ue0
# inet="10.0.0.0/24"

# nat on $ext_if from $inet to any -> ($ext_if)
# rdr on $ext_if inet proto tcp to port 22 -> 10.0.0.254 port 22

# pass in all
##nat on ue0 inet from 10.0.0.0/24 to any -> ue0

###nat on ue0 from any to any -> (ue0)

##########
# TEACUP
##########

# R
pkg install -y R

# PDFJAM
pkg install -y texlive-base

# SPP
pkg install -y mercurial
hg clone https://bitbucket.org/caia-swin/spp
cd spp
make
make install
cd ..

# Fabric
pkg install -y portmaster py27-pip
portsnap fetch && sudo portsnap extract
portmaster -D -G --no-confirm lang/python2
#sudo portmaster -D -G --no-confirm devel/py-pip
python2 -m pip install fabric3 # TODO: ERROR building wheels for pynacl PEP 517
python2 -m pip install -Iv pexpect==3.2

# TEACUP
pkg install -y wget nano
wget https://sourceforge.net/projects/teacup/files/teacup-1.1.tar.gz
tar -xf teacup-1.1.tar.gz
mkdir experiment
cp teacup-1.1/example_configs/config-scenario1.py experiment/config.py
cp teacup-1.1/run.sh experiment/
cp teacup-1.1/fabfile.py experiment/
cd experiment
nano config.py # TPCONF_script_path = '~/teacup-1.1'
fab check_config

##################
## TCP Setup
##################

# Enable ECN
sudo sysctl -w net.inet.tcp.ecn.enable=1

# Load kernel module SIFTR for TCP logging
sudo kldload siftr
sudo sysctl net.inet.siftr.enabled=1
sudo sysctl net.inet.siftr.logfile=/tmp/siftr.log

# Install iperf package
sudo pkg install -y iperf