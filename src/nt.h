#pragma once

#include <string_view>
#include <vector>

#include "phrase.h"

namespace jhu::thrax {

struct NT {
  SpanPair span{};
  std::string_view label = "X";

  NT() = default;
  NT(SpanPair sp) : span(sp) {}
  NT(SpanPair sp, std::string_view sv) : span(sp), label(sv) {}
};

constexpr bool bySourceStart(const NT& a, const NT& b) {
  return a.span.src.start < b.span.src.start;
}

struct ContainedNTs {
  NT lhs;
  std::vector<const NT*> contained;

  ContainedNTs() = default;
  ContainedNTs(const std::vector<NT>& nts, NT root)
      : lhs(std::move(root)), contained() {
    for (const auto& nt : nts) {
      if (lhs.span.contains(nt.span)) {
        contained.push_back(&nt);
      }
    }
  }
};

constexpr bool before(Span a, Span b) {
  return a.start < b.start || (a.start == b.start && a.end < b.end);
}

constexpr bool byLhsSpan(const ContainedNTs& a, const ContainedNTs& b) {
  auto al = a.lhs.span;
  auto bl = b.lhs.span;
  return before(al.src, bl.src) || (al.src == bl.src && before(al.tgt, bl.tgt));
}

}
