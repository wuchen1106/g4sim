#!/bin/sh
if [ -z $MYG4SIMROOT ]; then
	MYG4SIMROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
fi

export G4SIMEXEC=$MYG4SIMROOT/g4sim

#Configuration directory
export CONFIGUREROOT=$MYG4SIMROOT/configure/
export CONFIGUREDATAROOT=$MYG4SIMDATAROOT
export FIELDMAPSROOT=$MYFIELDMAPSROOT

#for detector construction
export MATERIALLISTROOT=material_list
export GEOCARDROOT=geometry_A9
#for magField
export MAGFIELDCARDROOT=MagField_A9_130927
#for generator
export GENFILEROOT=gen_root
#for output
export OFILENAMEROOT=$MYG4SIMROOT/output/raw_g4sim.root
export RUNNAMEROOT=TEST
export OUTCARDROOT=output_default
#for log
export LOGFILEROOT=$MYG4SIMROOT/runlog/logfile

#for shared library
export LD_LIBRARY_PATH=$MYG4SIMROOT:$LD_LIBRARY_PATH
