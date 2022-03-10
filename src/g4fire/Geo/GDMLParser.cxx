#include "g4fire/Geo/GDMLParser.h"

namespace g4fire {
namespace geo {

GDMLParser::GDMLParser(fire::config::Parameters &params,
                       g4fire::ConditionsInterface &ci) {
  parser_ = std::make_unique<G4GDMLParser>();
  info_ =
      std::make_unique<g4fire::geo::AuxInfoReader>(parser_.get(), params, ci);
  params_ = params;
}

G4VPhysicalVolume *GDMLParser::GetWorldVolume() {
  return parser_->GetWorldVolume();
}

void GDMLParser::read() {
  parser_->Read(params_.get<std::string>("detector"),
                params_.get<bool>("validate_detector", false));
  info_->readGlobalAuxInfo();
  info_->assignAuxInfoToVolumes();
  // detector_name_ = info_->getDetectorHeader()->getName();
}

} // namespace geo
} // namespace g4fire
