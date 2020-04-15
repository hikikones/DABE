#!/usr/bin/python3
import matplotlib.pyplot as plt
import numpy as np
import sys


args = sys.argv[1:]
title = args[0]
subtitle = args[1]

timeList = []
cwndList = []
rttList = []
rateList = []

with open("result") as file:
    time = 0
    step = 0.01

    for line in file:
        fields = line.rstrip().split(" ")

        timeList.append(time)
        time += step

        for index, field in enumerate(fields):
            if "cwnd" in field:
                cwnd = int(field.split(":")[1])
                cwndList.append(cwnd)
            
            if "rtt" in field and "minrtt" not in field:
                rtt = float(field.split(":")[1].split("/")[0])
                rttList.append(rtt)
            
            if "send" in field:
                rate = float(fields[index + 1].split("M")[0])
                rateList.append(rate)


# CWND plot
plt.subplot(3, 1, 1)
plt.plot(timeList, cwndList)
plt.suptitle(title)
plt.title(subtitle, fontsize=10)
plt.ylabel("CWND (packets)")

# RTT plot
plt.subplot(3, 1, 2)
plt.plot(timeList, rttList)
plt.ylabel("RTT (ms)")

# Throughput plot
plt.subplot(3, 1, 3)
plt.plot(timeList, rateList)
plt.ylabel("Throughput (Mbps)")
plt.xlabel("Time (s)")

plt.show()