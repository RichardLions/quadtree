#pragma once

#include <vector>
#include <array>

#include "shapeprimitives.h"

template <typename T>
concept HasGetPositionVec2D =
    requires(T type)
    {
        { type.GetPosition() } -> std::same_as<const glm::vec2&>;
    };

template<class TPoint, uint32_t SplitThreshold = 4, uint32_t ChildDepthThreshold = 2> requires HasGetPositionVec2D<TPoint>
class QuadtreeConcept
{
public:
    using Point = TPoint;

    class Node
    {
    public:
        Node() = default;
        explicit Node(uint32_t depth);
        void AddPoint(TPoint* newPoint);
        void Reset();
        static Node* FindNode(Node* node, const glm::vec2& point);
        void FindNodes(const Rectangle& rect, std::vector<Node*>& foundNodes);
        void FindPoints(const Rectangle& rect, std::vector<TPoint*>& foundPoints) const;
        void SetRect(Rectangle&& rect);
        const Rectangle& GetRect() const { return m_Rect; }
        const std::vector<Node>& GetChildren() const { return m_Children; }
        const std::vector<TPoint*>& GetPoints() const { return m_Points; }
    private:
        std::vector<Node> m_Children{};
        std::vector<TPoint*> m_Points{};
        Rectangle m_Rect{};
        uint32_t m_Depth{0};
    };

    bool FindPoints(const Rectangle& rect, std::vector<TPoint*>& foundPoints) const;
    bool FindNodes(const Rectangle& rect, std::vector<Node*>& foundNodes);
    const Node& GetRootNode() const { return m_RootNode; }
    Node* FindNode(const glm::vec2& point);
    void AddPoint(TPoint* newPoint);
    void Reset();
private:
    Node m_RootNode{0};
};

template<class TQuadtree>
void RebuildQuadtreeConcept(TQuadtree& quadtree, std::vector<typename TQuadtree::Point>& points)
{
    quadtree.Reset();

    for(typename TQuadtree::Point& point : points)
    {
        quadtree.AddPoint(&point);
    }
}

template<class TPoint, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires HasGetPositionVec2D<TPoint>
QuadtreeConcept<TPoint, SplitThreshold, ChildDepthThreshold>::Node::Node(const uint32_t depth)
    : m_Depth{depth}
{
}

template<class TPoint, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires HasGetPositionVec2D<TPoint>
void QuadtreeConcept<TPoint, SplitThreshold, ChildDepthThreshold>::Node::AddPoint(TPoint* const newPoint)
{
    if(m_Depth > ChildDepthThreshold || SplitThreshold > m_Points.size())
    {
        m_Points.push_back(newPoint);
        return;
    }

    m_Children.reserve(4);

    const float_t width{m_Rect.GetWidth() * 0.5f};
    const float_t height{m_Rect.GetHeight() * 0.5f};

    Node topLeft{m_Depth + 1};
    topLeft.m_Rect = Rectangle{m_Rect.GetTopLeft(), width, height};
    m_Children.push_back(std::move(topLeft));

    Node topRight{m_Depth + 1};
    topRight.m_Rect = Rectangle{m_Rect.GetTopLeft() + glm::vec2{width, 0.0f}, width, height};
    m_Children.push_back(std::move(topRight));

    Node bottomLeft{m_Depth + 1};
    bottomLeft.m_Rect = Rectangle{m_Rect.GetTopLeft() + glm::vec2{0.0f, height}, width, height};
    m_Children.push_back(std::move(bottomLeft));

    Node bottomRight{m_Depth + 1};
    bottomRight.m_Rect = Rectangle{m_Rect.GetTopLeft() + glm::vec2{width, height}, width, height};
    m_Children.push_back(std::move(bottomRight));

    {
        Node* node{FindNode(this, newPoint->m_Position)};
        node->AddPoint(newPoint);
    }

    for(TPoint* const point : m_Points)
    {
        Node* node{FindNode(this, point->m_Position)};
        node->AddPoint(point);
    }

    m_Points = {};
}

template<class TPoint, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires HasGetPositionVec2D<TPoint>
void QuadtreeConcept<TPoint, SplitThreshold, ChildDepthThreshold>::Node::Reset()
{
    m_Depth = 0;
    m_Rect = {};
    m_Points.clear();
    m_Children.clear();
}

template<class TPoint, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires HasGetPositionVec2D<TPoint>
void QuadtreeConcept<TPoint, SplitThreshold, ChildDepthThreshold>::Node::SetRect(Rectangle&& rect)
{
    m_Rect = std::move(rect);
}

template<class TPoint, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires HasGetPositionVec2D<TPoint>
QuadtreeConcept<TPoint, SplitThreshold, ChildDepthThreshold>::Node* QuadtreeConcept<TPoint, SplitThreshold, ChildDepthThreshold>::Node::FindNode(
    Node* const node, const glm::vec2& point)
{
    if(CollisionRectPoint(node->m_Rect, point))
    {
        for(Node& childNode : node->m_Children)
        {
            if(Node* foundNode{FindNode(&childNode, point)})
            {
                return foundNode;
            }
        }

        return node;
    }

    return nullptr;
}

template<class TPoint, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires HasGetPositionVec2D<TPoint>
void QuadtreeConcept<TPoint, SplitThreshold, ChildDepthThreshold>::Node::FindNodes(
    const Rectangle& rect, std::vector<Node*>& foundNodes)
{
    if(!m_Points.empty())
    {
        if(CollisionRectRect(rect, m_Rect))
        {
            foundNodes.push_back(this);
        }
    }

    for(Node& node : m_Children)
    {
        node.FindNodes(rect, foundNodes);
    }
}

template<class TPoint, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires HasGetPositionVec2D<TPoint>
void QuadtreeConcept<TPoint, SplitThreshold, ChildDepthThreshold>::Node::FindPoints(
    const Rectangle& rect, std::vector<TPoint*>& foundPoints) const
{
    if(!m_Points.empty())
    {
        if(CollisionRectRect(rect, m_Rect))
        {
            foundPoints.insert(std::end(foundPoints), std::begin(m_Points), std::end(m_Points));
        }
    }

    for(const Node& node : m_Children)
    {
        node.FindPoints(rect, foundPoints);
    }
}

template<class TPoint, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires HasGetPositionVec2D<TPoint>
void QuadtreeConcept<TPoint, SplitThreshold, ChildDepthThreshold>::Reset()
{
    m_RootNode.Reset();
    m_RootNode.SetRect(Rectangle{glm::vec2{0.0f, 0.0f}, static_cast<float_t>(WINDOW_WIDTH), static_cast<float_t>(WINDOW_HEIGHT)});
}

template<class TPoint, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires HasGetPositionVec2D<TPoint>
QuadtreeConcept<TPoint, SplitThreshold, ChildDepthThreshold>::Node* QuadtreeConcept<TPoint, SplitThreshold, ChildDepthThreshold>::FindNode(
    const glm::vec2& point)
{
    return Node::FindNode(&m_RootNode, point);
}

template<class TPoint, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires HasGetPositionVec2D<TPoint>
bool QuadtreeConcept<TPoint, SplitThreshold, ChildDepthThreshold>::FindNodes(
    const Rectangle& rect, std::vector<Node*>& foundNodes)
{
    assert(foundNodes.empty());
    m_RootNode.FindNodes(rect, foundNodes);
    return foundNodes.size();
}

template<class TPoint, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires HasGetPositionVec2D<TPoint>
bool QuadtreeConcept<TPoint, SplitThreshold, ChildDepthThreshold>::FindPoints(
    const Rectangle& rect, std::vector<TPoint*>& foundPoints) const
{
    assert(foundPoints.empty());
    m_RootNode.FindPoints(rect, foundPoints);
    return foundPoints.size();
}

template<class TPoint, uint32_t SplitThreshold, uint32_t ChildDepthThreshold> requires HasGetPositionVec2D<TPoint>
void QuadtreeConcept<TPoint, SplitThreshold, ChildDepthThreshold>::AddPoint(TPoint* const newPoint)
{
    Node* const node{FindNode(newPoint->m_Position)};
    assert(node);
    node->AddPoint(newPoint);
}
