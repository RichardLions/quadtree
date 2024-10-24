#pragma once

#include <SDL3/SDL_render.h>

#include "simulation/quadtreeconcept.h"

template<class TQuadtree>
void RenderQuadtreeConcept(SDL_Renderer* const renderer, const typename TQuadtree::Node& node)
{
    if(node.GetChildren().empty())
    {
        const Rectangle& rectangle{node.GetRect()};
        const SDL_FRect rect{rectangle.GetTopLeft().x, rectangle.GetTopLeft().y, rectangle.GetWidth(), rectangle.GetHeight()};
        SDL_RenderRect(renderer, &rect);
    }

    for(const TQuadtree::Node& childNode : node.GetChildren())
    {
        RenderQuadtreeConcept<TQuadtree>(renderer, childNode);
    }
}

inline constexpr auto RenderQuadtree = RenderQuadtreeConcept<Quadtree>;
