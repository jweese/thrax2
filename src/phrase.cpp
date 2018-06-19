#include "phrase.h"

#include <algorithm>

#include "sentence.h"

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
      if (src.empty()) {
        continue;
      } else if (auto it = std::find_if(
          result.begin(), result.end(), [src](auto p) { return src == p.src; });
          it != result.end()) {
        continue;
      } else if (src.size() > maxSize) {
        break;
      }
      if (auto tgt = minimalTargetSpan(a, src);
          tgt.has_value() && tgt->size() <= maxSize) {
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

namespace {

auto expand(const AlignedSentencePair& sentence, SpanPair sp, int maxSize) {
  const auto& a = sentence.alignment;
  auto sent = sentence.span();
  SpanPair s(sp);
  while (sent.contains(s) && isConsistent(a, s)) {
    s.src.start--;
  }
  s.src.start++;
  while (sent.contains(s) && isConsistent(a, s)) {
    s.src.end++;
  }
  s.src.end--;
  while (sent.contains(s) && isConsistent(a, s)) {
    s.tgt.start--;
  }
  s.tgt.start++;
  while (sent.contains(s) && isConsistent(a, s)) {
    s.tgt.end++;
  }
  s.tgt.end--;
  std::vector<SpanPair> result;
  for (auto i = s.src.start; i <= sp.src.start; i++) {
    for (auto j = sp.src.end; j <= s.src.end; j++) {
      for (auto x = s.tgt.start; x <= sp.tgt.start; x++) {
        for (auto y = sp.tgt.end; y <= s.tgt.end; y++) {
          auto res = SpanPair{ { i, j }, { x, y } };
          if (res.src.size() <= maxSize && res.tgt.size() <= maxSize) {
            result.push_back(res);
          }
        }
      }
    }
  }
  return result;
}

}  // namespace

std::vector<SpanPair> allConsistentPairs(
    const AlignedSentencePair& sentence, int maxSize) {
  std::vector<SpanPair> result;
  for (auto sp : minimalConsistentPairs(sentence.alignment, maxSize)) {
    auto es = expand(sentence, sp, maxSize);
    result.insert(result.end(), es.begin(), es.end());
  }
  return result;
}

}
