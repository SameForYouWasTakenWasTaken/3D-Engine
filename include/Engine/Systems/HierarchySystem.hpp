#pragma once

#include <entt.hpp>
#include <unordered_map>
#include <vector>

struct HierarchyNode
{
    entt::entity parent{entt::null};
    std::vector<entt::entity> children{};
};

struct Hierarchy
{
    std::unordered_map<entt::entity, HierarchyNode> nodes{};
};

class HierarchySystem
{
public:
    HierarchySystem() = default;

    static const std::vector<entt::entity>* GetChildren(const Hierarchy& hierarchy, entt::entity entity);
    static std::vector<entt::entity> GetDescendants(const Hierarchy& hierarchy, entt::entity entity);
    static std::vector<entt::entity> GetAncestors(const Hierarchy& hierarchy, entt::entity entity);

    template <typename T>
    static entt::entity GetClosestParentWithComponent(const Hierarchy& hierarchy, const entt::registry& registry, entt::entity entity);

    static entt::entity GetParent(const Hierarchy& hierarchy, entt::entity entity);

    static const HierarchyNode* FindInHierarchy(const Hierarchy& hierarchy, entt::entity possible_entity);

    static void PutInHierarchy(Hierarchy& hierarchy, entt::entity entity);
    static void SetChildren(Hierarchy& hierarchy, entt::entity parent, entt::entity child);
    static void SetChildren(Hierarchy& hierarchy, entt::entity parent, const std::vector<entt::entity>& children);
};

template <typename T>
entt::entity HierarchySystem::GetClosestParentWithComponent(const Hierarchy& hierarchy, const entt::registry& registry,
    entt::entity entity)
{
    const HierarchyNode* node = FindInHierarchy(hierarchy, entity);
    if (node == nullptr)
        return entt::null;

    entt::entity parent = node->parent;

    while (parent != entt::null)
    {
        if (registry.all_of<T>(parent))
            return parent;

        const HierarchyNode* parentNode = FindInHierarchy(hierarchy, parent);
        if (parentNode == nullptr)
            return entt::null;

        parent = parentNode->parent;
    }

    return entt::null;
}
