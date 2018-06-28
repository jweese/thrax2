#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <vector>

#include "label.h"
#include "phrasalrule.h"
#include "sentence.h"
#include "tree.h"

namespace {

std::mutex inputLock, outputLock;

bool process() {
  std::string line;
  {
    std::lock_guard g(inputLock);
    if (!std::getline(std::cin, line)) {
      return false;
    }
  }
  try {
    auto asp = jhu::thrax::readAlignedSentencePair<false, false>(line);
    auto initial = jhu::thrax::minimalConsistentPairs(asp.alignment, 10);
    auto label = jhu::thrax::HieroLabeler{};
    std::ostringstream out;
    for (auto& rule : jhu::thrax::extract(label, asp, std::move(initial))) {
      out << std::move(rule) << '\n';
    }
    std::lock_guard g(outputLock);
    std::cout << out.str();
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
  if (threads < 2) {
    while (process()) {}
    return 0;
  }
  std::vector<std::future<void>> workers(threads);
  for (auto& t : workers) {
    t = std::async([]() { while (process()) {} });
  }
}
