#include <iostream>

#include "label.h"
#include "phrasalrule.h"
#include "sentence.h"
#include "tree.h"

int main() {
  std::string line;
  while (std::getline(std::cin, line)) {
    try {
      auto asp = jhu::thrax::readAlignedSentencePair<false, true>(line);
      auto tree = jhu::thrax::readTree(jhu::thrax::fields(line)[1]);
      auto initial = jhu::thrax::allConsistentPairs(asp, 12);
      jhu::thrax::SAMTLabeler samt{std::move(tree)};
      for (const auto& rule : jhu::thrax::extract(asp, initial)) {
        std::cout << jhu::thrax::LabeledRuleView{ rule, samt } << '\n';
      }
    } catch (std::exception& e) {
      std::cerr << e.what() << ' ' << line << '\n';
    }
  }
}
