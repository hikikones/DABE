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

        timeList.append(time)
        time += step

        for field in fields:
            if "cwnd" in field:
                cwnd = int(field.split(":")[1])
                cwndList.append(cwnd)


plt.plot(timeList, cwndList)
plt.suptitle(title)
plt.title(subtitle, fontsize=10)
plt.ylabel(ylabel)
plt.xlabel(xlabel)
plt.show()