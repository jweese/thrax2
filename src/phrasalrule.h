#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <ostream>
#include <string_view>
#include <vector>

#include "label.h"
#include "phrase.h"
#include "sentence.h"

namespace jhu::thrax {

constexpr size_t kMaxNonterminals = 4;
using Rhs = std::array<SpanPair, kMaxNonterminals>;

struct PhrasalRule {
  Rhs nts{};
  int nextNT = 0;
  SpanPair lhs{};
  const AlignedSentencePair& sentence;
  Alignment alignment;

  explicit PhrasalRule(const AlignedSentencePair& asp)
      : PhrasalRule(asp, asp.span()) {}
  explicit PhrasalRule(const AlignedSentencePair& asp, SpanPair root)
      : lhs(root),
        sentence(asp),
        alignment(copyPoints(sentence.alignment, lhs.src.start, lhs.src.end)) {}

  int ntIndex(SpanPair nt) const {
    auto it = std::find(nts.begin(), nts.end(), nt);
    return static_cast<int>(1 + std::distance(nts.begin(), it));
  }


  auto terminalAlignment() const {
    Alignment result(alignment);
    for (auto& point : result) {
      point.src = terminalIndex<true>(point.src);
      point.tgt = terminalIndex<true>(point.tgt);
    }
    return result;
  }

 private:
  template<bool SourceSide>
  PointType terminalIndex(PointType i) const {
    auto result = i;
    if constexpr (SourceSide) {
      result -= lhs.src.start;
    } else {
      result -= lhs.tgt.start;
    }
    for (auto nt : nts) {
      if constexpr (SourceSide) {
        if (nt.src.start >= i) {
          break;
        }
        result -= nt.src.size();
      } else {
        if (nt.tgt.start >= i) {
          break;
        }
        result -= nt.tgt.size();
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
  constexpr auto empty = [](auto nt) { return nt.empty(); };
  auto nts = rule.nts;
  auto it = std::remove_if(nts.begin(), nts.end(), empty);
  if constexpr (!SourceSide) {
    std::sort(
        nts.begin(),
        it,
        [](auto a, auto b) { return a.tgt.start < b.tgt.start; });
  }
  auto nt = nts.begin();
  auto indices = rule.lhs.get<SourceSide>().indices();
  for (auto i : indices) {
    if (nt == it || i < nt->template get<SourceSide>().start) {
      if (i != indices.front()) {
        out << ' ';
      }
      if constexpr (SourceSide) {
        out << s.src[i];
      } else {
        out << s.tgt[i];
      }
    } else if (i == nt->template get<SourceSide>().end - 1) {
      if (i != indices.front()
          && nt->template get<SourceSide>().start != indices.front()) {
        out << ' ';
      }
      bracket(out, labeler(*nt), rule.ntIndex(*nt));
      nt++;
    }
  }
}

inline std::ostream& operator<<(std::ostream& out, LabeledRuleView v) {
  const auto& [rule, labeler] = v;
  const static std::string_view kSep = "\t";
  bracket(out, labeler(rule.lhs), kLhsIndex);
  out << kSep;
  printRhs<true>(out, v);
  out << kSep;
  printRhs<false>(out, v);
  out << kSep;
  printAlignment(out, rule.terminalAlignment());
  return out;
}

std::vector<PhrasalRule> extract(
    const AlignedSentencePair& sentence, std::vector<SpanPair> initial);

}
