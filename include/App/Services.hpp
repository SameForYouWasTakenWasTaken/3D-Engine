#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>

class IService
{
public:
    /**
 * @brief Ensures derived service objects are destroyed correctly through base pointers.
 *
 * Declares a virtual default destructor so that deleting an IService pointer
 * invokes the appropriate derived-class destructor and releases resources.
 */
virtual ~IService() = default;
};

class Services final
{
    std::unordered_map<std::type_index, std::unique_ptr<IService>> m_Services;
public:
    /**
 * @brief Constructs an empty service registry.
 *
 * Initializes a Services instance with no registered services.
 */
explicit Services() = default;
    /**
 * @brief Deleted copy constructor to prevent copying of Services instances.
 *
 * Disables copy construction to enforce non-copyable (singleton-like) semantics.
 */
Services(const Services&) = delete;
    /**
 * @brief Disable copy assignment to prevent copying of Services instances.
 *
 * This enforces non-copyable semantics (intended for singleton/global usage).
 */
Services& operator=(const Services&) = delete;

    static Services& Get();

    template <typename T, typename ... Args>
    /**
     * @brief Construct and register a service instance of type `T`.
     *
     * Creates a new `T` using the provided constructor arguments and stores it in
     * the registry keyed by `typeid(T)`.
     *
     * @tparam T Concrete service type to register.
     * @tparam Args Variadic constructor argument types for `T`.
     * @param args Constructor arguments forwarded to `T`'s constructor.
     * @throws std::runtime_error if a service of type `T` is already registered.
     */
    void RegisterService(Args&&... args)
    {
        const auto id = std::type_index(typeid(T));

        if (m_Services.contains(id))
            throw std::runtime_error("Service already registered!");

        m_Services[id] = std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    /**
     * @brief Retrieve a registered service instance by its concrete type.
     *
     * @tparam T Concrete service type previously registered with RegisterService.
     * @return T& Reference to the stored service instance of type `T`.
     * @throws std::runtime_error If a service of type `T` is not registered.
     */
    T& GetService() const
    {
        auto it = m_Services.find(typeid(T));

        if (it == m_Services.end())
            throw std::runtime_error("Service not registered!");

        return *static_cast<T*>(it->second.get());
    }

    template <typename T>
    /**
     * @brief Checks whether a service of the specified type is registered.
     *
     * @tparam T Concrete service type to check for.
     * @return `true` if a service of type `T` is registered, `false` otherwise.
     */
    bool ServiceExists() const
    {
        auto it = m_Services.find(typeid(T));

        if (it == m_Services.end())
            return false;
        return true;
    }

};
