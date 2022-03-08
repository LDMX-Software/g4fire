#include "g4fire/PrimaryGenerator.h"

#include "g4fire/PluginFactory.h"

namespace g4fire {

PrimaryGenerator::PrimaryGenerator(const std::string &name,
                                   fire::config::Parameters &params) {
  name_ = name;
  params_ = params;
}

void PrimaryGenerator::declare(const std::string &name,
                               PrimaryGeneratorBuilder *builder) {
  PluginFactory::getInstance().registerGenerator(name, builder);
}
} // namespace g4fire
