#include "g4fire/darkbrem/DarkBremVertexLibraryModel.h"

#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <boost/numeric/odeint.hpp>

#include "fire/exception/Exception.h"
//#include "Framework/Logger.h"

#include "g4fire/darkbrem/G4APrime.h"

#include "G4Electron.hh"
#include "G4EventManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

namespace g4fire {
namespace darkbrem {

DarkBremVertexLibraryModel::DarkBremVertexLibraryModel(
    fire::config::Parameters &params)
    : G4eDarkBremsstrahlungModel(params), method_(DarkBremMethod::undefined) {
  method_name_ = params.get<std::string>("method");
  if (method_name_ == "forward_only") {
    method_ = DarkBremMethod::forward_only;
  } else if (method_name_ == "cm_scaling") {
    method_ = DarkBremMethod::cm_scaling;
  } else if (method_name_ == "undefined") {
    method_ = DarkBremMethod::undefined;
  } else {
    throw fire::Exception(
        "InvalidMethod",
        "Invalid dark brem simulation method '" + method_name_ + "'.", false);
  }

  threshold_ = std::max(
      params.get<double>("threshold"),
      2. * G4APrime::APrime()->GetPDGMass() / CLHEP::GeV // mass A' in GeV
  );

  epsilon_ = params.get<double>("epsilon");

  library_path_ = params.get<std::string>("library_path");
  SetMadGraphDataLibrary(library_path_);
}

void DarkBremVertexLibraryModel::PrintInfo() const {
  G4cout << " Dark Brem Vertex Library Model" << G4endl;
  G4cout << "   Threshold [GeV]: " << threshold_ << G4endl;
  G4cout << "   Epsilon:         " << epsilon_ << G4endl;
  G4cout << "   Scaling Method:  " << method_name_ << G4endl;
  G4cout << "   Vertex Library:  " << library_path_ << G4endl;
}

void DarkBremVertexLibraryModel::RecordConfig(fire::RunHeader &h) const {
  h.set<float>("Minimum Threshold to DB [GeV]", threshold_);
  h.set<float>("DB Xsec Epsilon", epsilon_);
  h.set<std::string>("Vertex Scaling Method", method_name_);
  h.set<std::string>("Vertex Library", library_path_);
}

G4double
DarkBremVertexLibraryModel::ComputeCrossSectionPerAtom(G4double electron_ke,
                                                       G4double A, G4double Z) {
  static const double MA =
      G4APrime::APrime()->GetPDGMass() / CLHEP::GeV; // mass A' in GeV
  static const double Mel =
      G4Electron::Electron()->GetPDGMass() / CLHEP::GeV; // mass electron in GeV

  if (electron_ke < keV)
    return 0.; // outside viable region for model

  electron_ke = electron_ke / CLHEP::GeV; // Change energy to GeV.

  // TODO move to first cut above
  if (electron_ke < threshold_)
    return 0.; // can't produce a prime

  // begin: chi-formfactor calculation
  Chi chiformfactor;
  //  set parameters
  chiformfactor.A = A;
  chiformfactor.Z = Z;
  chiformfactor.E0 = electron_ke;
  chiformfactor.MA = MA;
  chiformfactor.Mel = Mel;

  double tmin = MA * MA * MA * MA / (4. * electron_ke * electron_ke);
  double tmax = MA * MA;

  // Integrate over chi.
  StateType integral(1);
  integral[0] = 0.; // start integral value at zero
  boost::numeric::odeint::integrate(
      chiformfactor // how to calculate integrand
      ,
      integral // integral result
      ,
      tmin // integral lower limit
      ,
      tmax // integral upper limit
      ,
      (tmax - tmin) / 1000 // dt - initial, adapts based off error
  );

  G4double ChiRes = integral[0];

  // Integrate over x. Can use log approximation instead, which falls off at
  // high A' mass.
  DiffCross diffcross;
  diffcross.E0 = electron_ke;
  diffcross.MA = MA;
  diffcross.Mel = Mel;

  double xmin = 0;
  double xmax = 1;
  if ((Mel / electron_ke) > (MA / electron_ke))
    xmax = 1 - Mel / electron_ke;
  else
    xmax = 1 - MA / electron_ke;

  // Integrate over differential cross section.
  integral[0] = 0.; // start integral value at zero
  boost::numeric::odeint::integrate(
      diffcross // how to calculate integrand
      ,
      integral // integral result
      ,
      xmin // integral lower limit
      ,
      xmax // integral upper limit
      ,
      (xmax - xmin) / 1000 // dx - initial, adapts based off error
  );

  G4double DsDx = integral[0];

  G4double GeVtoPb = 3.894E08;
  G4double alphaEW = 1.0 / 137.0;

  G4double cross = GeVtoPb * 4. * alphaEW * alphaEW * alphaEW * epsilon_ *
                   epsilon_ * ChiRes * DsDx * CLHEP::picobarn;

  if (cross < 0.)
    return 0.; // safety check all the math

  return cross;
}

void DarkBremVertexLibraryModel::GenerateChange(
    G4ParticleChange &particle_change, const G4Track &track,
    const G4Step &step) {
  static const double MA =
      G4APrime::APrime()->GetPDGMass() / CLHEP::GeV; // mass A' in GeV
  static const double Mel =
      G4Electron::Electron()->GetPDGMass() / CLHEP::GeV; // mass electron in GeV

  G4double incident_energy = step.GetPostStepPoint()->GetTotalEnergy();
  incident_energy =
      incident_energy / CLHEP::GeV; // Convert the energy to GeV, the units used
  // in the LHE files.

  OutgoingKinematics data = GetMadgraphData(incident_energy);
  // The fourth element in Eigen::Vector4f is obtained using the method .w()
  // double EAcc = (data.electron.E() - Mel) *
  double EAcc = (data.electron.w() - Mel) *
                    ((incident_energy - Mel - MA) / (data.E - Mel - MA)) +
                Mel;
  // double pt = data.electron.Pt();
  double pt{Pt(data.electron)};
  double P = sqrt(EAcc * EAcc - Mel * Mel);
  // double PhiAcc = data.electron.Phi();
  double phi_acc{Phi(data.electron)};
  if (method_ == DarkBremMethod::forward_only) {
    unsigned int i = 0;
    while (pt * pt + Mel * Mel > EAcc * EAcc) {
      // Skip events until the transverse energy is less than the total energy.
      i++;
      data = GetMadgraphData(incident_energy);
      // EAcc = (data.electron.E() - Mel) *
      EAcc = (data.electron.w() - Mel) *
                 ((incident_energy - Mel - MA) / (data.E - Mel - MA)) +
             Mel;
      // pt = data.electron.Pt();
      pt = Pt(data.electron);
      P = sqrt(EAcc * EAcc - Mel * Mel);
      // PhiAcc = data.electron.Phi();
      phi_acc = Phi(data.electron);

      if (i > max_iterations_) {
        // ldmx_log(warn)
        //    << "Could not produce a realistic vertex with library energy "
        //    << data.electron.E() << " MeV.\n"
        //    << "Consider expanding your libary of A' vertices to include a "
        //       "beam energy closer to "
        //    << incident_energy << " MeV.";
        break;
      }
    }
  } else if (method_ == DarkBremMethod::cm_scaling) {

    // TLorentzVector el(data.electron.X(), data.electron.Y(),
    // data.electron.Z(),
    //                  data.electron.E());
    Eigen::Vector4f el(data.electron);
    double ediff = data.E - incident_energy;
    // TLorentzVector newcm(data.center_momentum.X(), data.center_momentum.Y(),
    //                     data.center_momentum.Z() - ediff,
    //                     data.center_momentum.E() - ediff);
    Eigen::Vector4f newcm(data.center_momentum.x(), data.center_momentum.y(),
                          data.center_momentum.z() - ediff,
                          data.center_momentum.w() - ediff);
    // el.Boost(-1. * data.center_momentum.BoostVector());
    el = Boost(el, -1 * BoostVector(data.center_momentum));
    // el.Boost(newcm.BoostVector());
    el = Boost(el, BoostVector(newcm));
    // double newE = (data.electron.E() - Mel) *
    double newE = (data.electron.w() - Mel) *
                      ((incident_energy - Mel - MA) / (data.E - Mel - MA)) +
                  Mel;
    // el.SetE(newE);
    el[3] = newE;
    // EAcc = el.E();
    EAcc = el.w();
    // pt = el.Pt();
    pt = Pt(el);
    // P = el.P();
    P = sqrt(el.x() * el.x() + el.y() * el.y() + el.z() * el.z());
  } else if (method_ == DarkBremMethod::undefined) {
    // EAcc = data.electron.E();
    EAcc = data.electron.w();
    P = sqrt(EAcc * EAcc - Mel * Mel);
    // pt = data.electron.Pt();
    pt = Pt(data.electron);
  }

  // What we need:
  //  - EAcc
  //  - P and Pt for theta_acc
  //  - phi_acc
  // Basically we need the 3-momentum of the recoil electron

  EAcc = EAcc *
         CLHEP::GeV; // Change the energy back to MeV, the internal GEANT unit.

  G4double recoil_e_p_mag =
      sqrt(EAcc * EAcc -
           electron_mass_c2 * electron_mass_c2); // Electron momentum in MeV.
  G4ThreeVector recoil_e_p;
  double theta_acc = std::asin(pt / P);
  recoil_e_p.set(std::sin(theta_acc) * std::cos(phi_acc),
                 std::sin(theta_acc) * std::sin(phi_acc), std::cos(theta_acc));
  recoil_e_p.rotateUz(track.GetMomentumDirection());
  recoil_e_p.setMag(recoil_e_p_mag);

  // create g4dynamicparticle object for the dark photon.
  // define its 3-momentum so we conserve 3-momentum with primary and recoil
  // electron NOTE: does _not_ take nucleus recoil into account
  G4ThreeVector dp_p = track.GetMomentum() - recoil_e_p;
  G4DynamicParticle *dphoton = new G4DynamicParticle(G4APrime::APrime(), dp_p);
  // energy of primary
  G4double final_ke = EAcc - electron_mass_c2;

  // stop tracking and create new secondary instead of primary
  if (always_create_new_electron_) {
    // TODO copy over all other particle information from track I am killing
    G4DynamicParticle *el = new G4DynamicParticle(
        track.GetDefinition(), // should be all electrons right now, but leaves
        // positrons open
        recoil_e_p);
    particle_change.SetNumberOfSecondaries(2);
    particle_change.AddSecondary(dphoton);
    particle_change.AddSecondary(el);
    particle_change.ProposeTrackStatus(fStopAndKill);
    // continue tracking
  } else {
    // just have primary lose energy (don't rename to different track ID)
    // TODO untested this branch, not sure if it works as expected
    particle_change.SetNumberOfSecondaries(1);
    particle_change.AddSecondary(dphoton);
    particle_change.ProposeMomentumDirection(recoil_e_p.unit());
    particle_change.ProposeEnergy(final_ke);
  }
}

void DarkBremVertexLibraryModel::SetMadGraphDataLibrary(std::string path) {
  // Assumptions:
  //  - Directory passed is a flat directory (no sub directories) containing LHE
  //  files
  //  - LHE files are events generated with the correct mass point
  // TODO automatically select LHE files of the correct mass point?

  bool found_one_file = false;
  DIR *dir;           // handle to opened directory
  struct dirent *ent; // handle to entry inside directory
  if ((dir = opendir(path.c_str())) != NULL) {
    // directory can be opened
    while ((ent = readdir(dir)) != NULL) {
      std::string fp = path + '/' + std::string(ent->d_name);
      if (fp.substr(fp.find_last_of('.') + 1) == "lhe") {
        // file ends in '.lhe'
        ParseLHE(fp);
        found_one_file = true;
      }
    }
    closedir(dir);
  }

  if (!found_one_file) {
    throw fire::Exception("DirDNE",
                          "Directory '" + path +
                              "' was unable to be opened or no '.lhe' "
                              "files were found inside of it.",
                          false);
  }

  MakePlaceholders(); // Setup the placeholder offsets for getting data.

  // ldmx_log(info) << "MadGraph Library of Dark Brem Vertices:\n";
  for (const auto &kV : mad_graph_data_) {
    // ldmx_log(info) << "\t" << std::setw(8) << kV.first << " GeV Beam -> "
    //               << std::setw(6) << kV.second.size() << " Events";
  }

  return;
}

void DarkBremVertexLibraryModel::Chi::operator()(const StateType &,
                                                 StateType &dxdt, double t) {
  G4double MUp = 2.79;  // mass up quark [GeV]
  G4double Mpr = 0.938; // mass proton [GeV]

  G4double d = 0.164 / pow(A, 2. / 3.);
  G4double ap = 773.0 / (Mel * pow(Z, 2. / 3.));
  G4double a = 111.0 / (Mel * pow(Z, 1. / 3.));
  G4double G2el = pow(Z, 2) * pow(a, 4) * pow(t, 2) /
                  (pow(1.0 + a * a * t, 2) * pow(1.0 + t / d, 2));
  G4double G2in = Z * pow(ap, 4) * pow(t, 2) /
                  (pow(1.0 + ap * ap * t, 2) * pow(1.0 + t / 0.71, 8)) *
                  pow(1.0 + t * (pow(MUp, 2) - 1.0) / (4.0 * pow(Mpr, 2)), 2);
  G4double G2 = G2el + G2in;
  G4double ttmin = MA * MA * MA * MA / 4.0 / E0 / E0;
  G4double Under = G2 * (t - ttmin) / t / t;

  dxdt[0] = Under;

  return;
}

void DarkBremVertexLibraryModel::DiffCross::operator()(const StateType &,
                                                       StateType &dsigma_dx,
                                                       double x) {
  G4double beta = sqrt(1 - MA * MA / E0 / E0);
  G4double num = 1. - x + x * x / 3.;
  G4double denom = MA * MA * (1. - x) / x + Mel * Mel * x;

  dsigma_dx[0] = beta * num / denom;

  return;
}

void DarkBremVertexLibraryModel::ParseLHE(std::string fname) {
  static const double MA =
      G4APrime::APrime()->GetPDGMass() / CLHEP::GeV; // mass A' in GeV

  // TODO: use already written LHE parser?
  // ldmx_log(info) << "Parsing LHE file '" << fname << "'... ";

  std::ifstream ifile;
  ifile.open(fname.c_str());
  if (!ifile) {
    throw fire::Exception("LHEFile", "Unable to open LHE file '" + fname + "'.",
                          false);
  }

  std::string line;
  while (std::getline(ifile, line)) {
    std::istringstream iss(line);
    int ptype, state;
    double skip, px, py, pz, E, M;
    if (iss >> ptype >> state >> skip >> skip >> skip >> skip >> px >> py >>
        pz >> E >> M) {
      if ((ptype == 11) && (state == -1)) {
        double ebeam = E;
        double e_px, e_py, e_pz, a_px, a_py, a_pz, e_E, a_E, e_M, a_M;
        for (int i = 0; i < 2; i++) {
          std::getline(ifile, line);
        }
        std::istringstream jss(line);
        jss >> ptype >> state >> skip >> skip >> skip >> skip >> e_px >> e_py >>
            e_pz >> e_E >> e_M;
        if ((ptype == 11) && (state == 1)) { // Find a final state electron.
          for (int i = 0; i < 2; i++) {
            std::getline(ifile, line);
          }
          std::istringstream kss(line);
          kss >> ptype >> state >> skip >> skip >> skip >> skip >> a_px >>
              a_py >> a_pz >> a_E >> a_M;
          if (ptype == 622 and state == 1) {
            if (abs(1. - a_M / MA) > 1e-3) {
              throw fire::Exception("BadMGEvnt",
                              "A MadGraph imported event has a different "
                              "APrime mass than the model has (MadGraph = " +
                                  std::to_string(a_M) + "GeV; Model = " +
                                  std::to_string(MA) + "GeV).",
                              false);
            }
            OutgoingKinematics evnt;
            double cmpx = a_px + e_px;
            double cmpy = a_py + e_py;
            double cmpz = a_pz + e_pz;
            double cmE = a_E + e_E;
            evnt.electron << e_px, e_py, e_pz, e_E;
            // evnt.electron = TLorentzVector(e_px, e_py, e_pz, e_E);
            // evnt.center_momentum = TLorentzVector(cmpx, cmpy, cmpz, cmE);
            evnt.center_momentum << cmpx, cmpy, cmpz, cmE;
            evnt.E = ebeam;
            mad_graph_data_[ebeam].push_back(evnt);
          } // get a prime kinematics
        }   // check for final state
      }     // check for particle type and state
    }       // able to get momentum/energy numbers
  }         // while getting lines
  // Add the energy to the list, with a random offset between 0 and the total
  // number of entries.
  ifile.close();
  // ldmx_log(info) << "done parsing.";
}

void DarkBremVertexLibraryModel::MakePlaceholders() {
  current_data_points_.clear();
  max_iterations_ = 10000;
  for (const auto &iter : mad_graph_data_) {
    current_data_points_[iter.first] =
        int(G4UniformRand() * iter.second.size());
    if (iter.second.size() < max_iterations_)
      max_iterations_ = iter.second.size();
  }
}

DarkBremVertexLibraryModel::OutgoingKinematics
DarkBremVertexLibraryModel::GetMadgraphData(double E0) {
  OutgoingKinematics cmdata; // data frame to return

  // Cycle through imported beam energies until the closest one above is found,
  // or the max is reached.
  double samplingE = 0.;
  for (const auto &keyVal : current_data_points_) {
    samplingE = keyVal.first; // move samplingE up
    // check if went under the sampling energy
    //  the map is sorted by key, so we can be done right after E0 goes under
    //  samplingE
    if (E0 < samplingE)
      break;
  }
  // now samplingE is the closest energy above E0 or the maximum energy imported
  // from mad graph

  // Need to loop around if we hit the end, when the size of
  // mad_graph_data_[samplingE] is smaller than
  //  the number of events we want
  if (current_data_points_.at(samplingE) >=
      mad_graph_data_.at(samplingE).size()) {
    current_data_points_[samplingE] = 0;
  }

  // Get the lorentz vectors from the index given by the placeholder.
  cmdata = mad_graph_data_.at(samplingE).at(current_data_points_.at(samplingE));

  // Increment the current index
  current_data_points_[samplingE]++;

  return cmdata;
}

} // namespace darkbrem
} // namespace g4fire
