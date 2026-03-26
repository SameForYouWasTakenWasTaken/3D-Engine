#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>
#include <utility>
#include <typeinfo>

// @param seed the current combined hash
/**
 * @brief Mixes a new hash into an existing seed to update a combined hash value.
 *
 * Mutates `seed` in-place by incorporating `hash`, producing a new combined hash suitable
 * for incremental hashing of multiple values.
 *
 * @param seed Reference to the running hash seed to update.
 * @param hash The hash value to mix into `seed`.
 */
inline void HashCombine(std::size_t& seed, std::size_t hash)
{
    seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename T = std::size_t, typename... Args>
/**
 * @brief Computes a combined hash value from the provided arguments.
 *
 * @tparam T Return type for the combined hash (defaults to `std::size_t`).
 * @tparam Args Types of values to include in the combined hash.
 * @param args Values whose individual hashes will be combined into a single hash.
 * @return T Combined hash of `args`, cast to `T`.
 */
T Hash(const Args&... args)
{
    std::size_t seed = 0;

    (HashCombine(seed, std::hash<std::decay_t<Args>>{}(args)), ...);

    return static_cast<T>(seed);
}

class IService
{
public:
    /**
 * @brief Virtual destructor for IService.
 *
 * Ensures that derived service destructors are invoked when an instance is deleted through
 * a pointer to the IService base class.
 */
virtual ~IService() = default;
};

class Services final
{
    std::unordered_map<std::type_index, std::unique_ptr<IService>> m_Services;
public:
    /**
 * @brief Constructs an empty Services registry.
 *
 * Initializes an instance that holds no registered services.
 */
explicit Services() = default;
    /**
 * @brief Disable copy construction to prevent copying the Services registry.
 *
 */
Services(const Services&) = delete;
    /**
 * @brief Disables copy-assignment for Services to prevent copying of the registry.
 */
Services& operator=(const Services&) = delete;

    static Services& Get();

    template <typename T, typename ... Args>
    /**
     * @brief Registers a service instance of type `T` into the registry.
     *
     * Constructs a `T` in-place from the provided constructor arguments and stores it
     * keyed by `typeid(T)`. If a service of the same type is already registered,
     * no replacement occurs and an exception is thrown.
     *
     * @tparam T Service type to register.
     * @tparam Args Constructor argument types for `T`.
     * @param args Forwarded constructor arguments used to construct the `T` instance.
     * @throws std::runtime_error if a service of type `T` is already registered.
     *
     * Requirements: `T` must be constructible from `Args...` and the resulting type
     * must be compatible with being owned as an `IService` (convertible to `IService*`).
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
     * @brief Retrieves a registered service instance by type.
     *
     * @tparam T Concrete service type to retrieve; must have been previously registered.
     * @return T& Reference to the stored service instance of type `T`.
     * @throws std::runtime_error if no service of type `T` is registered.
     */
    [[nodiscard]] T& GetService() const
    {
        auto it = m_Services.find(typeid(T));

        if (it == m_Services.end())
            throw std::runtime_error("Service not registered!");

        return *static_cast<T*>(it->second.get());
    }

    template <typename T>
    /**
     * @brief Checks whether a service of type T is registered.
     *
     * @tparam T Service type to check for.
     * @return `true` if a service of type `T` is registered, `false` otherwise.
     */
    [[nodiscard]] bool ServiceExists() const
    {
        auto it = m_Services.find(typeid(T));

        if (it == m_Services.end())
            return false;
        return true;
    }

};
