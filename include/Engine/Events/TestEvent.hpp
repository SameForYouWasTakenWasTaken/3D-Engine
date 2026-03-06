#pragma once

#include <string>
#include <Engine/Events.hpp>

class TestEvent : public Event
{
public:
    std::string message;
    TestEvent(const std::string& message) : message(message) {}
    EVENT_TYPE(TestEvent);
};