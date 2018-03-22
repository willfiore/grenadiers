#include "Random.hpp"

// Initialize random generator
std::mt19937 Random::generator = std::mt19937(std::random_device()());

double Random::randomDouble(double a, double b) {
  std::uniform_real_distribution<double> distribution(a, b);
  return distribution(generator);
}

float Random::randomFloat(float a, float b) {
  std::uniform_real_distribution<float> distribution(a, b);
  return distribution(generator);
}

int Random::randomInt(int a, int b) {
  std::uniform_int_distribution<int> distribution(a, b);
  return distribution(generator);
}
