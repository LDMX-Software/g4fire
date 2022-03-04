
#
# Process the Geant4 targets so they are modern cmake compatible.
#
function(setup_geant4_target)

  # Search for Geant4 and load its settings
  find_package(Geant4 REQUIRED gdml ui_all vis_all)

  # Create an imported Geant4 target if it hasn't been done yet.
  if(NOT TARGET Geant4::Interface)

    # Geant4_DEFINITIONS already include -D, this leads to the error
    # <command-line>:0:1: error: macro names must be identifiers if not removed.
    set(G4_DEF_TEMP "")
    foreach(def ${Geant4_DEFINITIONS})
      string(REPLACE "-D" "" def ${def})
      list(APPEND G4_DEF_TEMP ${def})
    endforeach()
    set(Geant4_DEFINITIONS ${G4_DEF_TEMP})
    unset(G4_DEF_TEMP)

    # Create the Geant4 target
    add_library(Geant4::Interface INTERFACE IMPORTED GLOBAL)

    # Set the target properties
    set_target_properties(
      Geant4::Interface
      PROPERTIES INTERFACE_LINK_LIBRARIES "${Geant4_LIBRARIES}"
                 INTERFACE_COMPILE_OPTIONS "${Geant4_Flags}"
                 INTERFACE_COMPILE_DEFINITIONS "${Geant4_DEFINITIONS}"
                 INTERFACE_INCLUDE_DIRECTORIES "${Geant4_INCLUDE_DIRS}")

    message(STATUS "Found Geant4 version ${Geant4_VERSION}")

  endif()

endfunction()






