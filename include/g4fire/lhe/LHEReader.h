#pragma once 

#include "g4fire/LHEEvent.h"

#include <fstream>

namespace g4fire {

/**
 * @brief Reads LHE event data into an LHEEvent object
 */
class LHEReader {
 public:
  /**
   * Class constructor.
   * @param file_name The input file name.
   */
  LHEReader(std::string& fileName);

  /**
   * Class destructor.
   */
  virtual ~LHEReader();

  /**
   * Read the next event.
   * @return The next LHE event.
   */
  LHEEvent* readNextEvent();

 private:
  /**
   * The input file stream.
   */
  std::ifstream ifs_;
};
}  // namespace g4fire
