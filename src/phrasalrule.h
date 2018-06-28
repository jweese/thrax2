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

struct NT {
  SpanPair span{};
  std::string_view label{};

  NT() = default;
  NT(SpanPair sp) : span(sp) {}
  NT(SpanPair sp, std::string_view sv) : span(sp), label(sv) {}
};
using Rhs = std::array<NT, kMaxNonterminals>;

struct PhrasalRule {
  Rhs nts{};
  int nextNT = 0;
  NT lhs{};
  const AlignedSentencePair& sentence;
  Alignment alignment;

  explicit PhrasalRule(const AlignedSentencePair& asp)
      : PhrasalRule(asp, asp.span()) {}
  explicit PhrasalRule(const AlignedSentencePair& asp, NT root)
      : lhs(root),
        sentence(asp),
        alignment(copyPoints(
              sentence.alignment, lhs.span.src.start, lhs.span.src.end)) {}

  int ntIndex(SpanPair nt) const {
    auto it = std::find_if(
        nts.begin(), nts.end(), [nt](auto i) { return i.span == nt; });
    return static_cast<int>(1 + std::distance(nts.begin(), it));
  }


  auto terminalAlignment() const {
    Alignment result(alignment);
    for (auto& point : result) {
      point.src = terminalIndex<true>(point.src);
      point.tgt = terminalIndex<false>(point.tgt);
    }
    return result;
  }

 private:
  template<bool SourceSide>
  PointType terminalIndex(PointType i) const {
    auto result = i;
    if constexpr (SourceSide) {
      result -= lhs.span.src.start;
    } else {
      result -= lhs.span.tgt.start;
    }
    for (auto nt : nts) {
      if constexpr (SourceSide) {
        if (nt.span.src.start < i) {
          result -= nt.span.src.size();
        }
      } else {
        if (nt.span.tgt.start < i) {
          result -= nt.span.tgt.size();
        }
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
  constexpr auto empty = [](auto nt) { return nt.span.empty(); };
  auto nts = rule.nts;
  auto it = std::find_if(nts.begin(), nts.end(), empty);
  if constexpr (!SourceSide) {
    std::sort(
        nts.begin(),
        it,
        [](auto a, auto b) { return a.span.tgt.start < b.span.tgt.start; });
  }
  auto nt = nts.begin();
  auto [start, end] = rule.lhs.span.get<SourceSide>();
  for (auto i = start; i < end; i++) {
    if (nt == it || i < nt->span.template get<SourceSide>().start) {
      if (i != start) {
        out << ' ';
      }
      if constexpr (SourceSide) {
        out << s.src[i];
      } else {
        out << s.tgt[i];
      }
    } else if (i == nt->span.template get<SourceSide>().end - 1) {
      if (i != start && nt->span.template get<SourceSide>().start != start) {
        out << ' ';
      }
      bracket(out, labeler(nt->span), rule.ntIndex(nt->span));
      nt++;
    }
  }
}

inline std::ostream& operator<<(std::ostream& out, LabeledRuleView v) {
  const auto& [rule, labeler] = v;
  const static std::string_view kSep = "\t";
  bracket(out, labeler(rule.lhs.span), kLhsIndex);
  out << kSep;
  printRhs<true>(out, v);
  out << kSep;
  printRhs<false>(out, v);
  out << kSep;
  printAlignment(out, rule.terminalAlignment());
  return out;
}

std::vector<PhrasalRule> extract(
    const Labeler& labeler,
    const AlignedSentencePair& sentence,
    std::vector<SpanPair> initial);

}
