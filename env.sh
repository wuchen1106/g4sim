#!/bin/sh
if [ -z $MYG4SIMROOT ]; then
	export MYG4SIMROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
fi
if [ -z $MYG4SIMSUBROOT ]; then
	export MYG4SIMSUBROOT="Default"
fi
if [ -z $MYG4SIMDATAROOT ]; then
	export MYG4SIMDATAROOT="$MYG4SIMROOT/WorkDirectory/$MYG4SIMSUBROOT/data/"
fi
if [ -z $MYG4SIMFIELDMAPSROOT ]; then
	export MYG4SIMFIELDMAPSROOT="$MYG4SIMROOT/WorkDirectory/$MYG4SIMSUBROOT/data/"
fi

export G4SIMEXEC=$MYG4SIMROOT/g4sim

#for shared library
export LD_LIBRARY_PATH=$MYG4SIMROOT:$LD_LIBRARY_PATH
export PATH=$MYG4SIMROOT:$PATH

#Configuration directory
export CONFIGUREROOT=$MYG4SIMROOT/WorkDirectory/$MYG4SIMSUBROOT/configure/
export CONFIGUREDATAROOT=$MYG4SIMDATAROOT
export FIELDMAPSROOT=$MYG4SIMFIELDMAPSROOT
export OFILENAMEROOT=$MYG4SIMROOT/output/raw_g4sim.root
export RUNNAMEROOT=TEST
export LOGFILEROOT=$MYG4SIMROOT/WorkDirectory/$MYG4SIMSUBROOT/runlog/logfile

#for default setting
export MATERIALLISTROOT=material_list
export MAGFIELDCARDROOT=MagField_none
export OUTCARDROOT=output_default
export GEOCARDROOT=geometry_default
export GENFILEROOT=gen_default
