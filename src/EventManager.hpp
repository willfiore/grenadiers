#pragma once

#include <functional>
#include <vector>
#include <map>

#include "Event.hpp"

using namespace std::placeholders;

class EventManager
{
public:
  static void Register(Event::Type, std::function<void(Event)>);

  static void Send(Event);
  static void Send(Event::Type);
private:
  EventManager() {};

  static std::map<Event::Type, std::vector<std::function<void(Event)> > > funcs;
};
