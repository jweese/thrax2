#include "phrase.h"

#include <algorithm>

namespace jhu::thrax {

std::optional<Span> minimalTargetSpan(const Alignment& a, Span src) {
  auto startPoint = Point{ src.start, 0 };
  auto endPoint = Point{ static_cast<IndexType>(src.end - 1), 0 };
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
  constexpr auto contains = [](IndexType p, Span s) {
    return p >= s.start && p < s.end;
  };
  return std::none_of(
      a.begin(),
      a.end(),
      [contains,sp](Point p) {
        return contains(p.src, sp.src) ^ contains(p.tgt, sp.tgt);
      });
}

std::vector<SpanPair> minimalConsistentPairs(const Alignment& a, int maxSize) {
  std::vector<SpanPair> result;
  result.reserve(maxSize * a.size());  // over-estimate
  for (auto startPoint : a) {
    for (auto endPoint : a) {
      Span src{ startPoint.src, static_cast<IndexType>(endPoint.src + 1) };
      auto sz = src.size();
      if (sz == 0) {
        continue;
      } else if (sz > maxSize) {
        break;
      }
      if (auto tgt = minimalTargetSpan(a, src); tgt.has_value()) {
        result.push_back(SpanPair{ src, *tgt });
      }
    }
  }
  auto it = std::remove_if(
      result.begin(),
      result.end(),
      [&a](auto sp) { return !isConsistent(a, sp); });
  result.erase(it, result.end());
  return result;
}

}
