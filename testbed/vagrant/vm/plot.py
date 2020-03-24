import matplotlib.pyplot as plt
import numpy as np

timeList = []
cwndList = []

first_timestamp = None

with open("cwnd.log") as f:
    first_line = f.readline().rstrip().split(",")
    first_timestamp = float(first_line[2])

    for line in f:
        fields = line.rstrip().split(",")

        time = float(fields[2]) - first_timestamp
        timeList.append(time)

        cwnd = int(fields[8])
        cwndList.append(cwnd)

plt.plot(timeList, cwndList)
plt.ylabel('CWND (bytes)')
plt.xlabel('Time (s)')
plt.show()