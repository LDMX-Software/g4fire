#include "g4fire/PrimaryGeneratorAction.h"

#include "G4Event.hh"
#include "G4RunManager.hh"  // Needed for CLHEP

#include "g4fire/PluginFactory.h"
#include "g4fire/UserEventInformation.h"
#include "g4fire/UserPrimaryParticleInformation.h"

#include "fire/exception/Exception.h" 

namespace g4fire {

PrimaryGeneratorAction::PrimaryGeneratorAction(
    fire::config::Parameters& params)
    : G4VUserPrimaryGeneratorAction(), manager_(PluginFactory::getInstance()) {
  // The parameters used to configure the primary generator action
  params_ = params;

  // Check whether a beamspot should be used or not.
  // TODO(OM): Change individual beam spot variables to a vector
  auto beam_spot_{
      params.get<std::vector<double> >("beam_spot_delta", {})};
  if (!beam_spot_.empty()) {
    smear_beamspot_ = true;
    beamspot_delta_x_ = beam_spot_[0];
    beamspot_delta_y_ = beam_spot_[1];
    beamspot_delta_z_ = beam_spot_[2];
  }

  time_shift_primaries_ = params.get<bool>("time_shift_primaries", true);

  auto generators{
      params_.get<std::vector<fire::config::Parameters> >(
          "generators", {})};
  if (generators.empty()) {
    throw fire::Exception("MissingGenerator",
                    "Need to define some generator of primaries.", false);
  }

  for (auto& generator : generators) {
    manager_.createGenerator(
        generator.get<std::string>("class_name"),
        generator.get<std::string>("instance_name"), generator);
  }
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
  /*
   * Create our Event information first so that it
   * can be accessed by everyone from now on.
   */
  // Make sure we aren't overwriting a different information container
  if (event->GetUserInformation()) {
    throw fire::Exception(
        "Misconfig",
        "There was a UserEventInformation attached before beginning event."
        "\nI don't know how this happend!!", false);
  }

  // Make our information container and give it to geant4
  //    G4Event owns the event information and will delete it
  auto event_info{new UserEventInformation};
  event->SetUserInformation(event_info);

  /// Get the list of generators that will be used for this event
  auto generators{manager_.getGenerators()};

  // Generate the primary vertices using the generators
  std::for_each(generators.begin(), generators.end(),
                [event](const auto& generator) {
                  generator->GeneratePrimaryVertex(event);
                });

  // smear all primary vertices (if activated)
  int pv_count{event->GetNumberOfPrimaryVertex()};
  if (pv_count > 0) {
    // loop over all vertices generated
    for (int iPV = 0; iPV < pv_count; ++iPV) {
      G4PrimaryVertex* primary_vertex = event->GetPrimaryVertex(iPV);

      // Loop over all particle associated with the primary vertex and
      // set the generator status to 1.
      for (int iparticle = 0; iparticle < primary_vertex->GetNumberOfParticle();
           ++iparticle) {
        G4PrimaryParticle* primary = primary_vertex->GetPrimary(iparticle);

        auto primary_info{dynamic_cast<UserPrimaryParticleInformation*>(
            primary->GetUserInformation())};
        if (not primary_info) {
          // no user info defined
          //  ==> make a new one
          primary_info = new UserPrimaryParticleInformation;
          primary->SetUserInformation(primary_info);
        }  // check if primaryinfo is defined

        int hepStatus = primary_info->getHepEvtStatus();
        if (hepStatus <= 0) {
          // undefined hepStatus ==> set to 1
          primary_info->setHepEvtStatus(1);
        }  // check if hepStatus defined

      }  // iparticle - loop over primary particles from this vertex

      // include the weight of this primary vertex in the event weight
      event_info->incWeight(primary_vertex->GetWeight());

      // smear beamspot if it is turned on
      if (smear_beamspot_) {
        double x0_i = primary_vertex->GetX0();
        double y0_i = primary_vertex->GetY0();
        double z0_i = primary_vertex->GetZ0();
        /*
         * G4UniformRand returns a number in [0,1]
         *  - we shift this range so that it is [-0.5,0.5]
         *  - multiply by the width to get [-0.5*size,0.5*size]
         *  - add the initial point (in case its off center) to get
         *    [init-0.5*size, init+0.5*size]
         */
        double x0_f = beamspot_delta_x_ * (G4UniformRand() - 0.5) + x0_i;
        double y0_f = beamspot_delta_y_ * (G4UniformRand() - 0.5) + y0_i;
        double z0_f = beamspot_delta_z_ * (G4UniformRand() - 0.5) + z0_i;
        primary_vertex->SetPosition(x0_f, y0_f, z0_f);
      }

      // shift so that t=0 coincides with primaries arriving at (or coming from)
      // the target
      if (time_shift_primaries_) {
        primary_vertex->SetT0(primary_vertex->GetT0() +
                              primary_vertex->GetZ0() / 299.702547);
      }

    }  // iPV - loop over primary vertices
  } else {
    throw fire::Exception(
        "NoPrimaries",
        "No primary vertices were produced by any of the generators.", false);
  }
}
}  // namespace g4fire
