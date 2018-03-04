#!/usr/bin/env python

import sys
import argparse

import csv
import array

import datetime
import time

try:
    import matplotlib.pyplot as plt
    import matplotlib.dates as md
except ImportError:
    print("Matplotlib not found, please install required libraries")
    exit(1)

# Configuration variables
yaxis_label = '\u2103'
xaxis_format = md.DateFormatter('%d.%m.%Y %H:%M')

# Parse command line arguments
parser = argparse.ArgumentParser(description='Render thermometer data into plot.')
parser.add_argument('file', type=argparse.FileType(), help='source file')
parser.add_argument('--format', choices=['svg', 'png'], default='svg', help='destination image format')
parser.add_argument('--markers', action='store_true', help='draw markers on samples')
parser.add_argument('--width', type=int, default=35, help='output width in centimeters')
parser.add_argument('--height', type=int, default=20, help='output height in centimeters')
parser.add_argument('--days', type=int, default=0, help='render data from last DAYS')
parser.add_argument('--hours', type=int, default=0, help='render data from last HOURS')
parser.add_argument('--minutes', type=int, default=0, help='render data from last MINUTES')
parser.add_argument('--seconds', type=int, default=0, help='render data from last SECONDS')

args = parser.parse_args(sys.argv[1:])

# Calculate delta timestamp
time_delta = (args.days * 24 * 60 * 60) + (args.hours * 60 * 60) + (args.minutes * 60) + args.seconds
time_after = int(time.time()) - time_delta if time_delta else 0
time_offset = time.localtime().tm_gmtoff

# Load data from file
timestamps = array.array('i')
degrees = array.array('d')
try:
    args.file.readline()  # Skip header
    for row in csv.reader(args.file, delimiter=';'):
        utctime = int(row[0]) - time_offset
        if int(utctime) >= time_after:
            timestamps.append(utctime)
            degrees.append(float(row[1]))
except ValueError:
    print("File malformed - value has wrong data type, exiting.")
    exit(1)
except IndexError:
    print("File malformed - wrong number of columns, exiting.")
    exit(1)

if not len(timestamps):
    print("No data available in specified time range, or file is empty.")
    exit(1)

# Render plot
plt.figure(figsize=(args.width * 0.3937, args.height * 0.3937))

plt.plot([datetime.datetime.fromtimestamp(x) for x in timestamps], degrees)
if args.markers:
    plt.plot([datetime.datetime.fromtimestamp(x) for x in timestamps], degrees, linestyle='none', marker='o', markersize=3)

plt.ylabel(yaxis_label)

plt.gca().xaxis.set_major_formatter(xaxis_format)
plt.xticks(fontsize=8, rotation=10)
plt.grid(linestyle='dashed')

# Save plot to file
try:
    plt.savefig(args.file.name.rsplit('.')[0] + '.' + args.format, dpi='figure')
except PermissionError:
    print("Cannot write to file, exiting.")
    exit(1)
except Exception:
    print("Unexpected error: " + sys.exc_info()[0])
    raise
