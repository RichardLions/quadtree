#pragma once

#include <random/random.h>

inline constexpr uint32_t WINDOW_WIDTH{1920};
inline constexpr uint32_t WINDOW_HEIGHT{1080};
inline constexpr float_t MIN_RADIUS{1.0f};
inline constexpr float_t MAX_RADIUS{8.0f};
inline constexpr float_t MIN_VELOCITY{50.0f};
inline constexpr float_t MAX_VELOCITY{200.0f};

struct Circle
{
    explicit Circle(glm::vec2&& position, glm::vec2&& velocity, const float_t radius)
        : m_Position{std::move(position)}
        , m_Velocity{std::move(velocity)}
        , m_Radius{radius}
        , m_Mass{m_Radius * 0.5f}
    {
    }

    const glm::vec2& GetPosition() const { return m_Position; }

    glm::vec2 m_Position{};
    glm::vec2 m_Velocity{};
    float_t m_Radius{0.0f};
    float_t m_Mass{0.0f};
};

inline void ClampPositionToWindow(glm::vec2& spawnPosition)
{
    spawnPosition.x = glm::fclamp(spawnPosition.x, 0.0f, static_cast<float_t>(WINDOW_WIDTH));
    spawnPosition.y = glm::fclamp(spawnPosition.y, 0.0f, static_cast<float_t>(WINDOW_HEIGHT));
}

inline glm::vec2 RandomWindowPosition()
{
    return glm::vec2{Random::RandomInRange(0.0f, WINDOW_WIDTH), Random::RandomInRange(0.0f, WINDOW_HEIGHT)};
}

inline glm::vec2 RandomNormal()
{
    return glm::normalize(
        glm::vec2{Random::RandomInRange(-1.0f, 1.0f), Random::RandomInRange(-1.0f, 1.0f)});
}

inline Circle SpawnCircle(glm::vec2&& spawnPosition)
{
    ClampPositionToWindow(spawnPosition);
    return Circle{
        std::move(spawnPosition),
        RandomNormal() * Random::RandomInRange(MIN_VELOCITY, MAX_VELOCITY),
        Random::RandomInRange(MIN_RADIUS, MAX_RADIUS)};
}

class Rectangle
{
public:
    Rectangle() = default;

    explicit Rectangle(const glm::vec2& topLeft, const float_t width, const float_t height)
        : m_TopLeft{topLeft}
        , m_Width{width}
        , m_Height{height}
    {
    }

    const glm::vec2& GetTopLeft() const
    {
        return m_TopLeft;
    }

    glm::vec2 GetTopRight() const
    {
        return glm::vec2{m_TopLeft.x + m_Width, m_TopLeft.y};
    }

    glm::vec2 GetBottomLeft() const
    {
        return glm::vec2{m_TopLeft.x, m_TopLeft.y + m_Height};
    }

    glm::vec2 GetBottomRight() const
    {
        return glm::vec2{m_TopLeft.x + m_Width, m_TopLeft.y + m_Height};
    }

    float_t GetWidth() const
    {
        return m_Width;
    }

    float_t GetHeight() const
    {
        return m_Height;
    }

private:
    glm::vec2 m_TopLeft{};
    float_t m_Width{0.0f};
    float_t m_Height{0.0f};
};

inline bool CollisionRectPoint(const Rectangle& rect, const glm::vec2& point)
{
    if( rect.GetTopLeft().y <= point.y && rect.GetBottomRight().y >= point.y &&
        rect.GetTopLeft().x <= point.x && rect.GetBottomRight().x >= point.x)
    {
        return true;
    }

    return false;
}

inline bool CollisionRectWithinRect(const Rectangle& innerRect, const Rectangle& outerRect)
{
    return CollisionRectPoint(outerRect, innerRect.GetTopLeft()) &&
            CollisionRectPoint(outerRect, innerRect.GetTopRight()) &&
            CollisionRectPoint(outerRect, innerRect.GetBottomLeft()) &&
            CollisionRectPoint(outerRect, innerRect.GetBottomRight());
}

inline bool CollisionRectRect(const Rectangle& rectA, const Rectangle& rectB)
{
    // https://www.jeffreythompson.org/collision-detection/table_of_contents.php
    if( rectA.GetBottomRight().x >= rectB.GetTopLeft().x && // rectA right edge past rectB left
        rectA.GetTopLeft().x <= rectB.GetBottomRight().x && // rectA left edge past rectB right
        rectA.GetBottomRight().y >= rectB.GetTopLeft().y && // rectA top edge past rectB bottom
        rectA.GetTopLeft().y <= rectB.GetBottomRight().y)   // rectA bottom edge past rectB top
    {
        return true;
    }

    return false;
}

inline void ResolveCollisionCircleEdgeOfScreen(Circle& circle)
{
    static constexpr glm::vec2 topEdgeNormal{0.0f, -1.0f};
    static constexpr glm::vec2 bottomEdgeNormal{-topEdgeNormal};
    static constexpr glm::vec2 leftEdgeNormal{1.0f, 0.0f};
    static constexpr glm::vec2 rightEdgeNormal{-leftEdgeNormal};

    if(circle.m_Position.y - circle.m_Radius < 0.0f)
    {
        circle.m_Velocity = glm::reflect(circle.m_Velocity, topEdgeNormal);
        circle.m_Position.y = circle.m_Radius;
    }
    else if(circle.m_Position.y + circle.m_Radius > WINDOW_HEIGHT)
    {
        circle.m_Velocity = glm::reflect(circle.m_Velocity, bottomEdgeNormal);
        circle.m_Position.y = WINDOW_HEIGHT - circle.m_Radius;
    }

    if(circle.m_Position.x - circle.m_Radius < 0.0f)
    {
        circle.m_Velocity = glm::reflect(circle.m_Velocity, leftEdgeNormal);
        circle.m_Position.x = circle.m_Radius;
    }
    else if(circle.m_Position.x + circle.m_Radius > WINDOW_WIDTH)
    {
        circle.m_Velocity = glm::reflect(circle.m_Velocity, rightEdgeNormal);
        circle.m_Position.x = WINDOW_WIDTH - circle.m_Radius;
    }
}

inline void ResolveElasticCollisionCircleCircle(Circle& circleA, Circle& circleB)
{
    if(circleA.m_Position == circleB.m_Position)
    {
        circleA.m_Position.x += 0.01f;
    }

    const float_t radiusSum{circleA.m_Radius + circleB.m_Radius};
    const glm::vec2 toCircleB{circleB.m_Position - circleA.m_Position};
    const float_t distance{glm::length(toCircleB)};

    if(radiusSum > distance)
    {
        // https://blogs.love2d.org/content/circle-collisions
        const glm::vec2 total{circleA.m_Velocity - circleB.m_Velocity};
        const glm::vec2 velocityA{(circleA.m_Velocity * (circleA.m_Mass - circleB.m_Mass) + (2.0f * circleB.m_Mass * circleB.m_Velocity)) / (circleA.m_Mass + circleB.m_Mass)};
        const glm::vec2 velocityB{total + velocityA};
        circleA.m_Velocity = velocityA;
        circleB.m_Velocity = velocityB;

        const float_t halfOverlap{(radiusSum - distance) * 0.5f};
        const glm::vec2 toInnerNormalized{glm::normalize(toCircleB)};
        circleB.m_Position += toInnerNormalized * halfOverlap;
        circleA.m_Position -= toInnerNormalized * halfOverlap;
    }
}
