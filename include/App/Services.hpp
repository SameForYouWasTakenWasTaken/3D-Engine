#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>

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
