#include "phrasalrule.h"

#include <optional>

namespace jhu::thrax {

namespace {

std::optional<PhrasalRule> addNonterminal(const PhrasalRule& r, const NT& nt) {
  if (!r.lhs.span.contains(nt.span)) {
    return {};
  }
  auto begin = r.nts.begin();
  auto it = begin + r.nextNT;
  if (it == r.nts.end()) {
    return {};
  }
  if (it > begin && nt.span.src.start < (it - 1)->span.src.start) {
    return {};
  }
  if (!std::all_of(
        begin, it, [&nt](auto i) { return disjoint(i.span, nt.span); })) {
    return {};
  }
  PhrasalRule result(r);
  result.nts[result.nextNT++] = nt;
  cutPoints(result.alignment, nt.span.src.start, nt.span.src.end);
  return std::move(result);
}

using Rules = std::vector<PhrasalRule>;

Rules addAllNonterminals(
    const Rules& rules, const std::vector<NT>& phrases) {
  Rules next;
  next.reserve(rules.size());
  for (const auto& rule : rules) {
    const auto& prev = rule.nextNT == 0 ? rule.lhs : rule.nts[rule.nextNT - 1];
    auto it = std::lower_bound(
        phrases.begin(), phrases.end(), prev, bySourceStart);
    for (; it < phrases.end(); ++it) {
      if (auto r = addNonterminal(rule, *it); r.has_value()) {
        next.push_back(*std::move(r));
      } else if (it->span.src.start >= rule.lhs.span.src.end) {
        break;
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
    const Labeler& labeler,
    const AlignedSentencePair& sentence,
    const std::vector<SpanPair>& initial) {
  for (auto p : initial) {
    // Generate everything here so we don't invalidate views.
    labeler(p);
  }
  std::array<Rules, kMaxNonterminals + 1> rules;
  std::vector<NT> nts(initial.size());
  std::transform(
      initial.begin(),
      initial.end(),
      nts.begin(),
      [&labeler](auto p) { return NT(p, labeler(p)); });
  std::sort(nts.begin(), nts.end(), bySourceStart);
  rules.front().reserve(nts.size());
  for (auto nt : nts) {
    rules.front().emplace_back(sentence, nt);
  }
  for (size_t i = 0; i < kMaxNonterminals; i++) {
    rules[i + 1] = addAllNonterminals(rules[i], nts);
  }
  return cat(std::move(rules));
}

}
