##################
## General Setup
##################

# Keyboard layout
echo 'keymap="no.kbd"' >> /etc/rc.conf

# Remove freebsd user
rmuser freebsd

##################
## SSH Setup
##################

# SSH on FreeBSD is installed and enabled by default as seen by 'sshd_enable="YES"' from /etc/rc.conf.
# But you need to explicitly allow root login with:
echo 'PermitRootLogin yes' >> /etc/ssh/sshd_config

# Disable DNS lookup
echo 'UseDNS no' >> /etc/ssh/sshd_config

##################
## Network Setup
##################

# Change hostname
nano /etc/rc.conf

# Static IP
echo 'ifconfig_ue0="inet 10.0.0.x netmask 255.255.255.0"' >> /etc/rc.conf
echo 'defaultrouter="10.0.0.254"' >> /etc/rc.conf

# Add an interface alias (subinterface)
echo 'ifconfig_ue0_alias0="inet 192.168.xx.x netmask 255.255.255.0"' >> /etc/rc.conf

# Add route
static_routes="intnet"
route_intnet="-net 192.168.xx.0/24 192.168.xx.1"