#include "G4Session.h"

#include <fire/exception/Exception.h>

namespace g4fire {

LoggedSession::LoggedSession(const std::string& cout_file_name,
                             const std::string& cerr_file_name) {
  cout_file_.open(cout_file_name);
  if (not cout_file_.is_open()) {
    throw fire::Exception("G4Logging",
        "Unable to open log file '"+cout_file_name+"'.");
  }

  cerr_file_.open(cerr_file_name);
  if (not cerr_file_.is_open()) {
    throw fire::Exception("G4Logging",
        "Unable to open log file '"+cerr_file_name+"'.");
  }
}

LoggedSession::~LoggedSession() {
  cout_file_.close();
  cerr_file_.close();
}

G4int LoggedSession::ReceiveG4cout(const G4String& message) {
  cout_file_ << message;
  cout_file_.flush();
  return 0;  // 0 return value == sucess
}

G4int LoggedSession::ReceiveG4cerr(const G4String& message) {
  cerr_file_ << message;
  cerr_file_.flush();
  return 0;  // 0 return value == sucess
}
}  // namespace g4fire
