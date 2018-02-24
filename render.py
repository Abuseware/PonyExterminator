#!/usr/bin/env python

import matplotlib.pyplot as plt
import matplotlib.dates as md

import numpy
import scipy.interpolate as interp

import csv
import datetime

#Configuration variables
yaxis_label = '\u2103'
xaxis_format = md.DateFormatter('%d.%m.%Y %H:%M')
length_divider = 2*3



#Load data from file
data = numpy.loadtxt('temp.csv', delimiter = ';', dtype = {'names': ['date', 'time', 'degrees'], 'formats': ['U16', 'U16', 'f']} , skiprows = 1)

#Parse data into arrays
timestamps = numpy.array([datetime.datetime.strptime(x[0] + " " + x[1], '%d.%m.%Y %H:%M').timestamp() for x in data])
degrees = numpy.array([float(x[2]) for x in data])

#Interpolate
xnew = numpy.linspace(timestamps.min(), timestamps.max(), 1000)
ynew = interp.spline(timestamps, degrees, xnew)

#Render plot
plt.figure(figsize=(len(data)/length_divider, 7))

plt.plot([datetime.datetime.fromtimestamp(x) for x in xnew], ynew)
plt.plot([datetime.datetime.fromtimestamp(x) for x in timestamps], degrees, linestyle='none', marker='.', markersize=6)

plt.ylabel(yaxis_label)

plt.gca().xaxis.set_major_formatter(xaxis_format)
plt.xticks(fontsize=8, rotation=10)
plt.grid(linestyle='dashed')

#Save plot to file
plt.savefig("plot.svg", dpi='figure')
