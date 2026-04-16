#pragma once
#include "glm/vec3.hpp"
#include "vendor/entt.hpp"

enum class ColliderType
{
    AABB = 0,
    Sphere,
};

struct COMPCollider
{
    ColliderType type = ColliderType::AABB;

    // AABB
    glm::vec3 halfExtents{.5f};
    glm::vec3 centerOffset{0.f};

    // Sphere
    float radius = 1;
};

struct Contact
{
    entt::entity a{entt::null};
    entt::entity b{entt::null};

    glm::vec3 normal{0.0f};   // direction from A to B
    float penetration = 0.0f;
};