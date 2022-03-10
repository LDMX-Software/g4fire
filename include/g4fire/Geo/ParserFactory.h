#ifndef G4FIRE_GEO_PARSERFACTORY_H_
#define G4FIRE_GEO_PARSERFACTORY_H_

#include "g4fire/Geo/Parser.h"

#include <map>

#include "fire/config/Parameters.h"

namespace g4fire {
namespace geo {

class ParserFactory {
 public:
  /// Get the instance to this factory
  static ParserFactory &getInstance();

  /// Default constructor
  ~ParserFactory() = default;

  /**
   * Create an instance of the parser of the given type.
   *
   * @param[in] type String type of the perser that needs to be created.
   * @param[in] ci Interface to conditions system.
   */
  Parser *createParser(const std::string &name,
                       fire::config::Parameters &params,
                       g4fire::ConditionsInterface &ci);

  // Delete the following methods to make sure they are inaccesible.
  ParserFactory(ParserFactory const &) = delete;
  void operator=(ParserFactory const &) = delete;

 private:
  /// Default constructor
  ParserFactory();

  /**
   * Register the parser with this factory.
   *
   * This is used to map the name of the parser to the function used to create
   * it.
   *
   * @param[in] name Name of the parser being registered.
   * @param[in] crate Function used to create this function.
   */
  void registerParser(const std::string &name, createFunc create);

  // Mapping between a parser type and its create function
  std::map<std::string, createFunc> parser_map;
};
} // namespace geo
} // namespace g4fire

#endif // G4FIRE_GEO_PARSERFACTORY_H_
