# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(
  g4fire
  VERSION 
    0.1
  DESCRIPTION 
    "Geant4 based simulation using the fire event processing framework."
  LANGUAGES 
    C CXX
)

# Set the default release type to "Release". If a release type is specified 
# at the command line, it's respected.
set(default_build_type "Release")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
endif ()

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif ()

# Load additional macros used by this project
list (APPEND CMAKE_MODULE_PATH ${g4fire_SOURCE_DIR}/cmake)

# Load the Geant4 module. This is used to setup a proper target for Geant4
include (Geant4)

# Dependencies
setup_geant4_target ()
find_package (Boost REQUIRED COMPONENTS log)
find_package (fire REQUIRED)
find_package (Eigen3 REQUIRED NO_MODULE)

set (dark_brem_sources
  ${g4fire_SOURCE_DIR}/src/g4fire/darkbrem/APrimePhysics.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/darkbrem/G4APrime.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/darkbrem/DarkBremVertexLibraryModel.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/darkbrem/G4eDarkBremsstrahlung.cxx
)

set (event_sources
  ${g4fire_SOURCE_DIR}/src/g4fire/event/EventBuilder.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/event/SimParticle.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/event/SimCalorimeterHit.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/event/SimTrackerHit.cxx
)

set (geo_sources
  ${g4fire_SOURCE_DIR}/src/g4fire/geo/AuxInfoReader.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/geo/GDMLParser.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/geo/ParserFactory.cxx
)

set (sim_sources
  ${g4fire_SOURCE_DIR}/src/g4fire/DetectorConstruction.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/G4Session.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/GammaPhysics.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/GeneralParticleSource.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/LHEEvent.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/LHEParticle.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/LHEPrimaryGenerator.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/LHEReader.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/MagneticFieldMap3D.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/ParallelWorld.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/ParticleGun.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/PluginFactory.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/PrimaryGeneratorAction.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/PrimaryGenerator.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/RunManager.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/Simulator.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/TrackMap.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/UserAction.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/UserEventAction.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/UserEventInformation.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/UserRegionInformation.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/UserRunAction.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/UserStackingAction.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/UserTrackInformation.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/UserTrackingAction.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/USteppingAction.cxx
  ${g4fire_SOURCE_DIR}/src/g4fire/XsecBiasingOperator.cxx
)

add_library(g4fire SHARED 
  ${dark_brem_sources}
  ${event_sources}
  ${geo_sources}
  ${sim_sources}
)
target_link_libraries(g4fire PUBLIC 
  Geant4::Interface fire::io fire::framework Eigen3::Eigen)
target_include_directories(g4fire PUBLIC ${g4fire_SOURCE_DIR}/include/)

install(TARGETS g4fire 
  LIBRARY DESTINATION lib
  INCLUDES DESTINATION include
)

install(DIRECTORY python/ DESTINATION python FILES_MATCHING 
  PATTERN "*.py" 
)

# Unpack the example dark brem vertex library (or libraries)
#file(GLOB vertex_libraries data/*.tar.gz)

#option(INSTALL_EXAMPLE_DB_LIBRARIES 
#  "Unpack and install the example dark brem event libraries." ON)
#if(INSTALL_EXAMPLE_DB_LIBRARIES)
  # set installation location of dark brem vertex libraries
  #   also used when configuring the makePath.py module
  #  set(DB_VERTEX_LIB_INSTALL ${CMAKE_INSTALL_PREFIX}/data/db_vertex_libraries)
  #  if(NOT EXISTS ${DB_VERTEX_LIB_INSTALL})
  #      #assume new build ==> unpack vertex libraries
  #      message(STATUS "Unpacking example dark brem vertex libraries.")
  #      file(MAKE_DIRECTORY ${DB_VERTEX_LIB_INSTALL})
  #      foreach(library_path ${vertex_libraries})
  #          execute_process(COMMAND 
  #              tar -C ${DB_VERTEX_LIB_INSTALL} --skip-old-files -zxf ${library_path})
  #      endforeach()
  #  endif()
  
  # Add compiled tests
  #configure_file(${PROJECT_SOURCE_DIR}/test/DarkBremProcessTest.cxx.in 
  #               ${PROJECT_SOURCE_DIR}/test/DarkBremProcessTest.cxx)
  #setup_test(dependencies SimCore::SimCore Geant4::Interface)
  
  # Configure the executable so it knows the installation path of the db vertex libraries
  #configure_file(${PROJECT_SOURCE_DIR}/src/SimCore/DarkBrem/print_dark_brem_xsec_table.cxx.in
  #               ${CMAKE_CURRENT_BINARY_DIR}/bin/print_dark_brem_xsec_table.cxx)
  #add_executable(print-dark-brem-xsec-table 
  #               ${CMAKE_CURRENT_BINARY_DIR}/bin/print_dark_brem_xsec_table.cxx)
  
  # Link to the Framework library
  #target_link_libraries(print-dark-brem-xsec-table 
  #                      PRIVATE Geant4::Interface 
  #                              SimCore::SimCore 
  #                              Framework::Framework)
  
  # Install the fire executable
  #install(TARGETS print-dark-brem-xsec-table 
  #        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
  #endif()
