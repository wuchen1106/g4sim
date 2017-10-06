#!/bin/python
# Adds all the variables for the various local positions to the file

headerfile_in=open("../include/MonitorSD.hh", "r")
sourcefile_in=open("../src/MonitorSD.cc", "r")
headerfile_out=open("../include/MonitorSD.hh_out", "w")
sourcefile_out=open("../src/MonitorSD.cc_out", "w")

searchstrings=["_Ox", "_Oy", "_Oz", "_ox", "_oy", "_oz", "_x", "_y", "_z"]

# Read in each line
for line in headerfile_in:

    headerfile_out.write(line) # want to write out all the lines

    # Search for the lines we want to copy and replace
    for string in searchstrings:
        if line.find(string) != -1:
#            print(line)
            headerfile_out.write(line.replace(string, "_local"+string))


for line in sourcefile_in:

    sourcefile_out.write(line) # want to write out all the lines

    # Search for the lines we want to copy and replace
    for string in searchstrings:
        if line.find(string) != -1:
#            print(line)
            sourcefile_out.write(line.replace(string, "_local"+string))

