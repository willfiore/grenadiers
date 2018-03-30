#pragma once

#include <set>
#include <vector>

struct ControllerData
{
  std::set<int> buttons;
  std::vector<float> axes;
};
