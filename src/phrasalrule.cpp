#include "phrasalrule.h"

#include <optional>

namespace jhu::thrax {

std::optional<PhrasalRule> addNonterminal(PhrasalRule r, SpanPair nt) {
  if (!r.lhs.contains(nt)) {
    return {};
  }
  auto it = std::find_if(
      r.nts.begin(), r.nts.end(), [](auto i) { return i.empty(); });
  if (it == r.nts.end()) {
    return {};
  }
  if (!std::all_of(
        r.nts.begin(), r.nts.end(), [nt](auto i) {
          return i.empty() || disjoint(i, nt);
        })) {
    return {};
  }
  *it = nt;
  return std::make_optional<PhrasalRule>(std::move(r));
}

std::vector<PhrasalRule> extract(const AlignedSentencePair&) {
  std::vector<PhrasalRule> result;
  return result;
}

}
