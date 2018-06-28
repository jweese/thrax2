#pragma once

#include "phrasalrule.h"

namespace jhu::thrax {

inline bool isNonlexicalXRule(const PhrasalRule& rule) {
  return !rule.lexical() && rule.lhs.label == "X";
}

inline bool withinTokenLimit(const PhrasalRule& rule) {
  constexpr int kLexicalLimit = 10;
  constexpr int kNonlexLimit = 5;
  bool lex = rule.lexical();
  auto src = rule.numTokens<true>();
  auto tgt = rule.numTokens<false>();
  if (lex) {
    return src < kLexicalLimit && tgt < kLexicalLimit;
  } else {
    return src < kNonlexLimit && tgt < kNonlexLimit;
  }
}

}
