# Testbed in Mininet on Linux using Vagrant

Vagrant is a tool for building and managing virtual machine environments in a single workflow, all within a simple configuration file called `Vagrantfile`.

## Setup

Install [Vagrant](https://www.vagrantup.com/downloads.html) and [VirtualBox 6.0.x](https://www.virtualbox.org/wiki/Download_Old_Builds_6_0).

## Usage

Open a terminal and `cd` into this folder where the `Vagrantfile` is. To boot up the VM environment, run:

```
vagrant up
```

The VM specified from the `Vagrantfile` will now both be downloaded and start spinning up inside VirtualBox. If you experience a repeated `connection retrying` just wait, it will eventually resolve itself. When finished, you can access the VM through `ssh`:

```
vagrant ssh
```

To logout from the `ssh` session, type `logout` or press `CTRL+D`. To shut down everything, use the `vagrant destroy` command with the optional `-f` parameter to automatically confirm every shut down prompt.

## Examples

### Plotting CWND graph

_TODO_: Get the bandwidth/delay and other params/result right. Use Linux I/O redirection for debugging.

`ssh` into VM with simply `vagrant ssh`, then run:

```
sudo python /vagrant/linuxrouter.py
```

When finished, run the `plot.py` file in your host OS (not the VM) with:

```
python plot.py
```