#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(G4SIM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")

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

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})
#include(${Geant4_USE_FILE} ${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# Find ROOT (required package)
#
find_package(ROOT QUIET)
if(NOT ROOT_FOUND)
  message(STATUS "G4 Examples: ROOT package not found. --> P01 example disabled")  
  return()
endif()
#if(NOT GCCXML)
#   message(STATUS "G4 Examples: GCCXML not found. --> P01 example disabled")
#   return()
#endif() 

#----------------------------------------------------------------------------
# P01 requires shared libraries
#
if(BUILD_STATIC_LIBS AND NOT BUILD_SHARED_LIBS )
  message(STATUS "G4 Examples: Building only Static Libraires. --> P01 example disabled")
  return()
endif()

#----------------------------------------------------------------------------
# Set default directories
#
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/../bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/../lib)

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
include_directories(${PROJECT_SOURCE_DIR}/../include 
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/../src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/../include/*.hh)

#----------------------------------------------------------------------------
# Generate dictionaries, add ROOT libraries properties
#
#REFLEX_GENERATE_DICTIONARY(ExP01Classes include/ExP01Classes.hh SELECTION xml/selection.xml)
add_library(g4simlib SHARED ${sources})
set(libsuffix .so)
set(ROOT_LIBRARY_PROPERTIES ${ROOT_LIBRARY_PROPERTIES} SUFFIX ${libsuffix})
set_target_properties(g4simlib PROPERTIES ${ROOT_LIBRARY_PROPERTIES})
target_link_libraries(g4simlib ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} Cintex Reflex)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(g4sim ${sources} ${headers})
target_link_libraries(g4sim g4simlib ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

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

