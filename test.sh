#!/bin/sh

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
export OUTCARDROOT=output_test
export GEOCARDROOT=geometry_test
export GENFILEROOT=gen_test
