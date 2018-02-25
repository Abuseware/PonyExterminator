#!/usr/bin/env python

# This script will remove all samples with time difference lesser than 60 seconds

header = ''
lines = []
tstamps = []
result = []

with open("TEMP.CSV") as f:
	lines = f.readlines()
	header = lines.pop(0)

for line in lines:
	tstamps.append(int(line.split(';')[0]))

for x in range(0, len(tstamps) - 1):
	if not x == len(tstamps) - 1:
		skew = tstamps[x+1] - tstamps[x]
		if skew > 59:
			result.append(lines[x])
		else:
			print("Skipping: " + lines[x][:-2] + " - sample difference " + str(skew))


with open("TEMP.CSV", 'w') as f:
	f.write(header)
	for line in result:
		f.write(line)
