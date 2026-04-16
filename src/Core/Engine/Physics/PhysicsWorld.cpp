#include "PhysicsWorld.hpp"

#include "Engine/Components/Transform.hpp"
#include "Engine/Components/Physics/Collider.hpp"
#include "Engine/Components/Physics/Rigidbody.hpp"

constexpr float DEFAULT_STEP_INTERVAL = 60.f;

void Sanitize(glm::vec3& v)
{
    if (!std::isfinite(v.x) || !std::isfinite(v.y) || !std::isfinite(v.z))
    {
        v = glm::vec3(0.0f);
    }
}

void PhysicsWorld::ApplyForces(float dt)
{
    auto bodies = m_SceneRegistry.view<COMPRigidBody, COMPTransform>();

    bodies.each([&](COMPRigidBody& body, COMPTransform& transform){
        if (body.isStatic)
            return;

        body.linearVelocity += -Gravity * dt;
    });
}

void PhysicsWorld::IntegrateVelocities(float dt)
{
    auto bodies = m_SceneRegistry.view<COMPRigidBody, COMPTransform>();

    bodies.each([&](COMPRigidBody& body, COMPTransform& transform){
        if (body.isStatic)
            return;

        Sanitize(body.linearVelocity);
        Sanitize(transform.LocalPosition);

        transform.Move(body.linearVelocity * dt);
    });
}

void PhysicsWorld::DetectCollisions()
{
    m_Contacts.clear();

    auto view = m_SceneRegistry.view<COMPRigidBody, COMPTransform, COMPCollider>();
    for (auto itA = view.begin(); itA != view.end(); ++itA)
    {
        auto entityA = *itA;
        auto& transformA = view.get<COMPTransform>(entityA);
        auto& colliderA = view.get<COMPCollider>(entityA);
        auto& bodyA = view.get<COMPRigidBody>(entityA);

        PhysicsBody physA{entityA, bodyA, transformA, colliderA};

        for (auto itB = std::next(itA); itB != view.end(); ++itB)
        {
            auto entityB = *itB;


            auto& transformB = view.get<COMPTransform>(entityB);
            auto& colliderB = view.get<COMPCollider>(entityB);
            auto& bodyB = view.get<COMPRigidBody>(entityB);

            // Skip static-static
            if (bodyA.isStatic && bodyB.isStatic)
                continue;

            PhysicsBody physB{entityB, bodyB, transformB, colliderB};
            HandleAABB(physA, physB);
        }
    }
}

void PhysicsWorld::ResolveContacts(float dt)
{

    for (const auto& contact : m_Contacts)
    {
        auto& transformA = m_SceneRegistry.get<COMPTransform>(contact.a);
        auto& transformB = m_SceneRegistry.get<COMPTransform>(contact.b);

        auto& bodyA = m_SceneRegistry.get<COMPRigidBody>(contact.a);
        auto& bodyB = m_SceneRegistry.get<COMPRigidBody>(contact.b);

        // Inverse Mass
        float invMassA = bodyA.isStatic ? 0.0f : 1.0f / std::max(bodyA.mass, 0.0001f);
        float invMassB = bodyB.isStatic ? 0.0f : 1.0f / std::max(bodyB.mass, 0.0001f);

        float invMassSum = invMassA + invMassB;

        if (invMassSum <= 0.0f)
            continue;

        // 1. positional correction
        constexpr float percent = 0.2f;  // prevents jitter
        constexpr float slop = 0.001f;    // allow tiny penetration
        float correction = glm::max(contact.penetration - slop, 0.0f) * percent;
        glm::vec3 correctionVec = contact.normal * (correction / invMassSum);

        if (!bodyA.isStatic)
            transformA.Move({
                -correctionVec.x * invMassA,
                -correctionVec.y * invMassA,
                -correctionVec.z * invMassA});

        if (!bodyB.isStatic)
            transformB.Move({
                correctionVec.x * invMassB,
                correctionVec.y * invMassB,
                correctionVec.z * invMassB});

        // 2. impulse resolution
        glm::vec3 relativeVelocity = bodyB.linearVelocity - bodyA.linearVelocity;
        float velocityAlongNormal = glm::dot(relativeVelocity, contact.normal);

        if (velocityAlongNormal > 0.0f)
            continue;

        float restitution = 0.0f; // no bounce for now

        float j = -(1.0f + restitution) * velocityAlongNormal;
        j /= invMassSum;

        glm::vec3 impulse = j * contact.normal;

        if (!bodyA.isStatic)
            bodyA.linearVelocity -= impulse * invMassA;

        if (!bodyB.isStatic)
            bodyB.linearVelocity += impulse * invMassB;
    }
}

void PhysicsWorld::SyncTransforms()
{

}

void PhysicsWorld::HandleAABB(PhysicsBody& PhysBodyA, PhysicsBody& PhysBodyB)
{
    // If neither are AABB, then skip
    if (PhysBodyA.Collider.type != ColliderType::AABB && PhysBodyB.Collider.type != ColliderType::AABB)
        return;

    auto& bodyA = PhysBodyA.RigidBody;
    auto& bodyB = PhysBodyB.RigidBody;

    auto& transformA = PhysBodyA.Transform;
    auto& transformB = PhysBodyB.Transform;

    auto& colliderA = PhysBodyA.Collider;
    auto& colliderB = PhysBodyB.Collider;

    glm::vec3 centerA = glm::vec3(transformA.WorldMatrix[3]) + colliderA.centerOffset;
    glm::vec3 halfA = colliderA.halfExtents * transformA.LocalScale;

    glm::vec3 centerB = glm::vec3(transformB.WorldMatrix[3]) + colliderB.centerOffset;
    glm::vec3 halfB = colliderB.halfExtents * transformB.LocalScale;

    glm::vec3 delta = centerB - centerA;

    glm::vec3 overlap = halfA + halfB - glm::abs(delta);

    float penetration = overlap.x;
    // TODO: normal is axis-aligned, shouldn't be the case for spheres
    glm::vec3 normal = { delta.x < 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f };

    if (overlap.y < penetration)
    {
        penetration = overlap.y;
        normal = { 0.0f, delta.y < 0.0f ? -1.0f : 1.0f, 0.0f };
    }

    if (overlap.z < penetration)
    {
        penetration = overlap.z;
        normal = { 0.0f, 0.0f, delta.z < 0.0f ? -1.0f : 1.0f };
    }

    if (overlap.x > 0 && overlap.y > 0 && overlap.z > 0)
        m_Contacts.emplace_back(Contact{
            PhysBodyA.entity,
            PhysBodyB.entity,
            normal,
            penetration
        });
}

void PhysicsWorld::Step(float dt)
{
    float step_interval = 1.f / DEFAULT_STEP_INTERVAL;

    ApplyForces(step_interval);
    IntegrateVelocities(step_interval);

    for (int i{}; i < 4; i++)
    {
        DetectCollisions();
        ResolveContacts(step_interval);
    }

    SyncTransforms();
}
