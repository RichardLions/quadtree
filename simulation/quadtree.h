#pragma once

#include "quadtreeconcept.h"
#include "shapeprimitives.h"

namespace
{
    inline constexpr uint32_t SPLIT_THRESHOLD{4};
    inline constexpr uint32_t CHILD_DEPTH_THRESHOLD{2};
}

using Quadtree = QuadtreeConcept<Circle, SPLIT_THRESHOLD, CHILD_DEPTH_THRESHOLD>;

inline constexpr auto RebuildQuadtree = RebuildQuadtreeConcept<Quadtree>;
