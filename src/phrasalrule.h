#pragma once

#include <algorithm>
#include <array>
#include <functional>
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

  Alignment alignment() const {
    Alignment result(sentence.alignment);
    auto ss = terminalIndices<true>();
    auto ts = terminalIndices<false>();
    auto contains = [&ss](auto i) {
      return std::find(ss.begin(), ss.end(), i.src) != ss.end();
    };
    auto idx = [](const Indices& is, auto i) {
      auto it = std::find(is.begin(), is.end(), i);
      return static_cast<IndexType>(it - is.begin());
    };
    auto it = std::remove_if(
        result.begin(), result.end(), std::not_fn(contains));
    result.erase(it, result.end());
    for (auto& p : result) {
      p.src = idx(ss, p.src);
      p.tgt = idx(ts, p.tgt);
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
