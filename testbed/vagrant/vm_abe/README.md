# Testing ABE in separate VMs using Vagrant

Vagrant is a tool for building and managing virtual machine environments in a single workflow, all within a simple configuration file called `Vagrantfile`.

## Setup

Install [Vagrant](https://www.vagrantup.com/downloads.html) and [VirtualBox 6.0.x](https://www.virtualbox.org/wiki/Download_Old_Builds_6_0).

## Usage

Open a terminal and `cd` into this folder where the `Vagrantfile` is. To boot up the VM environment, run:

```
vagrant up
```

The VMs specified from the `Vagrantfile` will now both be downloaded and start spinning up inside VirtualBox. When finished, you can access any VM through `ssh`:

```
vagrant ssh <vm-name>
```

To logout from the `ssh` session, type `logout` or press `CTRL+D`. To shut down everything, use the `vagrant destroy` command with the optional `-f` parameter to automatically confirm every shut down prompt.

## Run experiment

First start `iperf` server on `host2` with `iperf -s`. Then access `host1` in another terminal, and run:

```
python3 /vagrant/run.py
```

After completion, plot the results by running `python plot.py` on the host OS (not VM).