#include "EventManager.hpp"

#include <iostream>

std::map<Event::Type, std::vector<std::function<void(Event)> > >
EventManager::funcs;

void EventManager::Register(Event::Type type, std::function<void(Event)> func)
{
  funcs[type].push_back(func);
}

void EventManager::Send(Event e)
{
  for (auto& func : funcs[e.type]) {
    func(e);
  }
}

void EventManager::Send(Event::Type t)
{
  Event e{t};

  for (auto& func : funcs[t]) {
    func(e);
  }
}
