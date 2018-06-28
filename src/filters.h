#pragma once

#include "phrasalrule.h"

namespace jhu::thrax {

inline bool isNonlexicalXRule(const PhrasalRule& rule) {
  return !rule.lexical() && rule.lhs.label == "X";
}

}
