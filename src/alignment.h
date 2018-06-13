#pragma once

#include <string_view>
#include <vector>

namespace jhu::thrax {

using PointType = int16_t;

struct Point {
  PointType src = 0;
  PointType tgt = 0;
};

using Alignment = std::vector<Point>;

Alignment readAlignment(std::string_view sv);

}
