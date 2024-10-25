#pragma once

#include <vector>
#include <array>

#include "shapeprimitives.h"

template <typename TLeaf>
concept LeafHasGetPositionVec2D =
    requires(TLeaf leaf)
    {
        { leaf.GetPosition() } -> std::same_as<const glm::vec2&>;
    };

template<class TLeaf, uint32_t SplitThreshold = 4, uint32_t ChildDepthThreshold = 2> requires LeafHasGetPositionVec2D<TLeaf>
class QuadtreeConcept
{
public:
    using Leaf = TLeaf;

    class Branch
    {
    public:
        Branch() = default;
        explicit Branch(uint32_t depth, Rectangle&& rect);
        void AddLeaf(Leaf* newLeaf);
        void Reset();
        static Branch* FindBranch(Branch& branch, const glm::vec2& point);
        void FindBranches(const Rectangle& rect, std::vector<Branch*>& foundBranches);
        void FindLeaves(const Rectangle& rect, std::vector<Leaf*>& foundLeaves) const;
        void SetRect(Rectangle&& rect);
        bool HasBranches() const { return !m_Branches.empty(); }
        const Rectangle& GetRect() const { return m_Rect; }
        const std::vector<Branch>& GetBranches() const { return m_Branches; }
        const std::vector<Leaf*>& GetLeaves() const { return m_Leaves; }
    private:
        std::vector<Branch> m_Branches{};
        std::vector<Leaf*> m_Leaves{};
        Rectangle m_Rect{};
        uint32_t m_Depth{0};
    };

    bool FindLeaves(const Rectangle& rect, std::vector<Leaf*>& foundLeaves) const;
    bool FindBranches(const Rectangle& rect, std::vector<Branch*>& foundBranches);
    const Branch& GetRootBranch() const { return m_RootBranch; }
    Branch* FindBranch(const glm::vec2& point);
    void AddLeaf(Leaf* newLeaf);
    void Reset();
private:
    Branch m_RootBranch{};
};

template<class TQuadtree>
void RebuildQuadtreeConcept(TQuadtree& quadtree, std::vector<typename TQuadtree::Leaf>& leaves)
{
    quadtree.Reset();

    for(typename TQuadtree::Leaf& leaf : leaves)
    {
        quadtree.AddLeaf(&leaf);
    }
}

template<class TLeaf, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires LeafHasGetPositionVec2D<TLeaf>
QuadtreeConcept<TLeaf, SplitThreshold, ChildDepthThreshold>::Branch::Branch(const uint32_t depth, Rectangle&& rect)
    : m_Depth{depth}
    , m_Rect{std::move(rect)}
{
}

template<class TLeaf, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires LeafHasGetPositionVec2D<TLeaf>
void QuadtreeConcept<TLeaf, SplitThreshold, ChildDepthThreshold>::Branch::AddLeaf(TLeaf* const newLeaf)
{
    if(m_Depth > ChildDepthThreshold || SplitThreshold > m_Leaves.size())
    {
        m_Leaves.push_back(newLeaf);
        return;
    }

    m_Branches.reserve(4);

    const float_t width{m_Rect.GetWidth() * 0.5f};
    const float_t height{m_Rect.GetHeight() * 0.5f};
    const glm::vec2& topLeft{m_Rect.GetTopLeft()};

    // Top Left
    m_Branches.emplace_back(m_Depth + 1, Rectangle{topLeft, width, height});
    // Top Right
    m_Branches.emplace_back(m_Depth + 1, Rectangle{topLeft + glm::vec2{width, 0.0f}, width, height});
    // Bottom Left
    m_Branches.emplace_back(m_Depth + 1, Rectangle{topLeft + glm::vec2{0.0f, height}, width, height});
    // Bottom Right
    m_Branches.emplace_back(m_Depth + 1, Rectangle{topLeft + glm::vec2{width, height}, width, height});

    {
        Branch* const branch{FindBranch(*this, newLeaf->GetPosition())};
        branch->AddLeaf(newLeaf);
    }

    for(TLeaf* const leaf : m_Leaves)
    {
        Branch* const branch{FindBranch(*this, leaf->GetPosition())};
        branch->AddLeaf(leaf);
    }

    m_Leaves.clear();
}

template<class TLeaf, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires LeafHasGetPositionVec2D<TLeaf>
void QuadtreeConcept<TLeaf, SplitThreshold, ChildDepthThreshold>::Branch::Reset()
{
    m_Depth = 0;
    m_Rect = {};
    m_Leaves.clear();
    m_Branches.clear();
}

template<class TLeaf, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires LeafHasGetPositionVec2D<TLeaf>
void QuadtreeConcept<TLeaf, SplitThreshold, ChildDepthThreshold>::Branch::SetRect(Rectangle&& rect)
{
    m_Rect = std::move(rect);
}

template<class TLeaf, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires LeafHasGetPositionVec2D<TLeaf>
QuadtreeConcept<TLeaf, SplitThreshold, ChildDepthThreshold>::Branch* QuadtreeConcept<TLeaf, SplitThreshold, ChildDepthThreshold>::Branch::FindBranch(
    Branch& branch, const glm::vec2& point)
{
    if(CollisionRectPoint(branch.m_Rect, point))
    {
        for(Branch& childBranch : branch.m_Branches)
        {
            if(Branch* const foundBranch{FindBranch(childBranch, point)})
            {
                return foundBranch;
            }
        }

        return &branch;
    }

    return nullptr;
}

template<class TLeaf, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires LeafHasGetPositionVec2D<TLeaf>
void QuadtreeConcept<TLeaf, SplitThreshold, ChildDepthThreshold>::Branch::FindBranches(
    const Rectangle& rect, std::vector<Branch*>& foundBranches)
{
    if(!HasBranches() && CollisionRectRect(rect, m_Rect))
    {
        foundBranches.push_back(this);
        return;
    }

    for(Branch& childBranch : m_Branches)
    {
        childBranch.FindBranches(rect, foundBranches);
    }
}

template<class TLeaf, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires LeafHasGetPositionVec2D<TLeaf>
void QuadtreeConcept<TLeaf, SplitThreshold, ChildDepthThreshold>::Branch::FindLeaves(
    const Rectangle& rect, std::vector<TLeaf*>& foundLeaves) const
{
    if(!HasBranches() && CollisionRectRect(rect, m_Rect))
    {
        foundLeaves.insert(std::end(foundLeaves), std::begin(m_Leaves), std::end(m_Leaves));
        return;
    }

    for(const Branch& childBranch : m_Branches)
    {
        childBranch.FindLeaves(rect, foundLeaves);
    }
}

template<class TLeaf, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires LeafHasGetPositionVec2D<TLeaf>
void QuadtreeConcept<TLeaf, SplitThreshold, ChildDepthThreshold>::Reset()
{
    m_RootBranch.Reset();
    m_RootBranch.SetRect(Rectangle{glm::vec2{0.0f, 0.0f}, static_cast<float_t>(WINDOW_WIDTH), static_cast<float_t>(WINDOW_HEIGHT)});
}

template<class TLeaf, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires LeafHasGetPositionVec2D<TLeaf>
QuadtreeConcept<TLeaf, SplitThreshold, ChildDepthThreshold>::Branch* QuadtreeConcept<TLeaf, SplitThreshold, ChildDepthThreshold>::FindBranch(
    const glm::vec2& point)
{
    return Branch::FindBranch(m_RootBranch, point);
}

template<class TLeaf, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires LeafHasGetPositionVec2D<TLeaf>
bool QuadtreeConcept<TLeaf, SplitThreshold, ChildDepthThreshold>::FindBranches(
    const Rectangle& rect, std::vector<Branch*>& foundBranches)
{
    assert(foundBranches.empty());
    m_RootBranch.FindBranches(rect, foundBranches);
    return !foundBranches.empty();
}

template<class TLeaf, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires LeafHasGetPositionVec2D<TLeaf>
bool QuadtreeConcept<TLeaf, SplitThreshold, ChildDepthThreshold>::FindLeaves(
    const Rectangle& rect, std::vector<TLeaf*>& foundLeaves) const
{
    assert(foundLeaves.empty());
    m_RootBranch.FindLeaves(rect, foundLeaves);
    return !foundLeaves.empty();
}

template<class TLeaf, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires LeafHasGetPositionVec2D<TLeaf>
void QuadtreeConcept<TLeaf, SplitThreshold, ChildDepthThreshold>::AddLeaf(TLeaf* const newLeaf)
{
    Branch* const branch{FindBranch(newLeaf->GetPosition())};
    assert(branch);
    branch->AddLeaf(newLeaf);
}
