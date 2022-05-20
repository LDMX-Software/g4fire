
#include "g4fire/Geo/ParserFactory.h"

#include "fire/exception/Exception.h"

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
                                    fire::config::Parameters &params,
                                    g4fire::ConditionsInterface &ci) {
  auto it{parser_map.find(name)};
  if (it == parser_map.end())
    throw fire::Exception("ParserNotFound",
                          "The parser " + name + " was not found.", false);

  return it->second(params, ci);
}

} // namespace geo
} // namespace g4fire
