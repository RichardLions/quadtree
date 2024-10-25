#include "stdafx.h"
#include "simulation.h"

#include <ranges>

void UpdateCirclesQuadtreeFoundLeaves(std::vector<Circle>& circles, Quadtree& quadtree, const float_t delta)
{
    std::vector<Quadtree::Leaf*> foundLeaves{};
    for(Circle& circle : circles)
    {
        foundLeaves.clear();
        const float_t widthHeight{circle.m_Radius * 2.0f};
        const Rectangle circleAprox{circle.m_Position - glm::vec2{circle.m_Radius, circle.m_Radius}, widthHeight, widthHeight};
        quadtree.FindLeaves(circleAprox, foundLeaves);

        for(Circle* const otherCircle : foundLeaves)
        {
            if(&circle == otherCircle)
                continue;

            ResolveElasticCollisionCircleCircle(circle, *otherCircle);
        }

        circle.m_Position += circle.m_Velocity * delta;
    }
}

void UpdateCirclesQuadtreeFoundBranches(std::vector<Circle>& circles, Quadtree& quadtree, const float_t delta)
{
    std::vector<Quadtree::Branch*> foundBranches{};
    for(Circle& circle : circles)
    {
        foundBranches.clear();
        const float_t widthHeight{circle.m_Radius * 2.0f};
        const Rectangle circleAprox{circle.m_Position - glm::vec2{circle.m_Radius, circle.m_Radius}, widthHeight, widthHeight};
        quadtree.FindBranches(circleAprox, foundBranches);

        for(Quadtree::Branch* const branch : foundBranches)
        {
            for(Circle* const otherCircle : branch->GetLeaves())
            {
                if(&circle == otherCircle)
                    continue;

                ResolveElasticCollisionCircleCircle(circle, *otherCircle);
            }
        }

        circle.m_Position += circle.m_Velocity * delta;
    }
}

void UpdateCirclesQuadtreeInnerLoop(Quadtree& quadtree, const Quadtree::Branch& branch, float_t delta)
{
    std::vector<Quadtree::Branch*> foundBranches{};
    for(Circle* const circle : branch.GetLeaves())
    {
        const float_t widthHeight{circle->m_Radius * 2.0f};
        const Rectangle circleAprox{circle->m_Position - glm::vec2{circle->m_Radius, circle->m_Radius}, widthHeight, widthHeight};

        if(CollisionRectWithinRect(circleAprox, branch.GetRect()))
        {
            for(Circle* const otherCircle : branch.GetLeaves())
            {
                if(circle == otherCircle)
                    continue;

                ResolveElasticCollisionCircleCircle(*circle, *otherCircle);
            }
        }
        else
        {
            foundBranches.clear();
            quadtree.FindBranches(circleAprox, foundBranches);
            for(Quadtree::Branch* const foundBranch : foundBranches)
            {
                for(Circle* const otherCircle : foundBranch->GetLeaves())
                {
                    if(circle == otherCircle)
                        continue;

                    ResolveElasticCollisionCircleCircle(*circle, *otherCircle);
                }
            }
        }

        circle->m_Position += circle->m_Velocity * delta;
    }

    for(const Quadtree::Branch& childBranch : branch.GetBranches())
    {
        UpdateCirclesQuadtreeInnerLoop(quadtree, childBranch, delta);
    }
}

void UpdateCirclesBruteForce(std::vector<Circle>& circles, const float_t delta)
{
    const uint32_t size{static_cast<uint32_t>(circles.size())};
    for(uint32_t i{0}; i != size; ++i)
    {
        Circle& circle{circles[i]};

        for(Circle& otherCircle : std::ranges::drop_view{circles, i})
        {
            if(&circle == &otherCircle)
                continue;

            ResolveElasticCollisionCircleCircle(circle, otherCircle);
        }

        circle.m_Position += circle.m_Velocity * delta;
    }
}
