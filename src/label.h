#pragma once

#include "phrase.h"
#include "sentence.h"
#include "tree.h"

namespace jhu::thrax {

class Labeler {
 public:
  virtual std::string operator()(SpanPair nt) const = 0;
  virtual ~Labeler() = default;
};

class HieroLabeler : public Labeler {
 public:
  std::string operator()(SpanPair) const override {
    return "X";
  }
};

struct CachedLabel {
  Span span;
  std::string label;
};

class SAMTLabeler : public Labeler {
 public:
  explicit SAMTLabeler(Tree t) : tree_(std::move(t)) {}
  std::string operator()(SpanPair nt) const override;

 private:
  Tree tree_;
  mutable std::vector<CachedLabel> cache_;
  mutable CachedLabel key_;
};

}
