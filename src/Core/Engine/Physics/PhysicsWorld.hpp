#pragma once
#include "Engine/Components/Transform.hpp"
#include "Engine/Components/Physics/Collider.hpp"
#include "Engine/Components/Physics/Rigidbody.hpp"
#include "glm/vec3.hpp"
#include "vendor/entt.hpp"

class PhysicsWorld final
{
    struct PhysicsBody
    {
        entt::entity entity{entt::null};
        COMPRigidBody RigidBody;
        COMPTransform Transform;
        COMPCollider Collider;

        PhysicsBody(entt::entity entity, COMPRigidBody& body, COMPTransform& transform, COMPCollider& collider)
            : entity(entity), RigidBody(body), Transform(transform), Collider(collider) {}
    };
    entt::registry& m_SceneRegistry;

    void ApplyForces(float dt);
    void IntegrateVelocities(float dt);
    void DetectCollisions();
    void ResolveContacts(float dt);
    void SyncTransforms();

    void ContactAABB(PhysicsBody& PhysBodyA, PhysicsBody& PhysBodyB);

private:

    void HandleAABB(PhysicsBody& PhysBodyA, PhysicsBody& PhysBodyB);

private:

    std::vector<Contact> m_Contacts{};
public:
    PhysicsWorld(entt::registry& scene_registry) : m_SceneRegistry(scene_registry) {}
    ~PhysicsWorld() = default;

    PhysicsWorld(PhysicsWorld&&) = delete;
    PhysicsWorld(const PhysicsWorld&) = delete;
    PhysicsWorld& operator=(const PhysicsWorld& other) = delete;

    void Step(float dt);
public:
    glm::vec3 Gravity = {0.f, 5.f, 0.f}; // --> m/s^2
};