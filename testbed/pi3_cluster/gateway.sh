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

host pihost2 { hardware ethernet b8:27:eb:07:7e:57; fixed-address 10.0.0.2; }
host pihost7 { hardware ethernet 02:03:04:05:06:07; fixed-address 10.0.0.7; }

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