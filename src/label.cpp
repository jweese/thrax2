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

auto replaceComma(std::string_view label) {
  // ',' is a separator between NT symbol and RHS index.
  return std::string(label == "," ? "COMMA" : label);
}

struct LabelVisitor {
  std::string operator()(Constituent c) const {
    return replaceComma(c.label);
  }
  std::string operator()(ForwardApply fa) const {
    return replaceComma(fa.result) + "/" + replaceComma(fa.arg);
  }
  std::string operator()(BackwardApply ba) const {
    return replaceComma(ba.result) + "\\" + replaceComma(ba.arg);
  }
  std::string operator()(Concat c) const {
    return replaceComma(c.a) + "+" + replaceComma(c.b);
  }
  std::string operator()(DoubleConcat dc) const {
    return replaceComma(dc.a)
        + "+" + replaceComma(dc.b)
        + "+" + replaceComma(dc.c);
  }
};

std::optional<Constituent> constituent(const Tree& tree, Span nt) {
  if (auto l = label(tree, nt); l.has_value()) {
    return Constituent{*l};
  }
  return {};
}

constexpr auto byStart = [](const auto& a, const auto& b) {
  return a.span.start < b.span.start;
};

std::optional<ForwardApply> forwardApply(const Tree& tree, Span nt) {
  if (tree.empty()) {
    return {};
  }
  auto [retStart, retEnd] = std::equal_range(
      tree.begin(), tree.end(), Node{ nt, {}, nullptr }, byStart);
  auto [argStart, argEnd] = std::equal_range(
      tree.begin(), tree.end(), Node{ Span{nt.end, 0 }, {}, nullptr }, byStart);
  // Reverse the ranges so we can get the lowest constituents.
  for (auto r = retEnd - 1; r >= retStart; r--) {
    for (auto a = argEnd - 1; a >= argStart; a--) {
      if (r->span.end == a->span.end) {
        return ForwardApply{ r->label, a->label };
      }
    }
  }
  return {};
}

std::optional<BackwardApply> backwardApply(const Tree& tree, Span nt) {
  for (const auto& node : tree) {
    if (node.span.start >= nt.start) {
      break;
    }
    if (node.span.end != nt.end) {
      continue;
    }
    auto arg = label(tree, Span{ node.span.start, nt.start });
    if (arg.has_value()) {
      return BackwardApply{ node.label, *arg };
    }
  }
  return {};
}

std::optional<Concat> concatenation(const Tree& tree, Span nt) {
  auto [s, e] = std::equal_range(
      tree.begin(), tree.end(), Node{ nt, {}, nullptr }, byStart);
  for (auto it = s; it < e; it++) {
    auto b = constituent(tree, Span{ it->span.end, nt.end });
    if (b.has_value()) {
      return Concat{ it->label, b->label };
    }
  }
  return {};
}

std::optional<DoubleConcat> doubleConcatenation(const Tree& tree, Span nt) {
  auto [s, e] = std::equal_range(
      tree.begin(), tree.end(), Node{ nt, {}, nullptr }, byStart);
  for (auto it = s; it < e; it++) {
    auto rest = concatenation(tree, Span{ it->span.end, nt.end });
    if (rest.has_value()) {
      return DoubleConcat{ it->label, rest->a, rest->b };
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

constexpr bool spanOrder(const CachedLabel& la, const CachedLabel& lb) {
  auto a = la.span;
  auto b = lb.span;
  return a.start < b.start || (a.start == b.start && a.end < b.end);
}

}  // namespace

std::string_view SAMTLabeler::operator()(SpanPair nt) const {
  key_.span = nt.tgt;
  auto it = std::lower_bound(cache_.begin(), cache_.end(), key_, spanOrder);
  if (it != cache_.end() && it->span == nt.tgt) {
    return it->label;
  }
  auto result = std::visit(LabelVisitor{}, label(tree_, nt));
  return cache_.insert(it, CachedLabel{ nt.tgt, result })->label;
}

}
