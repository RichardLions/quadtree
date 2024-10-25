// Based on SDL3 examples: https://examples.libsdl.org/SDL3/renderer/02-primitives/

#include "stdafx.h"

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_log.h>

#include "simulation/quadtree.h"
#include "simulation/simulation.h"

#include "quadtree.h"
#include "sdlextensions.h"

struct AppData
{
    Quadtree m_Quadtree{};
    std::vector<Circle> m_Circles{};
    SDL_Renderer* m_Renderer{nullptr};
    SDL_Window* m_Window{nullptr};
    glm::vec2 m_MousePoint{0.0f, 0.0f};
    bool m_Paused{false};
    bool m_DrawQuadtree{true};
    bool m_DrawTestSelectionQuad{true};
    bool m_UseQuadTree{true};
};

SDL_AppResult SDL_AppInit(
    [[maybe_unused]] void** const appState, [[maybe_unused]] const int argc, [[maybe_unused]] char* argv[])
{
    static AppData appData{};
    *appState = &appData;

    SDL_SetAppMetadata("Quadtree Example", "", "");

    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if(!SDL_CreateWindowAndRenderer("Quadtree Example", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &appData.m_Window, &appData.m_Renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    appData.m_Circles.reserve(NUM_CIRCLES);
    for(uint32_t i{0}; i != NUM_CIRCLES; ++i)
    {
        appData.m_Circles.push_back(SpawnCircle(RandomWindowPosition()));
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* const appState, SDL_Event* const event)
{
    AppData* const appData{reinterpret_cast<AppData*>(appState)};

    switch(event->type)
    {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if(event->button.button == SDL_BUTTON_LEFT)
        {
            appData->m_Circles.push_back(SpawnCircle(glm::vec2{event->button.x, event->button.y}));
        }
        return SDL_APP_CONTINUE;
    case SDL_EVENT_MOUSE_MOTION:
        appData->m_MousePoint = glm::vec2{event->motion.x, event->motion.y};
        return SDL_APP_CONTINUE;
    case SDL_EVENT_KEY_DOWN:
        if(event->key.key == SDLK_ESCAPE)
            return SDL_APP_SUCCESS;
        else if(event->key.key == SDLK_SPACE)
            appData->m_Paused = !appData->m_Paused;
        else if(event->key.key == SDLK_1)
            appData->m_DrawQuadtree = !appData->m_DrawQuadtree;
        else if(event->key.key == SDLK_2)
            appData->m_DrawTestSelectionQuad = !appData->m_DrawTestSelectionQuad;
        else if(event->key.key == SDLK_RETURN)
            appData->m_UseQuadTree = !appData->m_UseQuadTree;

        return SDL_APP_CONTINUE;
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    default:
        return SDL_APP_CONTINUE;
    }
}

SDL_AppResult SDL_AppIterate(void* const appState)
{
    AppData* const appData{reinterpret_cast<AppData*>(appState)};
    const float_t delta{SDL_FrameDelta()};

    // Log fps
    static float_t deltaSum{0.0f};
    static uint32_t frames{0};
    if(deltaSum > 1.0f)
    {
        SDL_Log("FPS - %i", frames);
        deltaSum = 0.0f;
        frames = 0;
    }
    deltaSum += delta;
    ++frames;

    SDL_SetRenderDrawColor(appData->m_Renderer, 0, 0, 0, 255);
    SDL_RenderClear(appData->m_Renderer);

    RebuildQuadtree(appData->m_Quadtree, appData->m_Circles);

    if(!appData->m_Paused)
    {
        if(appData->m_UseQuadTree)
        {
            UpdateCirclesQuadtreeInnerLoop(appData->m_Quadtree, appData->m_Quadtree.GetRootBranch(), delta);
        }
        else
        {
            UpdateCirclesBruteForce(appData->m_Circles, delta);
        }
    }

    // Draw and clamp within screen.
    for(Circle& circle : appData->m_Circles)
    {
        ResolveCollisionCircleEdgeOfScreen(circle);
        SDL_SetRenderDrawColor(
            appData->m_Renderer,
            static_cast<uint8_t>(std::clamp(255.0f * (glm::length(circle.m_Velocity) / MAX_VELOCITY), 50.f, 255.0f)), 0, 0, 255);
        SDL_RenderFillCircle(appData->m_Renderer, circle.m_Position.x, circle.m_Position.y, circle.m_Radius);
    }

    if(appData->m_DrawQuadtree)
    {
        SDL_SetRenderDrawColor(appData->m_Renderer, 0, 0, 255, 255);
        RenderQuadtree(appData->m_Renderer, appData->m_Quadtree.GetRootBranch());
    }

    if(appData->m_DrawTestSelectionQuad)
    {
        constexpr float_t width{75.0f};
        constexpr float_t height{75.0f};
        const Rectangle mouseRect{appData->m_MousePoint - glm::vec2{width * 0.5f, height * 0.5f}, width, height};
        const SDL_FRect rect{mouseRect.GetTopLeft().x, mouseRect.GetTopLeft().y, mouseRect.GetWidth(), mouseRect.GetHeight()};
        SDL_SetRenderDrawColor(appData->m_Renderer, 255, 255, 255, 255);
        SDL_RenderRect(appData->m_Renderer, &rect);

        std::vector<Quadtree::Branch*> branches{};
        if(appData->m_Quadtree.FindBranches(mouseRect, branches))
        {
            for(const Quadtree::Branch* const branch : branches)
            {
                const SDL_FRect branchRect{
                    branch->GetRect().GetTopLeft().x,
                    branch->GetRect().GetTopLeft().y,
                    branch->GetRect().GetWidth(),
                    branch->GetRect().GetHeight()};
                SDL_RenderRect(appData->m_Renderer, &branchRect);
            }
        }

        std::vector<Circle*> circles{};
        if(appData->m_Quadtree.FindLeaves(mouseRect, circles))
        {
            for(const Circle* const circle : circles)
            {
                SDL_RenderFillCircle(appData->m_Renderer, circle->m_Position.x, circle->m_Position.y, circle->m_Radius);
            }
        }
    }

    SDL_RenderPresent(appData->m_Renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit([[maybe_unused]] void* const appState, [[maybe_unused]] const SDL_AppResult result)
{
}
