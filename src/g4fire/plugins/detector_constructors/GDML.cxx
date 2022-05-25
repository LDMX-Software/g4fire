#include <g4fire/user/DetectorConstruction.h>

#include <G4GDMLParser.hh>

namespace g4fire::plugins::detector_construction {

class GDML {
  std::string gdml_file_path_;
 public:
  GDML(const fire::config::Parameters& p)
    : user::DetectorConstruction(p) {
      gdml_file_path_ = p.get<std::string>("file_path");
    }
  virtual void RecordConfig(fire::RunHeader& rh) const final override {
    rh.set("GDML File Path", gdml_file_path_);
  }
  virtual G4VPhysicalVolume* Construct() {
    G4GDMLParser parser;
    parser.Read(gdml_file_path_);
    return parser.GetWorldVolume();
  }
};

}

DECLARE_DETECTOR_CONSTRUCTION(g4fire::plugins::detector_construction::GDML);
