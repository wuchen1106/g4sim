#!/bin/bash

getMemSize(){
  output=`ps aux | grep "\<g4[s]im\>"`
  if [ ! -n "$output" ]; then
    echo "JOBNOTEXIST"
  else
    set -- $output
    pid=$2
    sizeS=`cat /proc/$pid/status|grep VmSize`
    set -- $sizeS
    size=$2
    if [ $3 = "kB" ]; then
    	size=`echo "$size/1000"|bc`
    elif [ $3 = "GB" -o $3 = "gB" ]; then
    	size=`echo "$size*1000"|bc`
    elif [ $3 = "MB" -o $3 = "mB" ]; then
    	size=`echo "$size/1"|bc`
    elif [ $3 = "Byte" -o $3 = "B" ]; then
    	size=`echo "$size/1000000"|bc`
    fi
    echo $size
  fi
}

getNevents(){
  output=`tail log -n 1`
  set -- $output
  nEvents=$5
  if [ ! -n $nEvents ]; then
    nEvents=0
  fi
  echo $nEvents
}

start=`date +%s`
echo "t/D s/D n/I" > logmem
continueornot=true
while $continueornot
do
  size=`getMemSize`
  if [ $size = "JOBNOTEXIST" ]; then
    echo "JOB DOES NOT EXIST ANYMORE!!!"
    txt2root -t logmem logmem.root
    continueornot=false
  else
    time=`date +%s`
    nEvents=`getNevents`
    ((time=time-start))
    echo "$time     $size    $nEvents" >> logmem
    sleep 1;
  fi
done

