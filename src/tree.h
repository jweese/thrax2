#pragma once

#include <algorithm>
#include <optional>
#include <string_view>
#include <vector>

#include "phrase.h"

namespace jhu::thrax {

struct Node {
  Span span{};
  std::string_view label{};
  Node* parent = nullptr;

  void add_suffix() {
    // increase span by one (when reading in a terminal symbol)
    span.end++;
    if (parent != nullptr) {
      parent->add_suffix();
    }
  }
};

using Tree = std::vector<Node>;
using MaybeLabel = std::optional<std::string_view>;

inline MaybeLabel label(const Tree& t, Span s) {
  auto n = Node{ s, {}, nullptr };
  auto it = std::upper_bound(
      t.begin(), t.end(), n,
      [](const auto& a, const auto& b) {
        return a.span.start < b.span.start ||
              (a.span.start == b.span.start && a.span.end > b.span.end);
              // Among nodes that have the same start point, the greater
              // end point comes first (i.e. larger spans before smaller).
      });
  if (it == t.begin() || std::prev(it)->span != s) {
    return {};
  }
  return std::prev(it)->label;
}

Tree readTree(std::string_view line);

}
