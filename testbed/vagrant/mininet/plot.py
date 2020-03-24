import matplotlib.pyplot as plt
import numpy as np

timeList = []
cwndList = []

first_timestamp = None

with open("cwnd.log") as f:
    first_line = f.readline().rstrip().split(" ")
    first_timestamp = float(first_line[0])

    for index, line in enumerate(f):
        fields = line.rstrip().split(" ")

        if fields[1] == "172.16.0.100:5001":
            continue

        time = float(fields[0]) - first_timestamp
        timeList.append(time)
        cwnd = int(fields[6])
        cwndList.append(cwnd)

plt.plot(timeList, cwndList)
plt.ylabel('CWND (MSS)')
plt.xlabel('Time (s)')
plt.show()