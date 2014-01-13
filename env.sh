#!/bin/sh
if [ -z $MYG4SIMROOT ]; then
	MYG4SIMROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
fi

export G4SIMEXEC=$MYG4SIMROOT/g4sim

#for shared library
export LD_LIBRARY_PATH=$MYG4SIMROOT:$LD_LIBRARY_PATH

#Configuration directory
export CONFIGUREROOT=$MYG4SIMROOT/configure/
export CONFIGUREDATAROOT=$MYG4SIMDATAROOT
export FIELDMAPSROOT=$MYFIELDMAPSROOT
export OFILENAMEROOT=$MYG4SIMROOT/output/raw_g4sim.root
export RUNNAMEROOT=TEST
export LOGFILEROOT=$MYG4SIMROOT/runlog/logfile

#for material
export MATERIALLISTROOT=material_list

#for default setting
export MAGFIELDCARDROOT=MagField_none
export OUTCARDROOT=output_default
export GEOCARDROOT=geometry_default
export GENFILEROOT=gen_default
