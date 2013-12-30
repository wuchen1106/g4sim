#!/bin/sh

#	for detector construction
	export MATERIALLISTROOT=material_list
#	export GEOCARDROOT=geometry_A9_2p4_0927
#	export GEOCARDROOT=geometry_A9_2p4_0927_4v
#	export GEOCARDROOT=geometry_A9_2p4_0731
#	export GEOCARDROOT=geometry_A9_2p4_0731_simple
#	export GEOCARDROOT=geometry_A9_2p4_0731_4v
#	export GEOCARDROOT=geometry_A9_1p5_0927
	export GEOCARDROOT=geometry_A9_1p5_0927_simple
#	export GEOCARDROOT=geometry_A9_1p5_0927_4v
#	export GEOCARDROOT=geometry_A9_1p5_0731
#	export GEOCARDROOT=geometry_A9_1p5_0731_4v
#	for magField
#	export MAGFIELDCARDROOT=MagField_A9_130927
	export MAGFIELDCARDROOT=MagField_A9_130731
#	for generator
	export GENFILEROOT=gen_signal
#	for output
	export OFILENAMEROOT=$MYG4SIMROOT/output/raw_g4sim.root
	export RUNNAMEROOT=TEST
#	export OUTCARDROOT=output_rpc
	export OUTCARDROOT=output_signal
