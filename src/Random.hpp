#pragma once

#include <random>

class Random {
public:
  Random();

  static double randomDouble(double a, double b);
  static float randomFloat(float a, float b);
  static int randomInt(int a, int b);

private:
  static std::mt19937 generator;
};
