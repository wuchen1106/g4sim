#!/bin/sh

#for general settings
if [ -z $MYG4SIMROOT ]; then
	export MYG4SIMROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
fi
if [ -z $G4SIMEXEC ]; then
	export G4SIMEXEC=$MYG4SIMROOT/g4sim
fi
if ! echo $LD_LIBRARY_PATH | grep -q "$MYG4SIMROOT"; then
	export LD_LIBRARY_PATH=$MYG4SIMROOT:$LD_LIBRARY_PATH
fi
if ! echo $PATH | grep -q "$MYG4SIMROOT"; then
	export PATH=$MYG4SIMROOT:$PATH
fi

#for directories
if [ -z $MYG4SIMSUBROOT ]; then
	export MYG4SIMSUBROOT="default"
fi
if [ -z $MYG4SIMWORKROOT ]; then
	export MYG4SIMWORKROOT=$MYG4SIMROOT/workDirectory/$MYG4SIMSUBROOT
fi
if [ -z $MACROSROOT ]; then
	export MACROSROOT=$MYG4SIMWORKROOT/macros/common
fi
if [ -z $CONFIGUREROOT ]; then
	export CONFIGUREROOT=$MYG4SIMWORKROOT/configure/
fi
if [ -z $MYG4SIMDATAROOT ]; then
	export MYG4SIMDATAROOT="$MYG4SIMWORKROOT/data/"
fi
if [ -z $CONFIGUREDATAROOT ]; then
	export CONFIGUREDATAROOT=$MYG4SIMDATAROOT
fi
if [ -z $MYG4SIMFIELDMAPSROOT ]; then
	export MYG4SIMFIELDMAPSROOT="$MYG4SIMWORKROOT/fieldMaps/"
fi
if [ -z $FIELDMAPSROOT ]; then
	export FIELDMAPSROOT=$MYG4SIMFIELDMAPSROOT
fi
if [ -z $OFILENAMEROOT ]; then
	export OFILENAMEROOT=$MYG4SIMWORKROOT/output/raw_g4sim.root
fi
if [ -z $RUNNAMEROOT ]; then
	export RUNNAMEROOT=TEST
fi
if [ -z $LOGFILEROOT ]; then
	export LOGFILEROOT=$MYG4SIMWORKROOT/runlog/logfile
fi

#for configuration cards
if [ -z $MATERIALLISTROOT ]; then
	export MATERIALLISTROOT=material_list
fi
if [ -z $MAGFIELDCARDROOT ]; then
	export MAGFIELDCARDROOT=MagField_none
fi
if [ -z $OUTCARDROOT ]; then
	export OUTCARDROOT=output_default
fi
if [ -z $GEOCARDROOT ]; then
	export GEOCARDROOT=geometry_default
fi
if [ -z $GENFILEROOT ]; then
	export GENFILEROOT=gen_default
fi
