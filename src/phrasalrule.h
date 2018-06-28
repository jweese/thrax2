#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <ostream>
#include <string_view>
#include <vector>

#include "label.h"
#include "nt.h"
#include "phrase.h"
#include "sentence.h"

namespace jhu::thrax {

constexpr size_t kMaxNonterminals = 4;
using Rhs = std::array<NT, kMaxNonterminals>;

struct PhrasalRule {
  Rhs nts{};
  int nextNT = 0;
  NT lhs{};
  const AlignedSentencePair& sentence;

  explicit PhrasalRule(const AlignedSentencePair& asp)
      : PhrasalRule(asp, asp.span()) {}
  PhrasalRule(const AlignedSentencePair& asp, NT root)
      : lhs(std::move(root)),
        sentence(asp) {}
  PhrasalRule(const PhrasalRule& prev, NT nt)
      : nts(prev.nts),
        nextNT(prev.nextNT + 1),
        lhs(prev.lhs),
        sentence(prev.sentence) {
    nts[nextNT - 1] = std::move(nt);
  }

  int ntIndex(const NT& needle) const {
    int i = 1;
    for (const auto& nt : nts) {
      if (nt.span == needle.span) {
        return i;
      } else {
        i++;
      }
    }
    return 0;
  }

  void printAlignment(std::ostream& out) const {
    bool firstPointToPrint = true;
    for (auto point : sentence.alignment) {
      auto src = terminalIndex<true>(point.src);
      if (src < 0) {
        continue;
      }
      auto tgt = terminalIndex<false>(point.tgt);
      if (firstPointToPrint) {
        firstPointToPrint = false;
      } else {
        out << ' ';
      }
      out << src << '-' << tgt;
    }
  }

  Span remainingSource() const {
    if (nextNT == 0) {
      return lhs.span.src;
    }
    return Span{ nts[nextNT - 1].span.src.end, lhs.span.src.end };
  }

  bool lexical() const {
    return nextNT == 0;
  }

 private:
  template<bool SourceSide>
  PointType terminalIndex(PointType i) const {
    auto [from, to] = lhs.span.get<SourceSide>();
    if (i < from || i >= to) {
      return -1;
    }
    auto result = i;
    result -= from;
    for (const auto& nt : nts) {
      auto [s, e] = nt.span.get<SourceSide>();
      if (i < s) {
        continue;
      }
      if (i < e) {
        // in the NT
        return -1;
      }
      result -= (e - s);
    }
    return result;
  }
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
inline void printRhs(std::ostream& out, const PhrasalRule& rule) {
  const auto& s = rule.sentence;
  constexpr auto empty = [](const auto& nt) { return nt.span.empty(); };
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
  auto [ntStart, ntEnd] = nt->span.get<SourceSide>();
  for (auto i = start; i < end; i++) {
    if (nt == it || i < ntStart) {
      if (i != start) {
        out << ' ';
      }
      if constexpr (SourceSide) {
        out << s.src[i];
      } else {
        out << s.tgt[i];
      }
    } else {
      if (i != start && ntStart != start) {
        out << ' ';
      }
      bracket(out, nt->label, rule.ntIndex(*nt));
      i = ntEnd - 1;
      nt++;
      if (nt < it) {
        auto [s, e] = nt->span.get<SourceSide>();
        ntStart = s;
        ntEnd = e;
      }
    }
  }
}

inline std::ostream& operator<<(std::ostream& out, const PhrasalRule& rule) {
  const static std::string_view kSep = "\t";
  bracket(out, rule.lhs.label, kLhsIndex);
  out << kSep;
  printRhs<true>(out, rule);
  out << kSep;
  printRhs<false>(out, rule);
  out << kSep;
  rule.printAlignment(out);
  return out;
}

std::vector<PhrasalRule> extract(
    const Labeler& labeler,
    const AlignedSentencePair& sentence,
    const std::vector<SpanPair>& initial);

}
