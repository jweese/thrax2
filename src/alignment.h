#pragma once

#include <iostream>
#include <string_view>
#include <vector>

namespace jhu::thrax {

using PointType = int16_t;

struct Point {
  PointType src = 0;
  PointType tgt = 0;
};

inline std::ostream& operator<<(std::ostream& out, Point p) {
  return out << p.src << '-' << p.tgt;
}

constexpr auto bySource = [](Point a, Point b) { return a.src < b.src; };

using Alignment = std::vector<Point>;

Alignment readAlignment(std::string_view sv);
void printAlignment(std::ostream& out, const Alignment& alignment);

}
