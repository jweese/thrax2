#include "label.h"

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

}  // namespace

std::string SAMTLabeler::operator()(SpanPair) const {
  return "";
}

}
