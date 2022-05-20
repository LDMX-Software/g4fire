#include "g4fire/event/SimCalorimeterHit.h"

namespace g4fire::event {

void SimCalorimeterHit::clear() {

  contribs_incident_id_.clear();
  contribs_track_id_.clear();
  contribs_pdg_id_.clear();
  contribs_edeps_.clear();
  contribs_time_.clear();

  contrib_count_ = 0;
  id_ = 0;
  edep_ = 0;
  x_ = 0;
  y_ = 0;
  z_ = 0;
  time_ = 0;
}

void SimCalorimeterHit::attach(fire::io::Data<SimCalorimeterHit> &d) { 
  d.attach("contrib_count", contrib_count_); 
  d.attach("id", id_);
  d.attach("edep", edep_); 
  d.attach("x", x_); 
  d.attach("y", y_); 
  d.attach("z", z_); 
  d.attach("time", time_);
  d.attach("contribs_incident_id", contribs_incident_id_); 
  d.attach("contribs_track_id", contribs_track_id_); 
  d.attach("contribs_pdg_id", contribs_pdg_id_); 
  d.attach("contribs_edeps", contribs_edeps_); 
  d.attach("contribs_time", contribs_time_); 
}

void SimCalorimeterHit::addContrib(const int &incident_id, const int &track_id,
                                   const int &pdg_id, const float &edep,
                                   const float &time) {
  contribs_incident_id_.push_back(incident_id);
  contribs_track_id_.push_back(track_id);
  contribs_pdg_id_.push_back(pdg_id);
  contribs_edeps_.push_back(edep);
  contribs_time_.push_back(time);

  edep_ += edep;

  if (time < time_ || time_ == 0) {
    time_ = time;
  }
  ++contrib_count_;
}

SimCalorimeterHit::Contrib SimCalorimeterHit::getContrib(const int &i) const {
  // TODO(OM): Should these be in a vector so they don't need to be created
  // everytime getContrib is called?  This will also simplify searching.
  Contrib contrib;
  contrib.incident_id = contribs_incident_id_.at(i);
  contrib.track_id = contribs_track_id_.at(i);
  contrib.edep = contribs_edeps_.at(i);
  contrib.time = contribs_time_.at(i);
  contrib.pdg_id = contribs_pdg_id_.at(i);

  return contrib;
}

int SimCalorimeterHit::findContribIndex(const int& track_id, const int& pdg_id) const {
  for (int jcontrib{0}; jcontrib < contrib_count_; ++jcontrib) {
    Contrib contrib{getContrib(jcontrib)};
    if (contrib.track_id == track_id && contrib.pdg_id == pdg_id) {
      return jcontrib; 
    }
  }
  return -1; 
}

void SimCalorimeterHit::updateContrib(const int& i, const float& edep, const float& time) {
  contribs_edeps_[i] += edep;
  if (time < contribs_time_.at(i)) {
    contribs_time_[i] = time;
  }
  edep_ += edep;
}

std::ostream &operator<<(std::ostream &output, const SimCalorimeterHit &hit) {
  output << "---[ SimCalorimeterHit ] { \n"
         << "\tID: " << hit.id() << "\n"
         << "\tEnergy Deposition (MeV): " << hit.edep() << "\n"
         << "\tPosition (mm): ( " << hit.position()[0] << ", "
         << hit.position()[1] << ", " << hit.position()[2] << " )\n"
         << "\tContribution count: " << hit.contribCount() << "\n"
         << "}" << std::endl;

  return output;
}
} // namespace g4fire::event
