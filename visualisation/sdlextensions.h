#pragma once

#include <SDL3/SDL_timer.h>

namespace
{
    // This is based on code taken from:
    // https://gist.github.com/wldomiciano/b38a28e2cde2c77b08ad824e7d8c0076#file-circle-drawing-c

    using RenderCircleStrategy = void(*)(SDL_Renderer*, float_t, float_t, float_t, float_t);

    inline void SDL_RenderCircleStrategy(
        SDL_Renderer* const renderer, const float_t xc, const float_t yc, const float_t x, const float_t y)
    {
        SDL_RenderPoint(renderer, xc + x, yc + y);
        SDL_RenderPoint(renderer, xc - x, yc + y);

        SDL_RenderPoint(renderer, xc + x, yc - y);
        SDL_RenderPoint(renderer, xc - x, yc - y);

        SDL_RenderPoint(renderer, xc + y, yc + x);
        SDL_RenderPoint(renderer, xc - y, yc + x);

        SDL_RenderPoint(renderer, xc + y, yc - x);
        SDL_RenderPoint(renderer, xc - y, yc - x);
    }

    inline void SDL_RenderFillCircleStrategy(
        SDL_Renderer* const renderer, const float_t xc, const float_t yc, const float_t x, const float_t y)
    {
        SDL_RenderLine(renderer, xc + x, yc + y, xc - x, yc + y);
        SDL_RenderLine(renderer, xc + x, yc - y, xc - x, yc - y);
        SDL_RenderLine(renderer, xc + y, yc + x, xc - y, yc + x);
        SDL_RenderLine(renderer, xc + y, yc - x, xc - y, yc - x);
    }

    template<RenderCircleStrategy renderCircleStrategy>
    inline void SDL_RenderCircleConcept(
        SDL_Renderer* const renderer, const float_t xc, const float_t yc, const float_t radius)
    {
        float_t x{0.0f};
        float_t y{radius};
        float_t d{3.0f - 2.0f * radius};
        renderCircleStrategy(renderer, xc, yc, x, y);

        while(y >= x)
        {
            ++x;
            if(d > 0.0f)
            {
                --y;
                d = d + 4.0f * (x - y) + 10.0f;
            }
            else
            {
                d = d + 4.0f * x + 6.0f;
            }
            renderCircleStrategy(renderer, xc, yc, x, y);
        }
    }
}

inline constexpr auto SDL_RenderCircle = SDL_RenderCircleConcept<SDL_RenderCircleStrategy>;
inline constexpr auto SDL_RenderFillCircle = SDL_RenderCircleConcept<SDL_RenderFillCircleStrategy>;

inline float_t SDL_FrameDelta()
{
    static const float_t performanceFrequency{static_cast<float_t>(SDL_GetPerformanceFrequency())};
    static uint64_t previousPerformanceCounter{SDL_GetPerformanceCounter()};

    const uint64_t performanceCounter{SDL_GetPerformanceCounter()};
    const float_t diff{static_cast<float_t>(performanceCounter - previousPerformanceCounter)};
    previousPerformanceCounter = performanceCounter;
    return diff / performanceFrequency;
}
