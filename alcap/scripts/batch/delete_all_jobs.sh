#!/bin/bash
# Deletes all the jobs

qstat | grep edmonds | cut -d ' ' -f 1 > jobs
for job in `cat jobs` ; do qdel $job; done