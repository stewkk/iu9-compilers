#include <cstdint>
#include <iostream>

#include "driver.h"
#include "formatter.h"

std::int32_t main(std::int32_t _, char* argv[]) {
  bool trace_parsing = true;
  bool trace_scanning = true;
  stewkk::lab11::Driver driver{trace_scanning, trace_parsing};
  driver.Parse(argv[1]);

  std::cout << stewkk::lab11::Formatter(driver.get_ident_table())
                   .Format(driver.get_program());
}
