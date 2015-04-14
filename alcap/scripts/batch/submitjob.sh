#!/bin/bash
# submitjob.sh -- submits one job to the Merlin cluster
# Usage: ./submitjob.sh N_RUNS NAME N_PARTICLES"

if [ -z $ALCAPWORKROOT ] ; then
    echo "ALCAPWORKROOT not set. You must run env.sh first!"
    exit
fi

if [ $# -ne 3 ] ; then
    echo "Incorrect Usage! You have not passed the correct number of arguments"
    echo "Correct Usage: ./submitjob.sh N_RUNS NAME N_PARTICLES"
    exit
fi

N_RUNS=$1
NAME=$2
N_PARTICLES=$3

echo "N_RUNS="$N_RUNS
echo "NAME="$NAME
echo "N_PARTICLES="$N_PARTICLES

cd $ALCAPWORKROOT

# Create an output directory for this set of runs
OUTPUTDIR=output/$NAME
mkdir -p $OUTPUTDIR
cd $OUTPUTDIR

# Now create the individual run folders
for I_RUN in `seq 1 $N_RUNS`; do
    
    let I_RUN=$I_RUN
    mkdir -p run_$I_RUN
    cd run_$I_RUN

    # Create a configure directory to store the configure files that were used
    CONFIGUREDIR=$PWD/configure
    GEOMDIR=$CONFIGUREDIR/geometry_R2013
    SUBGEOMDIR=$GEOMDIR/sub_all
    mkdir -p $CONFIGUREDIR
    mkdir -p $GEOMDIR
    mkdir -p $SUBGEOMDIR

    ## You may want to change these
    GEOMFILE="geometry_R2013/andy_geometry_Al50_obtain-transfer-matrix"
    GENFILE="gen_mu_1.07_target_hist"
    OUT_CONFIG_FILE="output_default"

    cp $ALCAPWORKROOT/configure/$GEOMFILE $GEOMDIR
    cp $ALCAPWORKROOT/configure/gen/$GENFILE $CONFIGUREDIR
    cp $ALCAPWORKROOT/configure/output/$OUT_CONFIG_FILE $CONFIGUREDIR

    while read -r line
    do 
	SUBGEOMFILE=`echo $line | grep sub | tr -s ' ' | cut -d ' ' -f 3`
	if [[ $SUBGEOMFILE == *"sub"* ]]; then # Check that the subgeo file contains the string "sub"
#	    echo $SUBGEOMFILE
	    cp $ALCAPWORKROOT/configure/$SUBGEOMFILE $SUBGEOMDIR
	    
	    SUBGEOMNAME=`echo $SUBGEOMFILE | cut -d '/' -f 3`
#	    echo $SUBGEOMNAME
            # Change the GEOMFILE so that it looks for the SUBGEOFILEs in the CONFIGUREDIR
	    sed -i 's#'$SUBGEOMFILE'#'$SUBGEOMDIR'/'$SUBGEOMNAME'#' $CONFIGUREDIR/$GEOMFILE
	fi
    done < "$ALCAPWORKROOT/configure/$GEOMFILE"

    # Now write the g4sim input macro
    echo "/control/getEnv ALCAPWORKROOT

# Get default settings
/control/execute {ALCAPWORKROOT}/macros/resetVerbose.mac
/control/execute {ALCAPWORKROOT}/macros/resetCut.mac

# Set Output
/g4sim/myAnalysisSvc/set_out_card "$CONFIGUREDIR/$OUT_CONFIG_FILE"
/g4sim/myAnalysisSvc/set_ofile_name "$ALCAPWORKROOT/output/$NAME"/run_"$I_RUN"/g4sim_"$I_RUN".root

# Set Gen
/g4sim/gun/ResetGen               "$CONFIGUREDIR/$GENFILE"

# Set Geo
/g4sim/det/ReloadGeo              "$CONFIGUREDIR/$GEOMFILE"

/g4sim/myAnalysisSvc/printModulo    10000

#       For this run
/random/setSeeds "$I_RUN" "`expr $I_RUN + 1`"
/run/beamOn  "$N_PARTICLES > g4sim.mac

    qsub $ALCAPWORKROOT/scripts/batch/batch_g4sim.sge
    cd ../
done
