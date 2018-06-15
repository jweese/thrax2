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
  // search from the back to get the lowest label
  auto it = std::find_if(
      tree.rbegin(), tree.rend(), [nt](auto n) { return n.span == nt; });
  if (it == tree.rend()) {
    return {};
  }
  return Constituent{it->label};
}

std::optional<ForwardApply> forwardApply(const Tree& tree, Span nt) {
  if (tree.empty()) {
    return {};
  }
  for (auto e = nt.end; e <= tree.front().span.end; e++) {
    auto ret = constituent(tree, Span{ nt.start, e });
    auto arg = constituent(tree, Span{ nt.end, e });
    if (ret.has_value() && arg.has_value()) {
      return ForwardApply{ ret->label, arg->label };
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
  return Constituent{"X"};
}

}  // namespace

std::string SAMTLabeler::operator()(SpanPair nt) const {
  return std::visit(LabelVisitor{}, label(tree_, nt));
}

}
