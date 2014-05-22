export MYG4SIMSUBROOT="default"
export MYG4SIMWORKROOT=$MYG4SIMROOT/workDirectory/$MYG4SIMSUBROOT

#for configuration cards
export MATERIALLISTROOT=material_list
export MAGFIELDCARDROOT=MagField_none
export OUTCARDROOT=output_default
export GEOCARDROOT=geometry_default
export GENFILEROOT=gen_default

#for directories
export MACROSROOT=$MYG4SIMWORKROOT/macros/common
export CONFIGUREROOT=$MYG4SIMWORKROOT/configure/
export MYG4SIMDATAROOT="$MYG4SIMWORKROOT/data/"
export CONFIGUREDATAROOT=$MYG4SIMDATAROOT
export MYG4SIMFIELDMAPSROOT="$MYG4SIMWORKROOT/fieldMaps/"
export FIELDMAPSROOT=$MYG4SIMFIELDMAPSROOT
export OFILENAMEROOT=$MYG4SIMWORKROOT/output/raw_g4sim.root
export RUNNAMEROOT=TEST
export LOGFILEROOT=$MYG4SIMWORKROOT/runlog/logfile

source ../../env.sh
