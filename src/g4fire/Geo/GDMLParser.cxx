#include "g4fire/Geo/GDMLParser.h"

namespace g4fire {
namespace geo {

GDMLParser::GDMLParser(framework::config::Parameters &parameters,
                       g4fire::ConditionsInterface &ci) {
  parser_ = std::make_unique<G4GDMLParser>();
  info_ =
      std::make_unique<g4fire::geo::AuxInfoReader>(parser_.get(), parameters, ci);
  parameters_ = parameters;
}

G4VPhysicalVolume *GDMLParser::GetWorldVolume() {
  return parser_->GetWorldVolume();
}

void GDMLParser::read() {
  parser_->Read(parameters_.getParameter<std::string>("detector"),
                parameters_.getParameter<bool>("validate_detector"));
  info_->readGlobalAuxInfo();
  info_->assignAuxInfoToVolumes();
  detector_name_ = info_->getDetectorHeader()->getName();
}

}  // namespace geo
}  // namespace g4fire
