#!/usr/bin/python3
import matplotlib.pyplot as plt
import numpy as np

title = 'title'
subtitle = 'subtitle'

step = 0.01

timeList = []
cwndList = []
rttList = []
rateList = []

with open('result.log') as file:
    time = 0

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
                rateField = fields[index + 1]
                rate = None
                if "Mbps" in rateField:
                    rate = float(rateField.split("M")[0])
                elif "Kbps" in rateField:
                    rate = float(rateField.split("K")[0])
                    rate /= 1000
                rateList.append(rate)


# CWND plot
plt.subplot(3, 1, 1)
plt.plot(timeList, cwndList)
plt.suptitle(title)
plt.title(subtitle, fontsize=10)
plt.ylabel("CWND (k)")

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