#include <G4Event.hh>
#include <G4UImanager.hh>
#include <G4GeneralParticleSource.hh>

#include <g4fire/user/PrimaryGenerator.h>

namespace g4fire::plugins::primary_generators {

/**
 * @brief Class that extends the functionality of G4GeneralParticleSource.
 */
class GeneralParticleSource : public user::PrimaryGenerator {
 public:
  /**
   * Constructor.
   *
   * @param params Parameters used to configure the particle gun.
   *
   * Parameters:
   *  initCommands : vector of Geant4 strings to initialize the GPS
   */
  GeneralParticleSource(const fire::config::Parameters& params);

  /// Destructor
  ~GeneralParticleSource();

  /**
   * Generate the primary vertices in the Geant4 event.
   *
   * @param event The Geant4 event.
   */
  void GeneratePrimaryVertex(G4Event* event) final override;

  virtual void RecordConfig(fire::RunHeader& rh) const final override;

 private:
  /**
   * The underlying Geant4 GPS implementation.
   *
   * The creation of this class creates a new messenger that we can pass
   * commands to.
   */
  G4GeneralParticleSource g4_source_;

  std::vector<std::string> init_cmds_;

};  // GeneralParticleSource

GeneralParticleSource::GeneralParticleSource(
    const fire::config::Parameters& params)
    : PrimaryGenerator(params) {
  init_cmds_ = params.get<std::vector<std::string>>("init_cmds",{});
  for (const auto& cmd : init_cmds_) {
    int g4Ret = G4UImanager::GetUIpointer()->ApplyCommand(cmd);
    if (g4Ret > 0) {
      throw fire::Exception("InitCmd",
                      "Initialization command '" + cmd +
                          "' returned a failue status from Geant4: " +
                          std::to_string(g4Ret), false);
    }
  }
}

GeneralParticleSource::~GeneralParticleSource() {}

void GeneralParticleSource::GeneratePrimaryVertex(G4Event* event) {
  // just pass to the Geant4 implementation
  g4_source_.GeneratePrimaryVertex(event);

  return;
}

void GeneralParticleSource::RecordConfig(fire::RunHeader& rh) const {
  for (std::size_t i{0}; i < init_cmds_.size(); i++) {
    rh.set<std::string>("GPS Init "+std::to_string(i), init_cmds_.at(i));
  }
}

}  // namespace g4fire

DECLARE_GENERATOR(g4fire::plugins::primary_generators::GeneralParticleSource)
