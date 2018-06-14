#include "phrasalrule.h"

#include <optional>

namespace jhu::thrax {

namespace {

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

using Rules = std::vector<PhrasalRule>;

Rules addAllNonterminals(
    const Rules& rules, const std::vector<SpanPair>& phrases) {
  Rules next;
  next.reserve(rules.size());
  for (auto p : phrases) {
    for (auto rule : rules) {
      if (auto r = addNonterminal(rule, p); r.has_value()) {
        next.push_back(std::move(*r));
      }
    }
  }
  return next;
}

template<typename T, size_t N>
std::vector<T> cat(std::array<std::vector<T>, N>&& vals) {
  auto sz = std::accumulate(
      vals.begin(),
      vals.end(),
      size_t{0},
      [](size_t total, const auto& v) { return total + v.size(); });
  std::vector<T> result;
  result.reserve(sz);
  for (auto& v : vals) {
    std::move(v.begin(), v.end(), std::back_inserter(result));
  }
  return result;
}

}

std::vector<PhrasalRule> extract(
    const AlignedSentencePair& sentence, int initialPhraseSize) {
  auto initial = minimalConsistentPairs(sentence.alignment, initialPhraseSize);
  std::array<Rules, kMaxNonterminals + 1> rules;
  rules.front().reserve(initial.size());
  for (auto p : initial) {
    rules.front().emplace_back(sentence, p);
  }
  for (size_t i = 0; i < kMaxNonterminals; i++) {
    rules[i + 1] = addAllNonterminals(rules[i], initial);
  }
  return cat(std::move(rules));
}

}
