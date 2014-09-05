#!/bin/bash
# install-root.sh: installs the ROOT package in AlcapDAQ from source
#                  creates three directories: root/ -- the source directory
#                                             root-build/ -- the build directory
#                                             root-install/ -- the install directory
# Created by: Andrew Edmonds
# Date: 25-04-2014
# 
# Requirements: Cmake v2.8 or higher

set -e # Make bash stop if an error occurs

if [ -d "geant4-install" ] ;then
	echo It seems Geant4 is already installed.  Check the directory:
	echo "  `pwd`/geant4-install"
	echo and remove it if you wish to replace that version of geant4.
	exit 1
fi

GEANT4_VERSION=4.10.00.p02

# Get the Geant4 tarball
wget http://geant4.cern.ch/support/source/geant${GEANT4_VERSION}.tar.gz

# Unpack the GEANT4 tarball, this creates the geant4/ directory
# using checkpoint and the pipe tricks is to reduce output down to ~20 lines
tar --checkpoint -xzf geant${GEANT4_VERSION}.tar.gz  2>&1 | grep ".*000$"

# Create the other two directories
mkdir -p geant4-build geant4-install

# Use cmake to create the Makefiles for your system
cd geant4-build/
cmake -DCMAKE_INSTALL_PREFIX=../geant4-install -DGEANT4_INSTALL_DATA=ON -DGEANT4_USE_G3TOG4=ON -DGEANT4_USE_OPENGL_X11=ON -DGEANT4_USE_RAYTRACER_X11=ON ../geant${GEANT4_VERSION}/ 2>&1 |tee install_log.txt

# Now compile and install
make 2>&1 |tee -a install_log.txt
make install 2>&1 |tee -a install_log.txt
