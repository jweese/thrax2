#pragma once

#include <algorithm>
#include <iostream>
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
    return start >= end;
  }

  auto indices() const {
    Indices result(size());
    std::iota(result.begin(), result.end(), start);
    return result;
  }

  bool operator==(Span s) const {
    return start == s.start && end == s.end;
  }

  bool operator!=(Span s) const {
    return !(*this == s);
  }

  bool contains(Span s) const {
    return s.start >= start && s.end <= end;
  }
};

inline void removeIndices(Indices& from, Span s) {
  auto b = std::lower_bound(from.begin(), from.end(), s.start);
  auto e = std::lower_bound(from.begin(), from.end(), s.end);
  if (b != from.end() && *b < s.start) {
    b++;
  }
  from.erase(b, e);
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

  bool contains(SpanPair sp) const {
    return src.contains(sp.src) && tgt.contains(sp.tgt);
  }
};

constexpr bool disjoint(SpanPair a, SpanPair b) {
  return disjoint(a.src, b.src) && disjoint(a.tgt, b.tgt);
}

inline std::ostream& operator<<(std::ostream& out, Span s) {
  return out << '[' << s.start << ',' << s.end << ')';
}

inline std::ostream& operator<<(std::ostream& out, SpanPair sp) {
  return out << sp.src << '+' << sp.tgt;
}

std::optional<Span> minimalTargetSpan(const Alignment& a, Span src);
bool isConsistent(const Alignment& a, SpanPair sp);
std::vector<SpanPair> minimalConsistentPairs(const Alignment& a, int maxSize);

}
