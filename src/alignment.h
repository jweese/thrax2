#pragma once

#include <algorithm>
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
constexpr auto byTarget = [](Point a, Point b) { return a.tgt < b.tgt; };

using Alignment = std::vector<Point>;

Alignment readAlignment(std::string_view sv);
void printAlignment(std::ostream& out, const Alignment& alignment);

inline auto copyPoints(const Alignment& a, PointType from, PointType to) {
  auto b = std::lower_bound(a.begin(), a.end(), Point{from, 0}, bySource);
  auto e = std::lower_bound(a.begin(), a.end(), Point{to, 0}, bySource);
  if (e != a.end() && e->src < to) {
    e++;
  }
  return Alignment(b, e);
}

inline void cutPoints(Alignment& a, PointType from, PointType to) {
  auto b = std::lower_bound(a.begin(), a.end(), Point{from, 0}, bySource);
  auto e = std::lower_bound(a.begin(), a.end(), Point{to, 0}, bySource);
  if (e != a.end() && e->src < to) {
    e++;
  }
  a.erase(b, e);
}

}
