#include "alignment.h"

#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <string>

namespace jhu::thrax {

Alignment readAlignment(std::string_view sv) {
  Alignment result(std::count(sv.begin(), sv.end(), '-'));
  auto s = sv.data();
  char* e;
  for (auto& p : result) {
    p.src = std::strtol(s, &e, 10);
    if (e == s) {
      throw std::invalid_argument(std::string(sv));
    }
    s = e;
    p.tgt = -std::strtol(s, &e, 10);
    if (e == s) {
      throw std::invalid_argument(std::string(sv));
    }
    s = e;
  }
  return result;
}

}
