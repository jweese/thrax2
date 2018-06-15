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

SAMTLabel label(const Tree&, SpanPair) {
  return Constituent{"X"};
}

}  // namespace

std::string SAMTLabeler::operator()(SpanPair nt) const {
  return std::visit(LabelVisitor{}, label(tree_, nt));
}

}
