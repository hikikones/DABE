#!/usr/bin/python3
import os
from time import sleep


host2 = "192.168.2.22"
cc_algo = 'reno'

duration = 10
time = 0
step = 0.01

temp = 'temp'
result = '/vagrant/result.log'

os.system('sudo sysctl -w net.ipv4.tcp_congestion_control=%s' % cc_algo)

os.system('iperf -t %s -c %s &' % (duration + 1, host2))
while time <= duration:
    os.system('ss -i dst %s | grep cwnd >> %s' % (host2, temp))
    sleep(step)
    time += step

os.system('cat %s | grep rtt > %s' % (temp, result))