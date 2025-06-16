#include "driver.h"

#include <fstream>

namespace stewkk::lab11 {

Driver::Driver(bool trace_scanning, bool trace_parsing)
    : trace_scanning_(trace_scanning), trace_parsing_(trace_parsing) {}

void Driver::Parse(const std::string &filename) {
  std::ifstream file{filename};
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file " + filename);
  }

  Scanner scanner{file, std::cout, &filename};
  scanner.set_debug(trace_scanning_);

  Parser parser{scanner, *this};
  parser.set_debug_level(trace_parsing_);

  parser.parse();
}

}  // namespace stewkk::lab11
