#!/usr/bin/python

"""
linuxrouter.py: Example network with Linux IP router

This example converts a Node into a router using IP forwarding
already built into Linux.

The example topology creates a router and three IP subnets:

    - 192.168.1.0/24 (r0-eth1, IP: 192.168.1.1)
    - 172.16.0.0/12 (r0-eth2, IP: 172.16.0.1)
    - 10.0.0.0/8 (r0-eth3, IP: 10.0.0.1)

Each subnet consists of a single host connected to
a single switch:

    r0-eth1 - s1-eth1 - h1-eth0 (IP: 192.168.1.100)
    r0-eth2 - s2-eth1 - h2-eth0 (IP: 172.16.0.100)
    r0-eth3 - s3-eth1 - h3-eth0 (IP: 10.0.0.100)

The example relies on default routing entries that are
automatically created for each router interface, as well
as 'defaultRoute' parameters for the host interfaces.

Additional routes may be added to the router or hosts by
executing 'ip route' or 'route' commands on the router or hosts.
"""


from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import Node
from mininet.log import setLogLevel, info
from mininet.cli import CLI
from mininet.util import pmonitor
from time import sleep
from subprocess import Popen
import os


class LinuxRouter( Node ):
    "A Node with IP forwarding enabled."

    def config( self, **params ):
        super( LinuxRouter, self).config( **params )
        # Enable forwarding on the router
        self.cmd( 'sysctl net.ipv4.ip_forward=1' )

    def terminate( self ):
        self.cmd( 'sysctl net.ipv4.ip_forward=0' )
        super( LinuxRouter, self ).terminate()


class NetworkTopo( Topo ):
    "A LinuxRouter connecting two IP subnets"

    def build( self, **_opts ):
        defaultIP = '192.168.1.1/24'  # IP address for r0-eth1
        router = self.addNode( 'r0', cls=LinuxRouter, ip=defaultIP )

        h1 = self.addHost('h1', ip='192.168.1.100/24', defaultRoute='via 192.168.1.1')
        h2 = self.addHost('h2', ip='172.16.0.100/12', defaultRoute='via 172.16.0.1')

        self.addLink(h1, router, intfName2='r0-eth1', params2={ 'ip' : defaultIP })
        self.addLink(h2, router, intfName2='r0-eth2', params2={ 'ip' : '172.16.0.1/12' })


def test(net):
    sender = net.get('h1')
    receiver = net.get('h2')
    router = net.get('r0')

    # Setup router bottleneck
    router.cmd('tc qdisc add dev r0-eth1 root tbf rate 1mbit limit 0.1mb burst 32kB peakrate 1.01mbit mtu 1600')
    router.cmd('tc qdisc add dev r0-eth2 root tbf rate 1mbit limit 0.1mb burst 32kB peakrate 1.01mbit mtu 1600')

    # Start traffic generator and tcp logging
    duration = 120
    print '\n\nStarting iperf traffic generator... Duration: %s (s)' % duration
    receiver.cmd('iperf -s &')
    start_tcpprobe("/vagrant/cwnd.log")
    sender.cmd('iperf -t %s -c %s &' % (duration, receiver.IP()))
    sleep(duration)

    # Kill everything
    receiver.cmd('kill %iperf')
    sender.cmd('kill %iperf')
    stop_tcpprobe()

def start_tcpprobe(outfile="cwnd.txt"):
    os.system("modprobe tcp_probe")
    os.system("rmmod tcp_probe; modprobe tcp_probe full=1;")
    Popen("cat /proc/net/tcpprobe > %s" % outfile, shell=True)

def stop_tcpprobe():
    Popen("killall -9 cat", shell=True).wait()

def run():
    "Test linux router"
    topo = NetworkTopo()
    net = Mininet( topo=topo )
    net.start()
    info( '*** Routing Table on Router:\n' )
    info( net[ 'r0' ].cmd( 'route' ) )
    test(net)
    #CLI( net )
    net.stop()
    
if __name__ == '__main__':
    setLogLevel( 'info' )
    run()
