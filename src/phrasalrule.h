#pragma once

#include <algorithm>
#include <array>
#include <ostream>
#include <string_view>

#include "label.h"
#include "phrase.h"
#include "sentence.h"

namespace jhu::thrax {

constexpr size_t kMaxNonterminals = 4;
using Rhs = std::array<SpanPair, kMaxNonterminals>;

struct PhrasalRule {
  Rhs nts{};
  SpanPair lhs{};
  const AlignedSentencePair& sentence;

  explicit PhrasalRule(const AlignedSentencePair& asp)
      : PhrasalRule(asp, asp.span()) {}
  explicit PhrasalRule(const AlignedSentencePair& asp, SpanPair root)
      : lhs(root), sentence(asp) {}

  template<bool SourceSide>
  auto terminalIndices() const {
    Span lhsSide;
    if constexpr (SourceSide) {
      lhsSide = lhs.src;
    } else {
      lhsSide = lhs.tgt;
    }
    auto result = lhsSide.indices();
    for (auto nt : nts) {
      if constexpr (SourceSide) {
        removeIndices(result, nt.src);
      } else {
        removeIndices(result, nt.tgt);
      }
    }
    return result;
  }
};

struct LabeledRuleView {
  const PhrasalRule& rule;
  const Labeler& labeler;
};

constexpr int kLhsIndex = 0;
inline void bracket(std::ostream& out, std::string_view nt, int index) {
  out << '[' << nt;
  if (index != kLhsIndex) {
    out << ',' << index;
  }
  out << ']';
}

template<bool SourceSide>
inline void printRhs(std::ostream& out, LabeledRuleView v) {
  const auto& [rule, labeler] = v;
  const auto& s = rule.sentence;
  auto nts = rule.nts;
  std::sort(
      nts.begin(),
      nts.end(),
      [](auto a, auto b) {
        return a.template get<SourceSide>().start
             < b.template get<SourceSide>().start;
      });
  auto nt = nts.begin();
  int ntIndex = 1;
  for (auto i : rule.terminalIndices<SourceSide>()) {
    out << ' ';
    if (nt == nts.end()
        || nt->empty()
        || i >= nt->template get<SourceSide>().start) {
      if constexpr (SourceSide) {
        out << s.src[i];
      } else {
        out << s.tgt[i];
      }
    } else {
      // next NT
      bracket(out, labeler(s, *nt), ntIndex);
      ntIndex++;
      nt++;
    }
  }
}

inline std::ostream& operator<<(std::ostream& out, LabeledRuleView v) {
  const auto& [rule, labeler] = v;
  const auto& s = rule.sentence;
  const static std::string_view kSep = " |||";
  bracket(out, labeler(s, rule.lhs), kLhsIndex);
  out << kSep;
  printRhs<true>(out, v);
  out << kSep;
  printRhs<false>(out, v);
  return out;
}

}
