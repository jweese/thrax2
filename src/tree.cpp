#include "tree.h"

#include "sentence.h"  // split

namespace jhu::thrax {

Tree readTree(std::string_view line) {
  auto tokens = split(line);
  auto nodeCount = std::count_if(
      tokens.begin(), tokens.end(), [](auto t) { return t.front() == '('; });
  Tree result(nodeCount);
  size_t nextNodeIndex = 0;
  Node* curr = nullptr;
  for (auto t : tokens) {
    if (t.front() == '(') {
      IndexType start = curr == nullptr ? 0 : curr->span.end;
      t.remove_prefix(1);
      result[nextNodeIndex] = Node{ { start, start }, t, curr };
      curr = &result[nextNodeIndex++];
    } else {
      if (curr == nullptr) {
        throw std::invalid_argument("bad tree: " + std::string(line));
      }
      curr->add_suffix();
    }
    while (!t.empty() && t.back() == ')') {
      if (curr == nullptr) {
        throw std::invalid_argument("bad tree: " + std::string(line));
      }
      curr = curr->parent;
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
