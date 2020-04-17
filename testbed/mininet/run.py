#!/usr/bin/python
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import Node
from mininet.log import setLogLevel, info
from mininet.cli import CLI
from time import sleep
import os


#########################
## Config Class
#########################

class Config:
    def __init__(self, aqm, aqm_params, cc_algo, limit, bandwidth, delay, duration):
        self.aqm = aqm
        self.aqm_params = "limit %s %s" % (limit, aqm_params)
        self.cc_algo = cc_algo
        self.host_cmds = [
            'sysctl -w net.ipv4.tcp_congestion_control=%s' % cc_algo,
            'sysctl -w net.ipv4.tcp_ecn=1',
        ]
        self.limit = limit
        self.bandwidth = bandwidth
        self.delay = delay
        self.duration = duration
        self.title = aqm + ' ' + cc_algo
        self.subtitle = self.aqm_params


#########################
## Experiment Configs
#########################

configs = {
    'fifo': Config(
        aqm='pfifo',
        aqm_params='',
        cc_algo='reno',
        limit=64,
        bandwidth='10mbit',
        delay='10ms',
        duration=10
        ),
    'codel': Config(
        aqm='codel',
        aqm_params='ecn',
        cc_algo='reno',
        limit=32,
        bandwidth='5mbit',
        delay='10ms',
        duration=10
        ),
    'pie': Config(
        aqm='pie',
        aqm_params='target 15ms tupdate 15ms ecn',
        cc_algo='reno',
        limit=64,
        bandwidth='100mbit',
        delay='50ms',
        duration=10
        ),
}

#########################
## Chosen Config
#########################

config = configs['fifo']




def run():
    #########################
    ## Network Setup
    #########################

    topo = NetworkTopo()
    net = Mininet( topo=topo )
    net.start()

    sender = net.get('h1')
    receiver = net.get('h2')
    router = net.get('r0')

    print('\nSetting up router:\n')

    router_interfaces = ['h1-eth', 'h2-eth']
    tcp_segment_offload = ("ethtool -K {iface} tso off gso off gro off")
    ingress = (
        "ip link add name {pface} type ifb;"
        "tc qdisc add dev {iface} handle ffff: ingress;"
        "tc qdisc add dev {pface} root {aqm} {params};"
        "ip link set dev {pface} up;"
        "tc filter add dev {iface} parent ffff: protocol all prio 10 u32 match u32 0 0"
            " flowid 1:1 action mirred egress redirect dev {pface};"
    )
    egress = (
        "tc qdisc add dev {iface} root handle 1: htb default 10;"
        "tc class add dev {iface} parent 1: classid 1:10 htb rate {rate} ceil {rate};"
        "tc qdisc add dev {iface} parent 1:10 handle 20: netem delay {delay} limit {queue};"
    )

    for intf in router_interfaces:
        router.cmdPrint(tcp_segment_offload.format(iface=intf))
        router.cmdPrint(ingress.format(iface=intf, pface=intf+'-ifb', aqm=config.aqm, params=config.aqm_params))
        router.cmdPrint(egress.format(iface=intf, rate=config.bandwidth, delay=config.delay, queue=config.limit))



    print('\n\nSetting up hosts:\n')

    hosts = [sender, receiver]

    for host in hosts:
        host.cmdPrint(tcp_segment_offload.format(iface=host.intf()))
        for cmd in config.host_cmds:
            host.cmdPrint(cmd)

    #########################
    ## Start Experiment
    #########################

    print('\n\nStarting experiment... %s second(s).\n' % config.duration)
    temp = 'test'
    result = 'result'
    receiver.cmdPrint('iperf -s &')
    sender.cmdPrint('iperf -t %s -c %s &' % (config.duration * 2, receiver.IP()))
    sender.cmd('ss -i dst %s > %s' % (receiver.IP(), temp))

    time = 0
    step = 0.01

    while time <= config.duration:
        sender.cmd('ss -i dst %s | grep cwnd >> %s' % (receiver.IP(), temp))
        sleep(step)
        time += step

    sender.cmdPrint('cat test | grep rtt > %s' % result)


    #########################
    ## End Experiment
    #########################

    print('\n\nExperiment done.\n')

    for host in hosts:
        host.cmdPrint('pkill iperf')
        host.cmdPrint('pkill iperf')
    
    net.stop()
    plot(result)
    os.system('rm %s %s' % (temp, result))


def plot(result):
    os.system('python3 plot.py %s "%s" "%s"' % (result, config.title, config.subtitle))


class LinuxRouter( Node ):
    "A Node with IP forwarding enabled."

    def config( self, **params ):
        super( LinuxRouter, self).config( **params )
        self.cmd( 'sysctl net.ipv4.ip_forward=1' )

    def terminate( self ):
        self.cmd( 'sysctl net.ipv4.ip_forward=0' )
        super( LinuxRouter, self ).terminate()


class NetworkTopo( Topo ):
    "A LinuxRouter connecting two IP subnets"

    def build( self, **_opts ):
        defaultIP = '192.168.1.1/24'
        router = self.addNode( 'r0', cls=LinuxRouter, ip=defaultIP )

        h1 = self.addHost('h1', ip='192.168.1.100/24', defaultRoute='via 192.168.1.1')
        h2 = self.addHost('h2', ip='172.16.0.100/12', defaultRoute='via 172.16.0.1')

        self.addLink(h1, router, intfName2='h1-eth', params2={ 'ip' : defaultIP })
        self.addLink(h2, router, intfName2='h2-eth', params2={ 'ip' : '172.16.0.1/12' })

    
if __name__ == '__main__':
    setLogLevel( 'info' )
    run()
