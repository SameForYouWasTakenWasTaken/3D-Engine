#pragma once
#include "vendor/entt.hpp"

#include "Rigidbody.hpp"

struct COMPPhysicsBody
{
    entt::entity entity{entt::null};
    COMPRigidBody rigidBody;
};
