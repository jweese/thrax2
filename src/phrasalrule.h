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

  auto ntsInSourceOrder() const {
    Rhs result(nts);
    std::sort(
        result.begin(),
        result.end(),
        [](auto a, auto b) { return a.src.start < b.src.start; });
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
  constexpr auto empty = [](auto nt) { return nt.empty(); };
  auto nts = rule.nts;
  auto sourceOrder = rule.ntsInSourceOrder();
  std::remove_if(sourceOrder.begin(), sourceOrder.end(), empty);
  auto it = std::remove_if(nts.begin(), nts.end(), empty);
  std::sort(
      nts.begin(),
      it,
      [](auto a, auto b) {
        return a.template get<SourceSide>().start
             < b.template get<SourceSide>().start;
      });
  auto nt = nts.begin();
  for (auto i : rule.lhs.get<SourceSide>().indices()) {
    out << ' ';
    if (nt == it
        || nt->empty()
        || i < nt->template get<SourceSide>().start) {
      if constexpr (SourceSide) {
        out << s.src[i];
      } else {
        out << s.tgt[i];
      }
    } else {
      // next NT
      auto iter = std::find(sourceOrder.begin(), sourceOrder.end(), *nt);
      auto idx = static_cast<int>(1 + std::distance(sourceOrder.begin(), iter));
      bracket(out, labeler(s, *nt), idx);
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
