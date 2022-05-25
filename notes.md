# Notes on g4fire Design

Geant4 is also event-by-event based so this document is mainly notes on the order
with which to do things.

### Organization
- cmake : modules for custom cmake functions
- include : only header files that will be used by end user
  - put header files internal to g4fire in src alongside implementation file
- src : source files for constructing library
- test : testing for g4fire
- plugins : non-necessary but potentially helpful generic plugins for g4fire
  - built downstream of g4fire
  - will be able to be deactivated with `G4FIRE_ENABLE_PLUGINS=OFF`
- ldmx : ldmx-specific library
  - will be build downstream of g4fire
  - including event objects, SDs, action for storing sim particles

### User Classes
These classes can be derived, registered, and dynamically loaded.

Class | Description | Pre-Defined Options
---|---|---
Physics Constructor | adding custom physics processes and particles | None
Detector | defining detector to be simulated | simple rectangle prism and GDML reader
Sensitive Detectors | generate "hits" in detector volumes | None (maybe Calorimeter/Tracker ?)
Biasing Operator | biasing processes in specific volumes | None
Primary Generators | starting simulation | ParticleGun, LHE, GPS
Generalized Actions | filtering and/or watching at G4 callbacks | Event weight calculator, PN/EN deduction

### Procedure
Separated these tasks into the different Simulator member.

#### Simulator constructor
1. Configure logging (optionally redirect G4cout)

#### Simulator::onProcessStart
2. `G4CascadeParameters` instantiation
    - Not sure on necessity of this, been copied from old versions of SimCore, 
      could be folded into user detector construction?
3. Parse and construct the detector
4. User pre-init G4 commands
5. Construct physics list including user physics constructors
6. Bias physics processes as listed by user
7. Call underlying `G4RunManager::Initialize`
    - This calls `DetectorConstruction::Construct`
8. Construct biasing operators and attach them to configured logical volumes
9. Construct primary generators
10. Construct user actions
11. User post-init, pre-run G4 commands

#### Simulator::beforeNewRun
12. Give run header to user classes to store user parameters

#### Simulator::onNewRun
13. Set G4 seeds using RNSS

#### Simulator::process
14. Process a single G4 event
15. Pass on if event is aborted using `abortEvent`

#### Simulator::onProcessEnd
16. Close up G4 event loop and run its termination
