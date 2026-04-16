#pragma once
#include "Engine/Components/Transform.hpp"
#include "Engine/Components/Physics/Collider.hpp"
#include "Engine/Components/Physics/Rigidbody.hpp"
#include "vendor/entt.hpp"

struct PhysicsBody
{
    entt::entity entity{entt::null};
    COMPRigidBody RigidBody;
    COMPTransform Transform;
    COMPCollider Collider;

    PhysicsBody(entt::entity entity, COMPRigidBody& body, COMPTransform& transform, COMPCollider& collider)
        : entity(entity), RigidBody(body), Transform(transform), Collider(collider) {}
};
