#!/usr/bin/env python

# This script will convert all data from old log format to new, UNIX timestamp based

import datetime

header = ''
lines = []
result = []

with open("TEMP.CSV") as f:
	lines = f.readlines()
	header = lines.pop(0)

for line in lines:
	data = line.split(';')
	timestamp = int(datetime.datetime.strptime(data[0] + " " + data[1], '%d.%m.%Y %H:%M').timestamp())
	result.append("{};{}".format(timestamp,data[2]))


with open("TEMP.CSV", 'w') as f:
	f.write("Timestamp;Degrees")
	for line in result:
		f.write(line)
