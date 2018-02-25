#!/usr/bin/env python

header = ''
lines = []
tstamps = []
result = []

with open("TEMP.CSV") as f:
	lines = f.readlines()
	header = lines.pop(0)

for line in lines:
	tstamps.append(line.split(';')[1])

for x in range(0, len(tstamps) - 1):
	if not x == len(tstamps) - 1:
		if tstamps[x] != tstamps[x+1]:
			result.append(lines[x])
		else:
			print("Skipping: " + lines[x])


with open("TEMP.CSV", 'w') as f:
	f.write(header)
	for line in result:
		f.write(line)
