#!/usr/bin/env python

import matplotlib.pyplot as plt
import matplotlib.dates as md

import csv
import datetime
import array

#Configuration variables
yaxis_label = '\u2103'
xaxis_format = md.DateFormatter('%d.%m.%Y %H:%M')

#Load data from file
timestamps = array.array('i')
degrees = array.array('d')
with open('temp.csv') as f:
	f.readline()
	data = csv.reader(f, delimiter = ';')

	for row in data:
		timestamps.append(int(row[0]))
		degrees.append(float(row[1]))

#Render plot
plt.figure(figsize=(14, 8))

plt.plot([datetime.datetime.fromtimestamp(x) for x in timestamps], degrees)
#plt.plot([datetime.datetime.fromtimestamp(x) for x in timestamps], degrees, linestyle='none', marker='o', markersize=0.5)

plt.ylabel(yaxis_label)

plt.gca().xaxis.set_major_formatter(xaxis_format)
plt.xticks(fontsize=8, rotation=10)
plt.grid(linestyle='dashed')

#Save plot to file
plt.savefig("plot.svg", dpi='figure')
