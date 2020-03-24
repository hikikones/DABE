#!/usr/bin/python3
import matplotlib.pyplot as plt
import numpy as np
import sys


args = sys.argv[1:]

title = args[0]
subtitle = args[1]
ylabel = args[2]
xlabel = args[3]

timeList = []
cwndList = []

with open("result") as f:
    time = 0
    step = 0.01

    for index, line in enumerate(f):
        fields = line.rstrip().split(" ")

        cwnd = int(fields[9].split(":")[1])
        cwndList.append(cwnd)

        timeList.append(time)
        time += step

plt.plot(timeList, cwndList)
plt.suptitle(title)
plt.title(subtitle, fontsize=10)
plt.ylabel(ylabel)
plt.xlabel(xlabel)
plt.show()