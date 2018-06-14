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
  std::sort(result.begin(), result.end(), bySource);
  return result;
}

void printAlignment(std::ostream& out, const Alignment& alignment) {
  for (size_t i = 0; i < alignment.size(); i++) {
    if (i > 0) {
      out << ' ';
    }
    out << alignment[i];
  }
}

}
