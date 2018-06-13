#include "alignment.h"

#include <algorithm>
#include <cstdlib>

namespace jhu::thrax {

Alignment readAlignment(std::string_view sv) {
  Alignment result(std::count(sv.begin(), sv.end(), '-'));
  auto s = sv.data();
  char* e;
  for (auto& p : result) {
    p.src = std::strtol(s, &e, 10);
    s = e;
    p.tgt = -std::strtol(s, &e, 10);
    s = e;
  }
  return result;
}

}
