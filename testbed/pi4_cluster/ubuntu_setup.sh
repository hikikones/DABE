##########
# SSH
##########

sudo apt install -y openssh-server
sudo ufw allow ssh

##########
# TEACUP
##########

# R
sudo apt install -y r-base

# PDFJAM
sudo apt install -y texlive-extra-utils

# SPP
sudo apt install -y mercurial libpcap-dev build-essential
hg clone https://bitbucket.org/caia-swin/spp
cd spp
sudo make
sudo mkdir /usr/local/man/man1
sudo make install
cd ..

# Fabric
sudo apt install -y fabric python-pip python-dev libffi-dev libssl-dev
pip install fabric
pip install -Iv pexpect==3.2

# TEACUP
wget https://sourceforge.net/projects/teacup/files/teacup-1.1.tar.gz
tar -xf teacup-1.1.tar.gz
# mkdir experiment
# cp teacup-1.1/example_configs/config-scenario1.py experiment/config.py
# cp teacup-1.1/run.sh experiment/
# cp teacup-1.1/fabfile.py experiment/