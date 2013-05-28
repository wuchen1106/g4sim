#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(G4SIM)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
	find_package(Geant4 REQUIRED ui_all vis_all)
else()
	find_package(Geant4 REQUIRED)
endif()
find_package(Geant4 REQUIRED)

#----------------------------------------------------------------------------
# set viriables for ROOT
#
include(FindROOT.cmake)

#----------------------------------------------------------------------------
# Setup Geant4 and ROOT include directories and compile definitions
#
include(${Geant4_USE_FILE} ${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# Locate link files for this project
#
#set(LINK_DIRECTORIES
#		${ROOT_LIBRARY_DIR}
#	 )
#link_directories( ${LINK_DIRECTORIES})
	 
#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIR}
                    ${PROJECT_SOURCE_DIR}/include )
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 and ROOT libraries
#
add_executable(g4sim g4sim.cc ${sources} ${headers})
target_link_libraries(g4sim ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build G4SIM. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(G4SIM_SCRIPTS
  )

foreach(_script ${G4SIM_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Add program to the project targets
# (this avoids the need of typing the program name after make)
#
add_custom_target(G4SIM DEPENDS g4sim)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS g4sim DESTINATION bin)

