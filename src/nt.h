#pragma once

#include <string_view>

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

}
