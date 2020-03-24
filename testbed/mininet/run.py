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
    def __init__(self, aqm, aqm_params, cc_algo, bandwidth, duration, title, subtitle, ylabel, xlabel):
        self.aqm = aqm
        self.aqm_params = aqm_params
        self.cc_algo = cc_algo
        self.host_cmds = [
            'sysctl -w net.ipv4.tcp_congestion_control=%s' % cc_algo,
            'sysctl -w net.ipv4.tcp_ecn=1',
        ]
        self.bandwidth = bandwidth
        self.duration = duration
        self.title = title
        self.subtitle = subtitle
        self.ylabel = ylabel
        self.xlabel = xlabel


#########################
## Experiment Configs
#########################

configs = {
    'fifo': Config(
        aqm='pfifo',
        aqm_params='limit 32',
        cc_algo='reno',
        bandwidth='5mbit',
        duration=30,
        title='FIFO NewReno',
        subtitle='pfifo 5mbit limit 32',
        ylabel='CWND (MSS)',
        xlabel='Time (s)'
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
    for i in range(1, 3):
        router.cmdPrint('tc qdisc add dev h%s-eth root handle 1:0 htb default 1' % i)
        router.cmdPrint('tc class add dev h%s-eth classid 1:1 htb rate %s' % (i, config.bandwidth))
        router.cmdPrint('tc qdisc add dev h%s-eth parent 1:1 handle 10:1 %s %s' % (i, config.aqm, config.aqm_params))

        # router.cmdPrint('tc qdisc add dev h%s-eth root %s' % (i, qdisc))

    print('\n\nSetting up hosts:\n')
    for cmd in config.host_cmds:
        sender.cmdPrint(cmd)
        receiver.cmdPrint(cmd)

    #########################
    ## Start Experiment
    #########################

    print('\n\nStarting experiment... %s second(s).\n' % config.duration)
    receiver.cmdPrint('iperf -s &')
    sender.cmdPrint('iperf -t %s -b %s -c %s &' % (config.duration * 2, config.bandwidth, receiver.IP()))
    sender.cmd('ss -i dst %s > test' % receiver.IP())

    time = 0
    step = 0.01

    while time <= config.duration:
        sender.cmd('ss -i dst %s | grep cwnd >> test' % receiver.IP())
        sleep(step)
        time += step

    sender.cmdPrint('cat test | grep cwnd > result')


    #########################
    ## End Experiment
    #########################

    print('\n\nExperiment done.\n')
    sender.cmdPrint('pkill iperf')
    receiver.cmdPrint('pkill iperf')
    sender.cmdPrint('pkill iperf')
    receiver.cmdPrint('pkill iperf')
    net.stop()
    plot()


def plot():
    os.system('python3 plot.py "%s" "%s" "%s" "%s"' % (config.title, config.subtitle, config.ylabel, config.xlabel))


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
