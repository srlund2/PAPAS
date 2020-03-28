#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(lightGuide)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED )
endif()
#----------------------------------------------------------------------------
# Find ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package (ROOT REQUIRED)
include(${ROOT_USE_FILE})
#----------------------------------------------------------------------------
# Find CADMesh
find_package(cadmesh)
include_directories(${CADMESH_INCLUDE_DIRS})
if(CADMESH_INCLUDE_DIRS)
  add_definitions(-DCADMESH)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE} )

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${Geant4_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(lightGuide lightGuide.cc ${sources} ${headers})
target_link_libraries(lightGuide ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} cadmesh )

add_executable(plotHisto ${DEPENDENCIES} Analysis/plotHisto.cc)
TARGET_LINK_LIBRARIES(plotHisto ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build lightGuide. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(lightGuide_SCRIPTS
    vis.mac
    run1.mac
    beam.mac
    geom.mac
  )

foreach(_script ${lightGuide_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS lightGuide DESTINATION bin )
install(TARGETS plotHisto DESTINATION bin )
install(FILES run1.mac DESTINATION bin )
install(FILES geom.mac DESTINATION bin )
install(FILES beam.mac DESTINATION bin )
install(FILES vis.mac DESTINATION bin)
install(FILES hist.mac DESTINATION bin)
install(DIRECTORY models/ DESTINATION models)
