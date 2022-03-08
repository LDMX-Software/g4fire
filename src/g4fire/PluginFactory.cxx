#include "g4fire/PluginFactory.h"

#include <dlfcn.h>

#include <algorithm>
#include <string>
#include <vector>

//#include "Framework/Exception/Exception.h"

namespace g4fire {

PluginFactory &PluginFactory::getInstance() {
  // the_factory is created on first call to getInstance
  //  and is guaranteed to be destroyed
  static PluginFactory the_factory;
  return the_factory;
}

void PluginFactory::registerGenerator(const std::string &class_name,
                                      PrimaryGeneratorBuilder *builder) {
  auto it{registered_generators_.find(class_name)};
  if (it != registered_generators_.end()) {
    // EXCEPTION_RAISE(
    //    "ExistingGeneratorDefinition",
    //    "The primary generator " + class_name + " has already been
    //    registered.");
  }

  registered_generators_[class_name] = builder;
}

void PluginFactory::createGenerator(const std::string &class_name,
                                    const std::string &instance_name,
                                    fire::config::Parameters &params) {
  auto it{registered_generators_.find(class_name)};
  if (it == registered_generators_.end()) {
    // EXCEPTION_RAISE("CreateGenerator",
    //                "Failed to create generator '" + class_name + "'.");
  }

  auto generator{it->second(instance_name, params)};

  // now that the generator is built --> put it on active list
  generators_.push_back(generator);

  // ldmx_log(info) << "Primary Generator '" << instance_name << "' of class '"
  //               << class_name << "' has been created.";
}

actionMap PluginFactory::getActions() {
  if (actions_.empty()) {
    actions_[TYPE::RUN] = new UserRunAction();
    actions_[TYPE::EVENT] = new UserEventAction();
    actions_[TYPE::TRACKING] = new UserTrackingAction();
    actions_[TYPE::STEPPING] = new USteppingAction();
    actions_[TYPE::STACKING] = new UserStackingAction();
  }

  return actions_;
}

void PluginFactory::registerAction(const std::string &class_name,
                                   UserActionBuilder *builder) {
  auto it{registered_actions_.find(class_name)};
  if (it != registered_actions_.end()) {
    // EXCEPTION_RAISE(
    //    "ExistingActionDefinition",
    //    "The user action " + class_name + " has already been registered.");
  }

  registered_actions_[class_name] = builder;
}

void PluginFactory::createAction(const std::string &class_name,
                                 const std::string &instance_name,
                                 fire::config::Parameters &params) {
  auto it{registered_actions_.find(class_name)};
  if (it == registered_actions_.end()) {
    // EXCEPTION_RAISE("PluginFactory", "Failed to create " + class_name);
  }

  auto act{it->second(instance_name, params)};

  std::vector<TYPE> types = act->getTypes();
  for (auto &type : types) {
    if (type == TYPE::RUN)
      std::get<UserRunAction *>(actions_[TYPE::RUN])->registerAction(act);
    else if (type == TYPE::EVENT)
      std::get<UserEventAction *>(actions_[TYPE::EVENT])->registerAction(act);
    else if (type == TYPE::TRACKING)
      std::get<UserTrackingAction *>(actions_[TYPE::TRACKING])
          ->registerAction(act);
    else if (type == TYPE::STEPPING)
      std::get<USteppingAction *>(actions_[TYPE::STEPPING])
          ->registerAction(act);
    else if (type == TYPE::STACKING)
      std::get<UserStackingAction *>(actions_[TYPE::STACKING])
          ->registerAction(act);
    else
    // EXCEPTION_RAISE("PluginFactory", "User action type doesn't exist.");
  }

  // now that the biasing is built --> put it on active list
  // ldmx_log(info) << "User Action '" << instance_name << "' of class '"
  //               << class_name << "' has been created.";
}

void PluginFactory::registerBiasingOperator(
    const std::string &class_name, XsecBiasingOperatorBuilder *builder) {
  auto it{registered_operators_.find(class_name)};
  if (it != registered_operators_.end()) {
    // EXCEPTION_RAISE(
    //    "ExistingOperatorDefinition",
    //    "The biasing operator " + class_name + " has already been
    //    registered.");
  }

  registered_operators_[class_name] = builder;
}

void PluginFactory::createBiasingOperator(const std::string &class_name,
                                          const std::string &instance_name,
                                          fire::config::Parameters &params) {
  auto it{registered_operators_.find(class_name)};
  if (it == registered_operators_.end()) {
    // EXCEPTION_RAISE("CreateBiasingOperator",
    //                "Failed to create biasing '" + class_name + "'.");
  }

  auto bop{it->second(instance_name, params)};

  // now that the biasing is built --> put it on active list
  biasing_operators_.push_back(bop);
  // ldmx_log(info) << "Biasing operator '" << instance_name << "' of class '"
  //               << class_name << "' has been created.";
}

} // namespace g4fire
