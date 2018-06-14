#pragma once

#include <numeric>
#include <optional>
#include <vector>

#include "alignment.h"

namespace jhu::thrax {

using IndexType = int16_t;

struct Span {
  IndexType start = 0;
  IndexType end = 0;

  auto size() const {
    return std::max(end - start, 0);
  }

  auto empty() const {
    return start == end;
  }

  auto indices() const {
    std::vector<IndexType> result(size());
    std::iota(result.begin(), result.end(), start);
    return result;
  }
};

constexpr bool disjoint(Span a, Span b) {
  if (b.start < a.start) {
    std::swap(a, b);
  }
  return b.start >= a.end;
}

struct SpanPair {
  Span src, tgt;

  auto empty() const {
    return src.empty() || tgt.empty();
  }
};

std::optional<Span> minimalTargetSpan(const Alignment& a, Span src);
bool isConsistent(const Alignment& a, SpanPair sp);
std::vector<SpanPair> minimalConsistentPairs(const Alignment& a, int maxSize);

}
