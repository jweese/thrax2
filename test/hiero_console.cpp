#include <iostream>

#include "label.h"
#include "phrasalrule.h"
#include "sentence.h"

int main() {
  std::string line;
  jhu::thrax::HieroLabeler hiero{};
  while (std::getline(std::cin, line)) {
    auto asp = jhu::thrax::readAlignedSentencePair<false, false>(line);
    for (const auto& rule : jhu::thrax::extract(asp, 10)) {
      std::cout << jhu::thrax::LabeledRuleView{ rule, hiero } << '\n';
    }
  }
}
