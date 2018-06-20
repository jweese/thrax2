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

enum class GrammarType {
  Hiero,
  SAMT,
};

template<GrammarType Grammar = GrammarType::Hiero>
bool process() {
  std::string line;
  {
    std::lock_guard g(inputLock);
    if (!std::getline(std::cin, line)) {
      return false;
    }
  }
  try {
    auto asp = jhu::thrax::readAlignedSentencePair<false, true>(line);
    auto initial = jhu::thrax::minimalConsistentPairs(asp.alignment, 10);
    std::unique_ptr<jhu::thrax::Labeler> label =
        std::make_unique<jhu::thrax::HieroLabeler>();
    if constexpr (Grammar == GrammarType::SAMT) {
      auto tree = jhu::thrax::readTree(jhu::thrax::fields(line)[1]);
      label = std::make_unique<jhu::thrax::SAMTLabeler>(std::move(tree));
    }
    std::ostringstream out;
    for (const auto& rule : jhu::thrax::extract(asp, initial)) {
      out << jhu::thrax::LabeledRuleView{ rule, *label } << '\n';
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
  // std::ios::sync_with_stdio(false);
  if (threads < 2) {
    while (process()) {}
    return 0;
  }
  std::vector<std::future<void>> workers(threads);
  for (auto& t : workers) {
    t = std::async([]() { while (process<GrammarType::SAMT>()) {} });
  }
}
