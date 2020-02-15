##################
## SSH Setup
##################

# SSH on FreeBSD is installed and enabled by default as seen by 'sshd_enable="YES"' from /etc/rc.conf.
# But you need to explicitly allow root login with:
echo 'PermitRootLogin yes' >> /etc/ssh/sshd_config

##################
## Network Setup
##################

# Add an interface alias (subinterface)
echo 'ifconfig_ue0_alias0="inet 192.168.xx.x netmask 255.255.255.0"' >> /etc/rc.conf

# Change hostname
nano /etc/rc.conf