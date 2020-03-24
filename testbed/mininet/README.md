# Network Experimentation in Mininet

Run various network experiments in Mininet with the goal of testing different AQMs and CC algorithms.

## Setup

Install Ubuntu 19.10 on a real machine and install Mininet and other necessary tools as follows:

```
sudo apt install mininet git python3-pip
git clone https://github.com/mininet/mininet.git
cd mininet
util/install.sh -fnv
sudo pip3 install matplotlib
```

## Usage

Configure the desired experiment in the `run.py` file. Set the `config` variable to any experiment that has been defined in the `configs` dictionary. Then simply run:

```
sudo python run.py
```
