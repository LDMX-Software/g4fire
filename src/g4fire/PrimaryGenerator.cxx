/**
 * @file PrimaryGenerator.cxx
 * @brief Implementation file for PrimaryGenerator
 *
 * @author Tom Eichlersmith, University of Minnesota
 */

#include "g4fire/PrimaryGenerator.h"

#include "g4fire/PluginFactory.h"

namespace simcore {

PrimaryGenerator::PrimaryGenerator(const std::string& name,
                                   framework::config::Parameters& parameters) {
  name_ = name;
  parameters_ = parameters;
}

PrimaryGenerator::~PrimaryGenerator() {}

void PrimaryGenerator::declare(const std::string& className,
                               PrimaryGeneratorBuilder* builder) {
  PluginFactory::getInstance().registerGenerator(className, builder);
}
}  // namespace simcore
