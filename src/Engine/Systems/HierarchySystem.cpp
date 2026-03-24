#include <Engine/Systems/HierarchySystem.hpp>
#include <ranges>

const HierarchyNode* HierarchySystem::FindInHierarchy(const Hierarchy& hierarchy, entt::entity possible_entity)
{
    auto it = hierarchy.nodes.find(possible_entity);
    if (it == hierarchy.nodes.end())
        return nullptr;

    return &it->second;
}

void HierarchySystem::PutInHierarchy(Hierarchy& hierarchy, entt::entity entity)
{
    auto it = hierarchy.nodes.find(entity);
    if (it != hierarchy.nodes.end())
        return;

    HierarchyNode newNode{};
    newNode.parent = entt::null;

    hierarchy.nodes.emplace(entity, newNode);
}

void HierarchySystem::SetChildren(Hierarchy& hierarchy, entt::entity parent, entt::entity child)
{
    auto parentIt = hierarchy.nodes.find(parent);
    if (parentIt == hierarchy.nodes.end())
        return;

    auto childIt = hierarchy.nodes.find(child);
    if (childIt == hierarchy.nodes.end())
    {
        hierarchy.nodes.emplace(child, HierarchyNode{});
        childIt = hierarchy.nodes.find(child);
    }

    parentIt->second.children.push_back(child);
    childIt->second.parent = parent;
}

void HierarchySystem::SetChildren(Hierarchy& hierarchy, entt::entity parent, const std::vector<entt::entity>& children)
{
    auto parentIt = hierarchy.nodes.find(parent);
    if (parentIt == hierarchy.nodes.end())
        return;

    for (entt::entity child : children)
    {
        auto childIt = hierarchy.nodes.find(child);
        if (childIt == hierarchy.nodes.end())
        {
            hierarchy.nodes.emplace(child, HierarchyNode{});
            childIt = hierarchy.nodes.find(child);
        }

        parentIt->second.children.push_back(child);
        childIt->second.parent = parent;
    }
}

const std::vector<entt::entity>* HierarchySystem::GetChildren(const Hierarchy& hierarchy, entt::entity entity)
{
    auto it = hierarchy.nodes.find(entity);
    if (it == hierarchy.nodes.end())
        return nullptr;

    return &it->second.children;
}

std::vector<entt::entity> HierarchySystem::GetDescendants(const Hierarchy& hierarchy, entt::entity entity)
{
    std::vector<entt::entity> allDescendants;

    const HierarchyNode* node = FindInHierarchy(hierarchy, entity);
    if (node == nullptr)
        return allDescendants;

    for (entt::entity child : node->children)
    {
        allDescendants.push_back(child);

        auto descendants = GetDescendants(hierarchy, child);
        allDescendants.insert(allDescendants.end(), descendants.begin(), descendants.end());
    }

    return allDescendants;
}

std::vector<entt::entity> HierarchySystem::GetAncestors(const Hierarchy& hierarchy, entt::entity entity)
{
    std::vector<entt::entity> ancestors;

    const HierarchyNode* node = FindInHierarchy(hierarchy, entity);
    if (node == nullptr)
        return ancestors;

    entt::entity parent = node->parent;

    while (parent != entt::null)
    {
        ancestors.push_back(parent);

        const HierarchyNode* parentNode = FindInHierarchy(hierarchy, parent);
        if (parentNode == nullptr)
            break;

        parent = parentNode->parent;
    }

    std::ranges::reverse(ancestors);
    return ancestors;
}

entt::entity HierarchySystem::GetParent(const Hierarchy& hierarchy, entt::entity entity)
{
    auto node = FindInHierarchy(hierarchy, entity);
    if (node == nullptr)
        return entt::null;

    return node->parent;
}