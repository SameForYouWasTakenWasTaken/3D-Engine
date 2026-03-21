#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>
#include <utility>
#include <typeinfo>

// @param seed the current combined hash
// @param hash the new hash to mix in
inline void HashCombine(std::size_t& seed, std::size_t hash)
{
    seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename T = std::size_t, typename... Args>
T Hash(const Args&... args)
{
    std::size_t seed = 0;

    (HashCombine(seed, std::hash<std::decay_t<Args>>{}(args)), ...);

    return static_cast<T>(seed);
}

class IService
{
public:
    virtual ~IService() = default;
};

class Services final
{
    std::unordered_map<std::type_index, std::unique_ptr<IService>> m_Services;
public:
    explicit Services() = default;
    Services(const Services&) = delete;
    Services& operator=(const Services&) = delete;

    static Services& Get();

    template <typename T, typename ... Args>
    void RegisterService(Args&&... args)
    {
        const auto id = std::type_index(typeid(T));

        if (m_Services.contains(id))
            throw std::runtime_error("Service already registered!");

        m_Services[id] = std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    [[nodiscard]] T& GetService() const
    {
        auto it = m_Services.find(typeid(T));

        if (it == m_Services.end())
            throw std::runtime_error("Service not registered!");

        return *static_cast<T*>(it->second.get());
    }

    template <typename T>
    [[nodiscard]] bool ServiceExists() const
    {
        auto it = m_Services.find(typeid(T));

        if (it == m_Services.end())
            return false;
        return true;
    }

};
