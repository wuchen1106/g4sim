export MYG4SIMWORKROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export PHYSICSLIST=QGSP_BERT           # QGSP_BERT;QGSP_BERT_HP;QGSP_BERT_noRadi;QGSP_BERT_HP_noRadi

#for configuration cards
export MATERIALLISTROOT=material_list
export MAGFIELDCARDROOT=mag/MagField_none
export OUTCARDROOT=output/output_default
export GEOCARDROOT=geometry_1217
export GENFILEROOT=gen/gen_mum_up

#for directories
export MACROSROOT=$MYG4SIMWORKROOT/macros
export MACROSCOMMONROOT=$MACROSROOT/common
export CONFIGUREROOT=$MYG4SIMWORKROOT/configure/
export MYG4SIMDATAROOT="$MYG4SIMWORKROOT/data/"
export CONFIGUREDATAROOT=$MYG4SIMDATAROOT
export MYG4SIMFIELDMAPSROOT="$MYG4SIMWORKROOT/fieldMaps/"
export FIELDMAPSROOT=$MYG4SIMFIELDMAPSROOT
export OFILENAMEROOT=$MYG4SIMWORKROOT/output/raw_g4sim.root
export RUNNAMEROOT=TEST
export LOGFILEROOT=$MYG4SIMWORKROOT/runlog/logfile

source ../env.sh
