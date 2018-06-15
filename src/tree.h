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
  auto it = std::find_if(
      t.begin(), t.end(), [s](auto n) { return n.span == s; });
  return it == t.end() ? MaybeLabel{} : it->label;
}

Tree readTree(std::string_view line);

}
