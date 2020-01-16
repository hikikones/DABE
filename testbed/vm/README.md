# Testbed in separate VMs using Vagrant

Vagrant is a tool for building and managing virtual machine environments in a single workflow, all within a simple configuration file called `Vagrantfile`.

## Setup

Install [Vagrant](https://www.vagrantup.com/downloads.html) and [VirtualBox 6.0.x](https://www.virtualbox.org/wiki/Download_Old_Builds_6_0).

## Usage

Open a terminal and `cd` into this folder where the `Vagrantfile` is. To boot up the VM environment, run:

```
vagrant up
```

The VMs specified from the `Vagrantfile` will now both be downloaded and start spinning up inside VirtualBox. If you experience a repeated `connection retrying` just wait, it will eventually resolve itself. When finished, you can access any VM through `ssh`:

```
vagrant ssh <vm-name>
```

To logout from the `ssh` session, type `logout` or press `CTRL+D`. To shut down everything, use the `vagrant destroy` command with the optional `-f` parameter to automatically confirm every shut down prompt.

## Examples

### Ping

A simple `ping` from `host1` to `host2` through the `router` can be run as follows:

```
vagrant ssh host1
ping 192.168.2.22
```

### Plotting CWND graph

_Note_: The plotting data is currently wrong. From [`siftr`](https://www.freebsd.org/cgi/man.cgi?query=siftr&apropos=0&sektion=4&manpath=FreeBSD+12.1-RELEASE&arch=default&format=html) we plot the field number 9 ("the current congestion window for the flow, in bytes") against time. Are currently more focused on the mininet in linux testbed instead.

First start `iperf` server on `host2` with `iperf -s`. Then access `host1` in another terminal, and run:

```
iperf -c 192.168.2.22 -t 60
```

After completion, upload the results:

```
cat /tmp/siftr.log | grep 192.168 | grep o, | nc termbin.com 9999
```

Paste the results in a `cwnd.log` file and run the Python script `plot.py` on host OS (not the VM).