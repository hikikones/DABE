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

# FreeBSD kernel source
#wget ftp://ftp.freebsd.org/pub/FreeBSD/releases/arm64/12.1-RELEASE/src.txz
#tar -C / -xzvf src.txz

# Lighttpd
pkg install -y lighttpd
# portsnap fetch && portsnap extract
# pkg install portmaster
# portmaster -D -G --no-confirm editors/lighttp
# portmaster -D -G --no-confirm web/lighttp

# Iperf
wget https://sourceforge.net/projects/iperf2/files/iperf-2.0.9.tar.gz --no-check-certificate
#wget http://caia.swin.edu.au/urp/newtcp/tools/caia_iperf208_1.1.patch
tar -xf iperf-2.0.9.tar.gz
cd iperf-2.0.9
patch -p1 < iperf-2.0.9.patch
./configure
make
make install

# Httperf
pkg install -y httperf

# Nttcp
#portsnap fetch && portsnap extract
#pkg install portmaster
#portmaster -D -G --no-confirm benchmark/nttcp
wget http://caia.swin.edu.au/tools/teacup/downloads/nttcp-1.47-mod.tar.gz
tar -xf nttcp-1.47-mod.tar.gz
cd nttcp-1.47-mod
make
cp nttcp /bin/