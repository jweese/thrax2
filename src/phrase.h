#pragma once

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

  auto has_value() const {
    return start != end;
  }
};

struct SpanPair {
  Span src, tgt;

  auto has_value() const {
    return src.has_value() && tgt.has_value();
  }
};

std::optional<Span> minimalTargetSpan(const Alignment& a, Span src);
bool isConsistent(const Alignment& a, SpanPair sp);
std::vector<SpanPair> minimalConsistentPairs(const Alignment& a, int maxSize);

}
