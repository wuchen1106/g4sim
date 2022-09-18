#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(G4SIM)

# for ROOT6 compilation, use c++11
set(CMAKE_CXX_STANDARD 11)

# --- Default build type if none was specified ---
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

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
# Setup Geant4 include directories and compile definitions
include(${Geant4_USE_FILE})
#include_directories(${Geant4_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Find ROOT (required package)
#
find_package(ROOT REQUIRED COMPONENTS RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread)
if(NOT ROOT_FOUND)
  message(FATAL_ERROR "G4 Examples: ROOT package not found. --> P01 example disabled")  
endif()
if(NOT ROOT_INCLUDE_DIR)
	set(ROOT_INCLUDE_DIR ${ROOT_INCLUDE_DIRS})
endif()
include_directories(${ROOT_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Setup include directory for this project
#
include_directories(${PROJECT_SOURCE_DIR}/src)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/src/*.hh)

##----------------------------------------------------------------------------
## P01 requires shared libraries
##
#if(BUILD_STATIC_LIBS AND NOT BUILD_SHARED_LIBS )
#  message(STATUS "G4 Examples: Building only Static Libraires. --> P01 example disabled")
#  return()
#endif()
#
##----------------------------------------------------------------------------
## Set default directories
##
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/../bin)
#set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/../lib)
#
##----------------------------------------------------------------------------
## Locate link files for this project
##
#set(LINK_DIRECTORIES
#		${ROOT_LIBRARY_DIR}
#	 )
#link_directories( ${LINK_DIRECTORIES})
	 
##----------------------------------------------------------------------------
## Generate dictionaries, add ROOT libraries properties
##
##REFLEX_GENERATE_DICTIONARY(ExP01Classes include/ExP01Classes.hh SELECTION xml/selection.xml)
#add_library(g4simlib SHARED ${sources})
#set(libsuffix .so)
#set(ROOT_LIBRARY_PROPERTIES ${ROOT_LIBRARY_PROPERTIES} SUFFIX ${libsuffix})
#set_target_properties(g4simlib PROPERTIES ${ROOT_LIBRARY_PROPERTIES})
##target_link_libraries(g4simlib ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} Cintex Reflex)
#target_link_libraries(g4simlib ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(g4sim ${sources} ${headers})
target_link_libraries(g4sim ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS g4sim DESTINATION bin)
