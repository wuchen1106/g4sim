g4sim
=====

Simulation toolkit based on Geant4 and ROOT 

# 1. Installation
## Prerequirements:  
1.	CMake installed

	If CMake is not provided by your package manager, you can visit [CMake](http://www.cmake.org/) for source code then install CMake in this way:

		cd CMake
		./bootstrap
		./configure --prefix=path/to/CMake
		make
		make install
	
2.	Geant4 installed  
	[Geant4](http://geant4.cern.ch/)  
	If you want a UI with QT & OpenGL, please configure while installation.
	You can follow this way:

		cmake -DCMAKE_INSTALL_PREFIX=path/to/Geant4\
			  -DGEANT4_INSTALL_DATA=ON\
			  -DGEANT4_USE_QT=ON\
			  -DGEANT4_USE_OPENGL_X11=ON\
			  -DGEANT4_USE_RAYTRACER_X11=ON\
	      		  path/to/Geant4-source
		make
		make install
	      
	~~CLHEP and XERCES are required by Geant4. If they are not installed in standard directories, please specify:~~  
            ~~-DGEANT4_USE_SYSTEM_CLHEP=ON -DCLHEP_ROOT_DIR=path/to/CLHEP -DXERCESC_ROOT_DIR=path/to/XERCES~~

        Now by default, Geant4 will install its own CLHEP.


3.	ROOT installed  
	[ROOT](http://root.cern.ch/drupal/)  
	You can install ROOT in this way:

		cd root
		./configure --enable-opengl\
		          --enable-soversion\
		          --enable-mathcore\
		          --enable-mathmore\
		          --enable-gdml\
		          --enable-python\
		          --enable-roofit\
		          --enable-minuit2
		make
		
	If you met an error saying mathmore is not installed, a possible reason can be that GSL is not installed in a standard directory. You can configure with:

		--with-gsl-incdir=path/to/GSL/include --with-gsl-libdir=path/to/GSL/lib

## Install:

	mkdir build
	cd build
	cmake ..
	make

	Especially if you are working with gcc at a non-standard location, please run 
	
		export CC=`which gcc`
		export CXX=`which g++`

	before you run `cmake ..`.
	If you already called `cmake ..` but want to change the gcc directory, please remove CMakeCache.txt and then export `CC` and `CXX` as above and call `cmake ..` again.
	
# 2. How to use
## Prerequirements:  
	
1. 	Geant4 environment is set already.

	If not, then  

		source /path/to/geant4/bin/geant4.sh
		source /path/to/geant4/share/Geant4-*/geant4make/geant4make.sh
	
2.	ROOT environment is set already.

	If not, then  

		source /path/to/root/bin/thisroot.sh

3.	Your working directory has been created.

	`Example` is a working directory for a simple simulation to show how `g4sim` works.
	It also serves as a template for someone who want to build another simulation.

4.	g4sim basic environment is set already.

	If you have already set environment variable `$MYG4SIMROOT` to the directory of `g4sim`,
	you can go to working directory,
	otherwise you will need to  

			source env.sh  
	Then go to your working directory and set environment:  

			cd Example # or whatever working directory
			source setenv.sh

## To Start:  

Type `g4sim -h` for the help message.

```
Usage g4sim [options] [macro]
		 In case [macro] is not given, UI mode will be activated
[options]
	 -e TYPE
		 Change the EmType. By default it's 0 (EmStandard). Only valid when the "PhysicsList" is chosen
	 -E energy
		 Change the default kinetic energy of the primary generator. Unit is MeV.
		 Available types: -1, EmLivermore; -2, EmCustomised; 0,3,4 EmStandard with the type number as option
	 -L cut
		 Change the low energy bound of production cuts to [cut] eV (9.900e+02 eV)
	 -N nEvents 
		 Added /run/beamOn nEvents command after the execution of the given macro. Will not be invoked if macro file is not given though.
	 -P physics
		 Change the physics List (QGSP_BERT_HP)
		 Available options: QGSP_BERT QGSP_BERT_HP QGSP_INCLXX PhysicsList
		 If the provided option is not supported, then QGSP_BERT_HP will be used as default
	 -o
		 Register G4OpticalPhysics
	 -p
		 Apply PAI to world
	 -r
		 Register G4RadioactiveDecayPhysics
	 -O file
		 Set output file name [g4sim/Example/output/raw_g4sim.root]
```

A figure to show how it works:  
![Flow Diagram](docs/FlowDiagram.png "Flow Diagram")  
	                                   
## To Configure:  
By default (set in `setenv.sh`):

	GENFILEROOT=gen/gen_mum_up
	GEOCARDROOT=geometry_1217
	OUTCARDROOT=output/output_default
	
With this default environment set already you can start simulation with `/run/beamOn XXX` immediately.  

### GENFILEROOT:  
We provide couple ways to configure:  

1.	Use another configure file  

Currently we have these optional choices:  

	gen_mum_up      # Generate muon from upstream before MuPC
	gen_gam_tgt     # Generate gamma from target position

You can either set the environment variable `$GENFILEROOT` or use macro command `/g4sim/gun/ResetGen gen/gen_mum_up` in Geant4 session to reset generator setting based on this given configure file.  

2.	Modify some parameters using macro commands

Here are a couple of macro commands we can use:

### Field options
By default there is no field in the simulation.

To add in field we have 2 options.
1. Using uniform field. Just add the following lines to the macro file before the `/g4sim/det/update` command:
```
/g4sim/mag/Reset
/g4sim/mag/ReadCard         mag/MagField_uniform
```
In the configure card `mag/MagField_uniform` you can set the field value.

2. Using a field map. In this case add the following lines to the macro file before the `/g4sim/det/update` command:
```
/g4sim/mag/Reset
/g4sim/mag/AddMap           cap1.map 1
```
Here the `cap1.map` must be presentated in a certain format under the `fieldMaps` directory.  
The number `1` in the end of the line denotes the scaling factor for the field values given in the map.  
Here is an example of a field map (`cap1.map`):
```
param normB=-1
grid X0=-600.0 Y0=0.0 Z0=-1000.0 nX=13 nY=7 nZ=41 dX=100.0 dY=100.0 dZ=100.0
extendY flip=By
data
600.0 0.0 -1000.0 1.70498279459 0.0 -2.94780127359
...
```
Here the first line is optional. It denotes the normalization factor.  
The second line is necessary. It defines the sampling grid, where `X0, Y0, Z0` are the start point of the field map, `dx dY dZ` are the grid sizes on the 3 directions. `nX nY nZ` are the total number of points on each direction. Please be careful that when your map contains `N` grids, the number of points is `N+1`. The unit here is `mm`.  
The third line is also optional. It defines how to extend the map. In this case the map is extended on the `Y` direction. With `flip=By` this means the `By` values in the extended side will be flipped. This is a mirror extention.
The forth line here denotes that following lines are data points.
The data points should be given in the order of `X Y Z Bx By Bz` where the units are `mm` and `Tesla`
