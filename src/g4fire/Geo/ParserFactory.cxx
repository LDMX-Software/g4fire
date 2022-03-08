
#include "g4fire/Geo/ParserFactory.h"

//#include "Framework/Exception/Exception.h"

#include "g4fire/Geo/GDMLParser.h"
#include "g4fire/Geo/Parser.h"

namespace g4fire {
namespace geo {

ParserFactory &ParserFactory::getInstance() {
  static ParserFactory instance;
  return instance;
}

ParserFactory::ParserFactory() { registerParser("gdml", &GDMLParser::create); }

void ParserFactory::registerParser(const std::string &name, createFunc create) {
  parser_map[name] = create;
}

Parser *ParserFactory::createParser(const std::string &name,
                                    framework::config::Parameters &parameters,
                                    g4fire::ConditionsInterface &ci) {
  auto it{parser_map.find(name)};
  //if (it == parser_map.end())
  //EXCEPTION_RAISE("ParserNotFound", "The parser " + name + " was not found.");

  return it->second(parameters, ci);
}

}  // namespace geo
}  // namespace g4fire
