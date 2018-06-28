#pragma once

#include "phrasalrule.h"

namespace jhu::thrax {

inline bool isNonlexicalXRule(const PhrasalRule& rule) {
  return rule.nextNT != 0 && rule.lhs.label == "X";
}

}
