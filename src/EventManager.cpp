#include "EventManager.hpp"

#include <iostream>

// Statics
float EventManager::time;

std::map<Event::Type, std::vector<std::function<void(Event)> > >
EventManager::funcs;

void EventManager::Update(float t, float dt)
{
  time = t;
}

void EventManager::Register(Event::Type type, std::function<void(Event)> func)
{
  funcs[type].push_back(func);
}

void EventManager::Send(Event::Type t, boost::any d)
{
  Event e{t};
  e.timestamp = time;
  e.data = d;

  for (auto& func : funcs[e.type])
    func(e);
}

void EventManager::Send(Event::Type t)
{
  Send(t, NULL);
}
