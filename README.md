g4sim
=====

Simulation toolkit based on Geant4 and ROOT 

###1. Installation
Prerequirements:  
1.	CMake installed  
2.	Geant4 installed  
3.	ROOT installed  

Install:

	$ cmake .
	$ make
	
###2. How to use
Prerequirements:  
	
1. 	Geant4 environment is set already. If not, then  
`source /path/to/geant4/bin/geant4.sh`  
`source /path/to/geant4/share/Geant4-*/geant4make/geant4make.sh`
	
2.	ROOT environment is set already. If not, then  
`source /path/to/root/bin/thisroot.sh`

3.	Your work directory has been created.
Work direcotories should be put under "/path/to/g4sim/WorkDirectory".
There is a template. You can just create your own following it.
Remember to change "MYG4SIMSUBROOT" in "setenv.sh" to the directory name you created.
The simplest way is ``$ cp -r Template Default``. By default it can work.

4.	g4sim basic environment is set already. If not, go to your work directory and
`source env.sh`

To start:  

	$ g4sim [macros/run.mac]           # Choose any Geant4 macro file you want.
	                                   # without macro file it would enter interactive mode.
	                                   
To explain:  

1.	Interactive mode would apply **macros/vis.mac**, which would call `vis/open OGL`.
		If you don't have `-DGEANT4_USE_RAYTRACER_X11=ON` when you configured Geant4
		then you may want to change these command.
