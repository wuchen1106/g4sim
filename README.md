g4sim
=====

Simulation toolkit based on Geant4 and ROOT 

###1. Installation
Prerequirements:  
1.	CMake installed  
2.	Geant4 installed  
3.	ROOT installed  

	$ cmake .
	$ make
	
###2. How to use
Prerequirements:  
	
1. 	Geant4 environment is set already. If not, then  
`source /path/to/geant4/bin/geant4.sh`  
`source /path/to/geant4/share/Geant4-*/geant4make/geant4make.sh`
	
2.	ROOT environment is set already. If not, then  
`source /path/to/root/bin/thisroot.sh`

3.	g4sim basic environment is set already. If not, then  
`source /path/to/g4sim/env.sh`

To start:  

	$ source setup/test.sh       # Choose anyone you like, or write another by yourself
	$ ./g4sim [macros/run.mac]         # Choose any Geant4 macro file you want.
	                                   # without macro file it would enter interactive mode.
	                                   
To explain:  

1.	Interactive mode would apply **macros/vis.mac**, which would call `vis/open OGL`.
		If you don't have `-DGEANT4_USE_RAYTRACER_X11=ON` when configure Geant4
		then you may want to change these command.

2.	A file like in **setup** directory would set up a specific environment for a certain study purpose
  
###2. Examples
1. Study the penetrating rate of anti-proton through Aluminium target
