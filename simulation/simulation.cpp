#include "stdafx.h"
#include "simulation.h"

#include <ranges>

void UpdateCirclesQuadtree(std::vector<Circle>& circles, Quadtree& quadtree, const float_t delta)
{
    std::vector<Quadtree::Node*> nodes{};
    for(Circle& circle : circles)
    {
        nodes.clear();
        const float_t widthHeight{circle.m_Radius * 2.0f};
        const Rectangle circleAprox{circle.m_Position - glm::vec2{circle.m_Radius, circle.m_Radius}, widthHeight, widthHeight};
        quadtree.FindNodes(circleAprox, nodes);

        for(Quadtree::Node* const node : nodes)
        {
            for(Circle* const otherCircle : node->GetPoints())
            {
                if(&circle == otherCircle)
                    continue;

                ResolveElasticCollisionCircleCircle(circle, *otherCircle);
            }
        }

        circle.m_Position += circle.m_Velocity * delta;
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
