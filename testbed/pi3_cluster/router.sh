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
echo 'ifconfig_ue0="inet 10.0.0.1 netmask 255.255.255.0"' >> /etc/rc.conf
echo 'defaultrouter="10.0.0.254"' >> /etc/rc.conf

# Add an interface alias (subinterface)
echo 'ifconfig_ue0_alias0="inet 192.168.10.1 netmask 255.255.255.0"' >> /etc/rc.conf
echo 'ifconfig_ue0_alias1="inet 192.168.20.1 netmask 255.255.255.0"' >> /etc/rc.conf

# Enable IP forwarding
echo 'gateway_enable="YES"' >> /etc/rc.conf


##################
## Time Sync
##################

echo 'ntpd_enable=YES' >> /etc/rc.conf
echo 'ntpd_sync_on_start=YES' >> /etc/rc.conf
echo 'server 0.no.pool.ntp.org' >> /etc/ntp.conf
echo 'server 1.no.pool.ntp.org' >> /etc/ntp.conf
echo 'server 2.no.pool.ntp.org' >> /etc/ntp.conf
echo 'server 3.no.pool.ntp.org' >> /etc/ntp.conf

##################
## TEACUP Setup
##################

# Configure firewall for dummynet
echo 'firewall_enable="YES"' >> /etc/rc.conf
echo 'firewall_type="OPEN"' >> /etc/rc.conf
