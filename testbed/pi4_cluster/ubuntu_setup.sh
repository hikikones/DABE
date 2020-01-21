##########
# SSH
##########

sudo apt install -y openssh-server
sudo ufw allow ssh

##########
# TEACUP
##########

# R
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys E298A3A825C0D65DFD57CBB651716619E084DAB9
sudo add-apt-repository 'deb https://cloud.r-project.org/bin/linux/ubuntu bionic-cran35/'
sudo apt update
sudo apt install -y r-base

# PDFJAM
sudo apt install -y texlive-extra-utils

# SPP
sudo apt install -y mercurial
hg clone https://bitbucket.org/caia-swin/spp
sudo apt install -y libpcap-dev
sudo apt install -y build-essential
cd spp
sudo make
sudo mkdir /usr/local/man/man1
sudo make install
cd ..

# Fabric
sudo apt install -y fabric
sudo apt install -y python-pip
pip install fabric
pip install -Iv pexpect==3.2

# TEACUP
wget https://sourceforge.net/projects/teacup/files/teacup-1.1.tar.gz
tar -xf teacup-1.1.tar.gz
# mkdir experiment
# cp teacup-1.1/example_configs/config-scenario1.py experiment/config.py
# cp teacup-1.1/run.sh experiment/
# cp teacup-1.1/fabfile.py experiment/