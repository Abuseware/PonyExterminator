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
#length_divider = 240
interpolation_multiplier = 10


#Load data from file
data = numpy.loadtxt('temp.csv', delimiter = ';', dtype = {'names': ['timestamp', 'degrees'], 'formats': ['int32', 'f']} , skiprows = 1)

#Parse data into arrays
timestamps = numpy.array([float(x[0]) for x in data])
degrees = numpy.array([float(x[1]) for x in data])

#Interpolate
samples = len(timestamps) * interpolation_multiplier
print("Interpolating {} samples to {} samples".format(len(timestamps), samples))
xnew = numpy.linspace(timestamps.min(), timestamps.max(), samples)
ynew = interp.spline(timestamps, degrees, xnew)

#Render plot
#print("Output width: {}in".format(len(data)/length_divider))
#plt.figure(figsize=(len(data)/length_divider, 8))
plt.figure(figsize=(14, 8))

plt.plot([datetime.datetime.fromtimestamp(x) for x in xnew], ynew)
#plt.plot([datetime.datetime.fromtimestamp(x) for x in timestamps], degrees, linestyle='none', marker='o', markersize=0.5)

plt.ylabel(yaxis_label)

plt.gca().xaxis.set_major_formatter(xaxis_format)
plt.xticks(fontsize=8, rotation=10)
plt.grid(linestyle='dashed')

#Save plot to file
plt.savefig("plot.svg", dpi='figure')
