#include <future>
#include <iostream>
#include <mutex>
#include <vector>

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
  try {
    auto asp = jhu::thrax::readAlignedSentencePair<false, true>(line);
    auto tree = jhu::thrax::readTree(jhu::thrax::fields(line)[1]);
    jhu::thrax::SAMTLabeler samt{std::move(tree)};
    std::unique_lock<std::mutex> g(outputLock, std::defer_lock_t{});
    if constexpr (Lock) {
      g.lock();
    }
    for (const auto& rule : jhu::thrax::extract(asp, 10)) {
      std::cout << jhu::thrax::LabeledRuleView{ rule, samt } << '\n';
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << ' ' << line << '\n';
  }
  return true;
}

}  // namespace

int main(int argc, char** argv) {
  int threads = 1;
  if (argc > 1) {
    threads = std::atoi(argv[1]);
  }
  std::ios::sync_with_stdio(false);
  if (threads < 2) {
    while (process<false>()) {}
    return 0;
  }
  std::vector<std::future<void>> workers;
  workers.reserve(threads);
  for (int i = 0; i < threads; i++) {
    workers.push_back(std::async([]() { while (process<true>()) {} }));
  }
  for (auto& f : workers) {
    f.get();
  }
}
