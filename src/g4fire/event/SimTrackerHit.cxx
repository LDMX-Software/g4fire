#include "g4fire/event/SimTrackerHit.h"

namespace g4fire::event {

void SimTrackerHit::clear() {
  id_ = 0;
  layer_id_ = 0;
  module_id_ = 0;
  edep_ = 0;
  time_ = 0;
  px_ = 0;
  py_ = 0;
  pz_ = 0;
  x_ = 0;
  y_ = 0;
  z_ = 0;
  energy_ = 0;
  path_length_ = 0;
  track_id_ = -1;
  pdg_id_ = 0;
}

void SimTrackerHit::attach(fire::io::Data<SimTrackerHit> &d) { 
  d.attach("id", id_); 
  d.attach("layer_id", layer_id_); 
  d.attach("module_id", module_id_); 
  d.attach("edep", edep_);
  d.attach("time", time_); 
  d.attach("x", x_);
  d.attach("y", y_);
  d.attach("z", z_);
  d.attach("px", px_);
  d.attach("py", py_);
  d.attach("pz", pz_);
  d.attach("energy", energy_);
  d.attach("path_length", path_length_); 
  d.attach("track_id", track_id_);
  d.attach("pdg_id", pdg_id_);
}

std::ostream &operator<<(std::ostream &output, const SimTrackerHit &hit) {
  output << "---[ SimTrackerHit ] {\n"
         << "\tID: " << hit.id() << "\n"
         << "\tLayer ID: " << hit.layerID() << "\n"
         << "\tModule ID: " << hit.moduleID() << "\n"
         << "\tPosition (mm): ( " << hit.position()[0] << ", "
         << hit.position()[1] << ", " << hit.position()[2] << ")\n"
         << "\tEnergy Deposited (MeV): " << hit.edep() << "\n"
         << "\tEnergy (MeV): " << hit.energy() << "\n"
         << "\tTime (ns): " << hit.time() << "\n"
         << "\tMomentum (MeV): ( " << hit.momentum()[0] << ", "
         << hit.momentum()[1] << ", " << hit.momentum()[2] << " )\n"
         << "\tPath length: " << hit.pathLength() << "\n"
         << "\tTrack ID: " << hit.trackID() << "\n"
         << "\tPDG ID: " << hit.pdgID() << "\n"
         << " }" << std::endl;

  return output;
}
} // namespace g4fire::event
