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

# Execute the command to extract the SHA1 hash of the current git tag.
# 'git' is removed from within the container to discourage opening a shell
# in the container, so we need to go to some lengths in order to avoid using 'git'
# The variable GIT_SHA1 will be set to contain the hash.
set(git_dir "${PROJECT_SOURCE_DIR}/.git")
if (NOT IS_DIRECTORY "${git_dir}")
  # we are a submodule of another project
  file(READ "${git_dir}" git_dir)
  string(REGEX REPLACE "^gitdir: " "" git_dir ${git_dir})
  string(REGEX REPLACE "\n$" "" git_dir ${git_dir})
endif()
file(READ "${git_dir}/HEAD" current_ref)
string(REGEX REPLACE "\n$" "" current_ref ${current_ref})
if (current_ref MATCHES "^ref:.*$")
  # on a branch
  string(REGEX REPLACE "^ref: " "" current_ref "${current_ref}")
  file(READ "${git_dir}/${current_ref}" GIT_SHA1)
  string(REGEX REPLACE "\n$" "" GIT_SHA1 ${GIT_SHA1})
else()
  # in detached head state (probably on a tag)
  set(GIT_SHA1 ${current_ref})
endif()
message(STATUS "Deduced git SHA: ${GIT_SHA1}")

# Copies the file 'Version.h.in', substitutes the value of GIT_SHA1 and writes
# it out to Version.h.
configure_file(${PROJECT_SOURCE_DIR}/include/g4fire/version/Version.h.in
              ${PROJECT_BINARY_DIR}/include/g4fire/version/Version.h)
install(FILES ${PROJECT_BINARY_DIR}/include/g4fire/version/Version.h
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/g4fire/version)

add_library(version INTERFACE)
target_include_directories(version
  INTERFACE
  "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>"
  )

add_library(user INTERFACE)
target_include_directories(user INTERFACE 
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>")
target_link_libraries(user INTERFACE Geant4::Interface fire::config)

add_library(g4user SHARED
  src/g4fire/g4user/EventAction.cxx
  src/g4fire/g4user/RunAction.cxx
  src/g4fire/g4user/SteppingAction.cxx
  src/g4fire/g4user/StackingAction.cxx
  src/g4fire/g4user/TrackingAction.cxx
  src/g4fire/g4user/PrimaryGeneratorAction.cxx
  )
target_link_libraries(g4user PUBLIC user)

add_library(lhe SHARED
  src/g4fire/lhe/LHEEvent.cxx
  src/g4fire/lhe/LHEParticle.cxx
  src/g4fire/lhe/LHEReader.cxx)
target_include_directories(lhe INTERFACE 
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>")

add_library(simulator SHARED 
  src/g4fire/G4Session.cxx
  src/g4fire/UserEventInformation.cxx
  src/g4fire/UserRegionInformation.cxx
  src/g4fire/UserTrackInformation.cxx
  src/g4fire/Simulator.cxx)
target_link_libraries(simulator PUBLIC g4user fire::framework)

add_library(primary_generators SHARED
  src/g4fire/primary_generators/ParticleGun.cxx
  src/g4fire/primary_generators/GeneralParticleSource.cxx
  src/g4fire/primary_generators/LHEPrimaryGenerator.cxx
  src/g4fire/primary_generators/MultiParticleGunPrimaryGenerator.cxx
  )
target_link_libraries(primary_generators PUBLIC user lhe)

add_library(detector_constructors SHARED
  src/g4fire/detector_constructors/SimplePrism.cxx
  src/g4fire/detector_constructors/GDML.cxx)

set_target_properties(
  user g4user lhe simulator primary_generators detector_constructors
  PROPERTIES CXX_STANDARD 17
             CXX_STANDARD_REQUIRED YES
             CXX_EXTENSIONS NO
             PREFIX "libg4fire_")

install(TARGETS 
  user g4user lhe simulator primary_generators detector_constructors
  EXPORT g4fireTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
  )
install(DIRECTORY include DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY python DESTINATION python FILES_MATCHING
  PATTERN "*.py" # INCLUDE
  PATTERN "test_*" EXCLUDE
  PATTERN ".pytest_cache" EXCLUDE
  PATTERN "__pycache__" EXCLUDE
  )

install(EXPORT g4fireTargets
  FILE g4fireTargets.cmake
  NAMESPACE g4fire::
  DESTINATION lib/cmake/g4fire)

include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/g4fireConfig.cmake"
  INSTALL_DESTINATION lib/cmake/g4fire)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/g4fireConfigVersion.cmake"
  COMPATIBILITY AnyNewerVersion)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/g4fireConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/g4fireConfigVersion.cmake
  DESTINATION lib/cmake/g4fire)

