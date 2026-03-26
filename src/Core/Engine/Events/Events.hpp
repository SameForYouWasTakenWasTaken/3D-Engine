#pragma once
#include <cstddef>

// Removes most of boilerplate
#define EVENT_TYPE(type)                                            \
    static std::size_t GetStaticType()                              \
    {                                                               \
        return Event::GetStaticType<type>();                         \
    }                                                               \
    std::size_t GetType() const override                            \
    {                                                               \
        return GetStaticType();                                     \
    }

class Event
{
public:
    bool Handled = false;

    virtual ~Event() = default;
    virtual std::size_t GetType() const = 0;

private:
    // Central ID generator
    static std::size_t GenerateTypeID()
    {
        static std::size_t counter = 0;
        return counter++;
    }

protected:
    // One unique ID per type
    template<typename T>
    static std::size_t GetStaticType()
    {
        static std::size_t id = GenerateTypeID();
        return id;
    }
};