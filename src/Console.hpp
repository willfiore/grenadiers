#pragma once

#include <sstream>
#include <deque>
#include <string>

constexpr char none[] = "^^0";
constexpr char red[] = "^^1";
constexpr char green[] = "^^2";
constexpr char yellow[] = "^^3";
constexpr char blue[] = "^^4";
constexpr char cyan[] = "^^5";
constexpr char magenta[] = "^^6";
constexpr char white[] = "^^7";
constexpr char black[] = "^^8";

class Console {
public:
  static std::ostringstream& log();
  static void flush();
  static void render();
private:
  Console() {};

  static std::ostringstream stream;
  static std::deque<std::string> entries;
};
