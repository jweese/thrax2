#pragma once

#include <algorithm>
#include <numeric>
#include <optional>
#include <vector>

#include "alignment.h"

namespace jhu::thrax {

using IndexType = int16_t;
using Indices = std::vector<IndexType>;

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
    Indices result(size());
    std::iota(result.begin(), result.end(), start);
    return result;
  }

  bool operator==(Span s) const {
    return start == s.start && end == s.end;
  }
};

inline void removeIndices(Indices& from, Span s) {
  auto it = std::remove_if(
      from.begin(),
      from.end(),
      [s](auto i) { return i >= s.start && i < s.end; });
  from.erase(it, from.end());
}

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

  template<bool SourceSide> Span get() const {
    if constexpr (SourceSide) {
      return src;
    } else {
      return tgt;
    }
  }

  bool operator==(SpanPair sp) const {
    return src == sp.src && tgt == sp.tgt;
  }
};

std::optional<Span> minimalTargetSpan(const Alignment& a, Span src);
bool isConsistent(const Alignment& a, SpanPair sp);
std::vector<SpanPair> minimalConsistentPairs(const Alignment& a, int maxSize);

}
