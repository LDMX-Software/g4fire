#include "g4fire/lhe/LHEEvent.h"

#include "fire/exception/Exception.h"

#include "globals.hh"

#include <iostream>
#include <sstream>

namespace g4fire {

void LHEEvent::load(std::istream& is) {
  static std::string line;
  while(is and getline(is, line)) {
    if (line == "<event>") break;
  }
  if (!is) return;

  // next line after '<event>' tag is event information
  //    use 'getline' to avoid bleading into other line
  getline(is, line);
  std::istringstream iss(line);
  iss >> nup_ >> idprup_ >> xwgtup_ >> scalup_ >> aqedup_ >> aqcdup_;
  vtxt_ = 0.;
  vtx_[0] = 0.;
  vtx_[1] = 0.;
  vtx_[2] = 0.;

  while (getline(is, line)) {
    if (line == "</event>") break;

    if (line.find("#") == std::string::npos) {
      // not a comment, create a particle
      particles_.emplace_back(line);
    } else if (line.find("#vertex") != std::string::npos) {
      // imbedded vertex information added after the fact
      std::istringstream iss(line);
      std::vector<std::string> tokens;
      do {
        std::string elem;
        iss >> elem;
        if (elem.size() != 0) {
          tokens.push_back(elem);
        }
      } while (iss);
    
      if (tokens.size() != 4 && tokens.size() != 5) {
        throw fire::Exception(
            "TokenNum",
            "Wrong number of tokens or format in LHE event vertex "
            "information record.",
            false);
      }
      vtx_[0] = atof(tokens[1].c_str());
      vtx_[1] = atof(tokens[2].c_str());
      vtx_[2] = atof(tokens[3].c_str());
      if (tokens.size() > 4) {
        vtxt_ = atof(tokens[4].c_str());
      }
    }
  }

  for (auto& particle : particles_) {
    int m0{particle.getMOTHUP(0)}, m1{particle.getMOTHUP(1)};
    if (m0 > 0) particle.setMother(0, &particles_[m0-1]);
    if (m1 > 0) particle.setMother(1, &particles_[m1-1]);
  }
}

int LHEEvent::getNUP() const { return nup_; }

int LHEEvent::getIDPRUP() const { return idprup_; }

double LHEEvent::getXWGTUP() const { return xwgtup_; }

double LHEEvent::getSCALUP() const { return scalup_; }

double LHEEvent::getAQEDUP() const { return aqedup_; }

double LHEEvent::getAQCDUP() const { return aqcdup_; }

const double *LHEEvent::getVertex() const { return vtx_; }

const double LHEEvent::getVertexTime() const { return vtxt_; }

const std::vector<LHEParticle> &LHEEvent::getParticles() {
  return particles_;
}

} // namespace g4fire
