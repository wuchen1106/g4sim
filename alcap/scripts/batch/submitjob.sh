#!/bin/bash
# submitjob.sh -- submits one job to the Merlin cluster

if [ -z $ALCAPWORKROOT ] ; then
    echo "ALCAPWORKROOT not set. You must run env.sh first!"
    return
fi

if [ $# -ne 3 ] ; then
    echo "Incorrect Usage! You have not passed the correct number of arguments"
    echo "Correct Usage: ./submitjob.sh N_RUNS NAME N_PARTICLES"
fi

N_RUNS=$1
NAME=$2
N_PARTICLES=$3

echo "N_RUNS="$N_RUNS
echo "NAME="$NAME
echo "N_PARTICLES="$N_PARTICLES

cd $ALCAPWORKROOT

# Create an output directory for this set of runs
mkdir -p output/$NAME
