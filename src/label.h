#pragma once

#include "phrase.h"
#include "sentence.h"

namespace jhu::thrax {

class Labeler {
 public:
  virtual std::string operator()(
      const AlignedSentencePair& sentence, SpanPair nt) const = 0;
  virtual ~Labeler() = default;
};

class HieroLabeler : public Labeler {
 public:
  std::string operator()(const AlignedSentencePair&, SpanPair) const override {
    return "X";
  }
};

}
