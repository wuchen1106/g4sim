#!/bin/sh
export GEANT4VIRSION=geant4.9.6.p01
export GEANT4=$CMHOME/Programs/$GEANT4VIRSION-install
export ROOT=$OTHOME/Programs/root
export G4SIMROOT=$CMHOME/Programs/g4sim
export G4SIMEXE=/afs/ihep.ac.cn/users/w/wuc/geant4_workdir/bin/Linux-g++/g4sim
export WORKAREA=$CMHOME/workarea
export RECONSTRUCT=$WORKAREA/Reconstruct
export SIMULATE=$WORKAREA/Simulate
export ANALYSE=$WORKAREA/Analyse
export DATA=$CMHOME/Data
export GENFIT="/work4/wuchen/genfit/test/CometPhase1"

#Configuration directory
export CONFIGUREROOT=$G4SIMROOT/configure/
export CONFIGUREDATAROOT=$G4SIMROOT/configure/data/
#for detector construction
export MATERIALLISTROOT=material_list.txt
export GEOCARDROOT=geometry_CdcLayer.txt
#for magField
export MAGFIELDCARDROOT=MagField_default.txt
#for generator
export GENFILEROOT=gen_Signal.txt
#for output
export OFILENAMEROOT=$G4SIMROOT/output/raw_g4sim.root
export RUNNAMEROOT=TEST
export OUTCARDROOT=output_default.txt
#for log
export LOGFILEROOT=$G4SIMROOT/runlog/logfile.txt

. $MYBOSSDIR/662/env.sh
. $CMHOME/Programs/geant4.9.6.p01-install/env.sh
. $OTHOME/Programs/root/bin/thisroot.sh
. $CMHOME/Programs/genfit/trunk/env.sh
export LD_LIBRARY_PATH="$OTHOME/Programs/xerces-c-3.1.1/lib:"${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH="/afs/ihep.ac.cn/users/w/wuc/geant4_workdir/tmp/Linux-g++/g4sim:$LD_LIBRARY_PATH"
