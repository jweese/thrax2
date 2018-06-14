#include "phrase.h"

#include <algorithm>

namespace jhu::thrax {

std::optional<Span> minimalTargetSpan(const Alignment& a, Span src) {
  auto startPoint = Point{ src.start, 0 };
  auto endPoint = Point{ src.end, 0 };
  auto start = std::lower_bound(a.begin(), a.end(), startPoint, bySource);
  auto end = std::upper_bound(a.begin(), a.end(), endPoint, bySource);
  if (start == end) {
    // no alignment points in src span
    return {};
  }
  auto [min, max] = std::minmax_element(
      start, end, [](Point a, Point b) { return a.tgt < b.tgt; });
  return Span{ min->tgt, static_cast<IndexType>(max->tgt + 1) };
}

bool isConsistent(const Alignment& a, SpanPair sp) {
  static auto contains = [](Point p, Span s) {
    return p.src >= s.start && p.tgt < s.end;
  };
  return std::none_of(
      a.begin(),
      a.end(),
      [sp](Point p) {
        return contains(p, sp.src) ^ contains(p, sp.tgt);
      });
}

}
