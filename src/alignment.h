#pragma once

#include <string_view>
#include <vector>

namespace jhu::thrax {

using PointType = int16_t;

struct Point {
  PointType src = 0;
  PointType tgt = 0;
};

constexpr auto bySource = [](Point a, Point b) { return a.src < b.src; };
constexpr auto byTarget = [](Point a, Point b) { return a.tgt < b.tgt; };

using Alignment = std::vector<Point>;

Alignment readAlignment(std::string_view sv);

}
