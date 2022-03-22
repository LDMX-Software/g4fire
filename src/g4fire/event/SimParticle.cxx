#include "g4fire/event/SimParticle.h"

#include <iostream>

namespace g4fire::event {

SimParticle::ProcessTypeMap SimParticle::createProcessTypeMap() {
  
  ProcessTypeMap procMap;
  /// e Z --> e Z gamma
  procMap["eBrem"] = ProcessType::eBrem;
  /// gamma --> e+ e-
  procMap["conv"] = ProcessType::conv;
  /// e+ e- --> gamma gamma
  procMap["annihil"] = ProcessType::annihil;
  /// gamma e --> gamma e
  procMap["compt"] = ProcessType::compt;
  /// gamma Z --> e- Z
  procMap["phot"] = ProcessType::phot;
  /// Electron ionization
  procMap["eIoni"] = ProcessType::eIoni;
  /// Multiple scattering
  procMap["msc"] = ProcessType::msc;
  /// gamma Z --> Z + X
  procMap["photonNuclear"] = ProcessType::photonNuclear;
  /// e Z --> e Z + X
  procMap["electronNuclear"] = ProcessType::electronNuclear;
  /// gamma --> mu+ mu-
  procMap["GammaToMuPair"] = ProcessType::GammaToMuPair;
  /// e- Z --> e- Z A'
  procMap["eDarkBrem"] = ProcessType::eDarkBrem;
  return procMap;

}

SimParticle::ProcessTypeMap SimParticle::PROCESS_MAP =
    SimParticle::createProcessTypeMap();

void SimParticle::clear() {
  daughters_.clear();
  parents_.clear();

  energy_ = 0;
  pdg_id_ = 0;
  gen_status_ = -1;
  time_ = 0;
  x_ = 0;
  y_ = 0;
  z_ = 0;
  endx_ = 0;
  endy_ = 0;
  endz_ = 0;
  px_ = 0;
  py_ = 0;
  pz_ = 0;
  endpx_ = 0;
  endpy_ = 0;
  endpz_ = 0;
  mass_ = 0;
  charge_ = 0;
  //processType_ = ProcessType::unknown;
  vertex_vol_ = "";
}

void SimParticle::attach(fire::io::Data<SimParticle>& d) { 
  d.attach("energy", energy_);
  d.attach("pdg_id", pdg_id_);
  d.attach("gen_status", gen_status_);
  d.attach("time", time_);
  d.attach("x", x_);
  d.attach("y", y_);
  d.attach("z", z_);
  d.attach("endx", endx_);
  d.attach("endy", endy_);
  d.attach("endz", endz_);
  d.attach("px", px_);
  d.attach("px", py_);
  d.attach("pz", pz_);
  d.attach("endpx", endpx_);
  d.attach("endpy", endpy_);
  d.attach("endpz", endpz_);
  d.attach("mass", mass_);
  d.attach("charge", charge_);
  //d.attach(processType_ = ProcessType::unknown;
  d.attach("vertex_vol", vertex_vol_);
}

/*
void SimParticle::Print() const {
  std::cout << "SimParticle { "
            << "energy: " << energy_ << ", "
            << "PDG ID: " << pdg_id_ << ", "
            << "gen_status: " << gen_status_ << ", "
            << "time: " << time_ << ", "
            << "vertex: ( " << x_ << ", " << y_ << ", " << z_ << " ), "
            << "endPoint: ( " << endx_ << ", " << endy_ << ", " << endz_
            << " ), "
            << "momentum: ( " << px_ << ", " << py_ << ", " << pz_ << " ), "
            << "endPointMomentum: ( " << endpx_ << ", " << endpy_ << ", "
            << endpz_ << " ), "
            << "mass: " << mass_ << ", "
            << "nDaughters: " << daughters_.size() << ", "
            << "nParents: " << parents_.size() << ", "
            << "processType: " << processType_ << ", "
            << "vertex volume: " << vertex_vol_ << " }" << std::endl;
}*/

SimParticle::ProcessType SimParticle::findProcessType(std::string processName) {
  if (processName.find("biasWrapper") != std::string::npos) {
    std::size_t pos = processName.find_first_of("(") + 1;
    processName = processName.substr(pos, processName.size() - pos - 1);
  }

  if (PROCESS_MAP.find(processName) != PROCESS_MAP.end()) {
    return PROCESS_MAP[processName];
  } else {
    return ProcessType::unknown;
  }
}
} // namespace g4fire::event
