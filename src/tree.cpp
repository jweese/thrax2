#include "tree.h"

#include "sentence.h"  // split

namespace jhu::thrax {

Tree readTree(std::string_view line) {
  auto tokens = split(line);
  auto nodeCount = std::count_if(
      tokens.begin(), tokens.end(), [](auto t) { return t.front() == '('; });
  Tree result;
  result.reserve(nodeCount);
  Node* curr = nullptr;
  for (auto t : tokens) {
    if (t.front() == '(') {
      IndexType start = result.empty() ? 0 : result.back().span.end;
      t.remove_prefix(1);
      result.push_back(Node{ { start, start }, t, curr });
      curr = &result.back();
    } else {
      result.back().add_suffix();
    }
    while (!t.empty() && t.back() == ')') {
      if (curr != nullptr) {
        curr = curr->parent;
      }
      t.remove_suffix(1);
    }
  }
  if (curr != nullptr) {
    // Not enough close parens
    throw std::invalid_argument("bad tree: " + std::string(line));
  }
  return result;
}

}
