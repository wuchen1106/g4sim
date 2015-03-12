export MYG4SIMROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export G4SIMEXEC=$MYG4SIMROOT/bin/g4sim
if ! echo $LD_LIBRARY_PATH | grep -q "$MYG4SIMROOT"; then
	export LD_LIBRARY_PATH=$MYG4SIMROOT/lib:$LD_LIBRARY_PATH
fi
if ! echo $PATH | grep -q "$MYG4SIMROOT"; then
	export PATH=$MYG4SIMROOT/bin:$PATH
fi

source geant4-install/bin/geant4.sh
source geant4-install/share/Geant4-*/geant4make/geant4make.sh
source ../AlcapDAQ/root-install/bin/thisroot.sh


export ALCAPWORKROOT=$MYG4SIMROOT/alcap

export PHYSICSLIST=QGSP_BERT           # QGSP_BERT;QGSP_BERT_HP;QGSP_BERT_noRadi;QGSP_BERT_HP_noRadi

#for configuration cards
export MATERIALLISTROOT=material_list
export MAGFIELDCARDROOT=mag/MagField_none
export OUTCARDROOT=output/output_default
export GEOCARDROOT=geometry_R2013/archive/geometry_1217
export GENFILEROOT=gen/gen_default

#for directories
export MACROSROOT=$ALCAPWORKROOT/macros
export MACROSCOMMONROOT=$MACROSROOT/common
export CONFIGUREROOT=$ALCAPWORKROOT/configure/
export ALCAPDATAROOT="$ALCAPWORKROOT/data/"
export CONFIGUREDATAROOT=$ALCAPDATAROOT
export ALCAPFIELDMAPSROOT="$ALCAPWORKROOT/fieldMaps/"
export FIELDMAPSROOT=$ALCAPFIELDMAPSROOT
export OFILENAMEROOT=$ALCAPWORKROOT/output/raw_g4sim.root
export RUNNAMEROOT=TEST
export LOGFILEROOT=$ALCAPWORKROOT/runlog/logfile
