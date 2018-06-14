#pragma once

#include <array>

#include "phrase.h"
#include "sentence.h"

namespace jhu::thrax {

constexpr size_t kMaxNonterminals = 4;

struct PhrasalRule {
  std::array<SpanPair, kMaxNonterminals> nts;
  SpanPair lhs;
  AlignedSentencePair* sentence;
}

}
