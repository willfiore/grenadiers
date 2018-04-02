#include "Console.hpp"
#include "imgui.h"
#include <algorithm>

// Static initialization
std::ostringstream Console::stream;
std::deque<std::string> Console::entries;

std::ostringstream& Console::log()
{
  flush();
  return stream;
}

void Console::flush()
{
  if (stream.str() == std::string()) return;

  entries.push_back(stream.str());
  stream.str(std::string());

  while (entries.size() > 200) {
    entries.pop_front();
  }
}

void Console::render()
{
  flush();

  ImGui::Begin("Debug Console", NULL, ImGuiWindowFlags_NoCollapse);
  ImGui::TextColored({0.0, 1.0, 0.0, 1.0},
      "Framerate: %2.f", ImGui::GetIO().Framerate);
  ImGui::TextColored({0.4, 0.4, 0.4, 1.0},
      "--------------------");

  for (auto e : entries) {

    ImVec4 col = {0.8, 0.8, 0.8, 1.0};

    bool first = true;
    size_t found = 0;
    while (found != std::string::npos) {
      found = e.find("^^");
      std::string p = e.substr(0, found);

      // Print previous text
	if (!first) {
	  ImGui::SameLine(0.f, 1.f);
	}
	ImGui::TextColored(col, "%s", p.c_str());

      if (found+2 < e.size()) {
	int colorCode = e[found+2] - '0';

	if (colorCode < 0 || colorCode > 9) {
	}
	else {
	  switch(colorCode) {
	    case 1: col = {1.0, 0.0, 0.0, 1.0}; break;
	    case 2: col = {0.0, 1.0, 0.0, 1.0}; break;
	    case 3: col = {1.0, 1.0, 0.0, 1.0}; break;
	    case 4: col = {0.0, 0.0, 1.0, 1.0}; break;
	    case 5: col = {0.0, 1.0, 1.0, 1.0}; break;
	    case 6: col = {1.0, 0.0, 1.0, 1.0}; break;
	    case 7: col = {1.0, 1.0, 1.0, 1.0}; break;
	    case 8: col = {0.0, 0.0, 0.0, 1.0}; break;
	    case 0:
	    default: break;
	  }
	}
      }
      if (found != std::string::npos) {
	e = e.substr(found+3, std::string::npos);
      }
      first = false;
    }
  }

  ImGui::SetScrollHere();
  ImGui::End();
}
