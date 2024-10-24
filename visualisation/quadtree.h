#pragma once

#include <SDL3/SDL_render.h>

#include "simulation/quadtreeconcept.h"

template<class TQuadtree>
void RenderQuadtreeConcept(SDL_Renderer* const renderer, const typename TQuadtree::Branch& branch)
{
    if(!branch.HasBranches())
    {
        const Rectangle& rectangle{branch.GetRect()};
        const SDL_FRect rect{rectangle.GetTopLeft().x, rectangle.GetTopLeft().y, rectangle.GetWidth(), rectangle.GetHeight()};
        SDL_RenderRect(renderer, &rect);
    }

    for(const TQuadtree::Branch& childBranch : branch.GetBranches())
    {
        RenderQuadtreeConcept<TQuadtree>(renderer, childBranch);
    }
}

inline constexpr auto RenderQuadtree = RenderQuadtreeConcept<Quadtree>;
