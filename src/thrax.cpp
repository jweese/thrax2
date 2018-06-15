#include <iostream>
#include <mutex>

#include "label.h"
#include "phrasalrule.h"
#include "sentence.h"
#include "tree.h"

namespace {

std::mutex inputLock, outputLock;

template<bool Lock>
bool process() {
  std::string line;
  {
    std::unique_lock<std::mutex> g(inputLock, std::defer_lock_t{});
    if constexpr (Lock) {
      g.lock();
    }
    if (!std::getline(std::cin, line)) {
      return false;
    }
  }
  auto asp = jhu::thrax::readAlignedSentencePair<false, true>(line);
  auto tree = jhu::thrax::readTree(jhu::thrax::fields(line)[1]);
  jhu::thrax::SAMTLabeler samt{std::move(tree)};
  try {
    {
      std::unique_lock<std::mutex> g(outputLock, std::defer_lock_t{});
      if constexpr (Lock) {
        g.lock();
      }
      for (const auto& rule : jhu::thrax::extract(asp, 10)) {
        std::cout << jhu::thrax::LabeledRuleView{ rule, samt } << '\n';
      }
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << ' ' << line << '\n';
  }
  return true;
}

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  while (process<false>()) {}
}
