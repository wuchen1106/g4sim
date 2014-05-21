#!/bin/sh
if [ -z $MYG4SIMROOT ]; then
	export MYG4SIMROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd ../.. && pwd )"
fi
if [ -z $MYG4SIMSUBROOT ]; then
	export MYG4SIMSUBROOT="Default"
fi
if [ -z $MYG4SIMDATAROOT]; then
	export MYG4SIMDATAROOT="$MYG4SIMROOT/WorkDirectory/$MYG4SIMSUBROOT/data/"
fi
if [ -z $MYG4SIMFIELDMAPSROOT]; then
	export MYG4SIMFIELDMAPSROOT="$MYG4SIMROOT/WorkDirectory/$MYG4SIMSUBROOT/data/"
fi
source ../../env.sh
