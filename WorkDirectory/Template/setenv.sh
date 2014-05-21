if [ -z $MYG4SIMROOT ]; then
	export MYG4SIMROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd ../.. && pwd )"
fi
export MYG4SIMSUBROOT="Default"
export MYG4SIMWORKROOT=$MYG4SIMROOT/WorkDirectory/$MYG4SIMSUBROOT
if [ -z $MYG4SIMDATAROOT ]; then
	export MYG4SIMDATAROOT="$MYG4SIMWORKROOT/data/"
fi
if [ -z $MYG4SIMFIELDMAPSROOT ]; then
	export MYG4SIMFIELDMAPSROOT="$MYG4SIMWORKROOT/fieldMaps/"
fi
source ../../env.sh
