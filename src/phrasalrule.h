#pragma once

#include <array>
#include <ostream>

#include "label.h"
#include "phrase.h"
#include "sentence.h"

namespace jhu::thrax {

constexpr size_t kMaxNonterminals = 4;

struct PhrasalRule {
  std::array<SpanPair, kMaxNonterminals> nts{};
  SpanPair lhs{};
  const AlignedSentencePair& sentence;

  explicit PhrasalRule(const AlignedSentencePair& asp)
      : PhrasalRule(asp, SpanPair{}) {}
  explicit PhrasalRule(const AlignedSentencePair& asp, SpanPair root)
      : lhs(root), sentence(asp) {}
};

struct LabeledRuleView {
  const PhrasalRule& rule;
  const Labeler& labeler;
};

std::ostream& operator<<(std::ostream& out, LabeledRuleView v) {
  constexpr int kLhs = 0;
  auto bracket = [&out](std::string_view nt, int index) {
    out << '[' << nt;
    if (index != kLhs) {
      out << ',' << index;
    }
    out << ']';
  };
  const auto& [rule, labeler] = v;
  const auto& s = rule.sentence;
  bracket(labeler(s, rule.lhs), kLhs);
}

}
