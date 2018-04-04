#pragma once

#include <functional>
#include <vector>
#include <map>

#include "Event.hpp"

using namespace std::placeholders;

class EventManager
{
public:
  static void Update(double t, double dt);

  static void Register(Event::Type, std::function<void(Event)>);

  static void Send(Event::Type, boost::any);
  static void Send(Event::Type);
private:
  EventManager() {};

  static double time;
  static std::map<Event::Type, std::vector<std::function<void(Event)> > > funcs;
};
