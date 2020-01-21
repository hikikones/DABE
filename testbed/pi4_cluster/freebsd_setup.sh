##########
# TEACUP
##########

# R
sudo pkg install -y R

# PDFJAM
sudo pkg install -y texlive-base

# SPP
sudo pkg install -y mercurial
hg clone https://bitbucket.org/caia-swin/spp
cd spp
sudo make
sudo make install
cd ..

# Fabric
sudo pkg install -y portmaster py27-pip
sudo portsnap fetch
sudo portsnap extract
sudo portmaster -D -G --no-confirm lang/python2
#sudo portmaster -D -G --no-confirm devel/py-pip
sudo python2 -m pip install fabric
sudo python2 -m pip install -Iv pexpect==3.2

# TEACUP
sudo pkg install -y wget nano
wget https://sourceforge.net/projects/teacup/files/teacup-1.1.tar.gz
tar -xf teacup-1.1.tar.gz
mkdir experiment
cp teacup-1.1/example_configs/config-scenario1.py experiment/config.py
cp teacup-1.1/run.sh experiment/
cp teacup-1.1/fabfile.py experiment/