#include "label.h"

#include <string>
#include <variant>

namespace jhu::thrax {

namespace {

struct Constituent {
  std::string_view label;
};

struct ForwardApply {
  // "result/arg"
  std::string_view result, arg;
};

struct BackwardApply {
  // "result\arg"
  std::string_view result, arg;
};

struct Concat {
  std::string_view a, b;
};

struct DoubleConcat {
  std::string_view a, b, c;
};

using SAMTLabel = std::variant<
  Constituent,
  ForwardApply,
  BackwardApply,
  Concat,
  DoubleConcat
>;

struct LabelVisitor {
  std::string operator()(Constituent c) const {
    return std::string(c.label);
  }
  std::string operator()(ForwardApply fa) const {
    return std::string(fa.result) + "/" + std::string(fa.arg);
  }
  std::string operator()(BackwardApply ba) const {
    return std::string(ba.result) + "\\" + std::string(ba.arg);
  }
  std::string operator()(Concat c) const {
    return std::string(c.a) + "+" + std::string(c.b);
  }
  std::string operator()(DoubleConcat dc) const {
    return std::string(dc.a)
        + "+" + std::string(dc.b)
        + "+" + std::string(dc.c);
  }
};

std::optional<Constituent> constituent(const Tree& tree, Span nt) {
  if (auto l = label(tree, nt); l.has_value()) {
    return Constituent{*l};
  }
  return {};
}

std::optional<ForwardApply> forwardApply(const Tree& tree, Span nt) {
  if (tree.empty()) {
    return {};
  }
  for (auto e = nt.end; e <= tree.front().span.end; e++) {
    auto ret = constituent(tree, Span{ nt.start, e });
    if (!ret.has_value()) {
      continue;
    }
    auto arg = constituent(tree, Span{ nt.end, e });
    if (arg.has_value()) {
      return ForwardApply{ ret->label, arg->label };
    }
  }
  return {};
}

std::optional<BackwardApply> backwardApply(const Tree& tree, Span nt) {
  for (auto b = nt.start; b >= 0; b--) {
    auto ret = constituent(tree, Span{ b, nt.end });
    if (!ret.has_value()) {
      continue;
    }
    auto arg = constituent(tree, Span{ b, nt.start });
    if (arg.has_value()) {
      return BackwardApply{ ret->label, arg->label };
    }
  }
  return {};
}

std::optional<Concat> concatenation(const Tree& tree, Span nt) {
  for (auto i = nt.start; i < nt.end; i++) {
    auto a = constituent(tree, Span{ nt.start, i });
    if (!a.has_value()) {
      continue;
    }
    auto b = constituent(tree, Span{ i, nt.end });
    if (b.has_value()) {
      return Concat{ a->label, b->label };
    }
  }
  return {};
}

std::optional<DoubleConcat> doubleConcatenation(const Tree& tree, Span nt) {
  for (auto i = nt.start; i < nt.end; i++) {
    auto a = constituent(tree, Span{ nt.start, i });
    if (!a.has_value()) {
      continue;
    }
    auto rest = concatenation(tree, Span{ i, nt.end });
    if (rest.has_value()) {
      return DoubleConcat{ a->label, rest->a, rest->b };
    }
  }
  return {};
}

SAMTLabel label(const Tree& tree, SpanPair nt) {
  if (auto label = constituent(tree, nt.tgt); label.has_value()) {
    return *label;
  }
  if (auto fa = forwardApply(tree, nt.tgt); fa.has_value()) {
    return *fa;
  }
  if (auto ba = backwardApply(tree, nt.tgt); ba.has_value()) {
    return *ba;
  }
  if (auto cat = concatenation(tree, nt.tgt); cat.has_value()) {
    return *cat;
  }
  if (auto dc = doubleConcatenation(tree, nt.tgt); dc.has_value()) {
    return *dc;
  }
  return Constituent{"X"};
}

}  // namespace

std::string SAMTLabeler::operator()(SpanPair nt) const {
  return std::visit(LabelVisitor{}, label(tree_, nt));
}

}
