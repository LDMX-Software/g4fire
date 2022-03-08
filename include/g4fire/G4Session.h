#ifndef G4FIRE_G4SESSION_H
#define G4FIRE_G4SESSION_H

#include <fstream>
#include <iostream>

#include "G4UIsession.hh"

namespace g4fire {

/**
 * Log the output of Geant4 to files in current directory.
 */
class LoggedSession : public G4UIsession {
 public:
  /**
   * Constructor
   *
   * Sets up output file streams for the cout and cerr paths.
   */
  LoggedSession(const std::string& cout_file_name = "G4cout.log",
                const std::string& cerr_file_name = "G4cerr.log");

  /**
   * Destructor
   *
   * Closes the output files streams
   */
  ~LoggedSession();

  /**
   * Required hook for Geant4
   *
   * Does nothing
   */
  G4UIsession* SessionStart() { return nullptr; }

  /**
   * Redirects cout to file
   */
  G4int ReceiveG4cout(const G4String& message);

  /**
   * Redirects cerr to file
   */
  G4int ReceiveG4cerr(const G4String& message);

 private:
  /** cout log file */
  std::ofstream cout_file_;

  /** cerr log file */
  std::ofstream cerr_file_;

};  // LoggedSession

/**
 * @class BatchSession
 *
 * Do _nothing_ with G4cout and G4cerr messages. This is made to improve
 * performance.
 */
class BatchSession : public G4UIsession {
 public:
  /**
   * Constructor
   */
  BatchSession() {}

  /**
   * Destructor
   */
  ~BatchSession() {}

  /**
   * Required hook for Geant4
   *
   * Does nothing
   */
  G4UIsession* SessionStart() { return nullptr; }

  /**
   * Does nothing with input
   */
  G4int ReceiveG4cout(const G4String&) { return 0; }

  /**
   * Does nothing with input
   */
  G4int ReceiveG4cerr(const G4String&) { return 0; }
};

}  // namespace g4fire

#endif
