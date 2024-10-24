#pragma once

#include "quadtreeconcept.h"
#include "shapeprimitives.h"

using Quadtree = QuadtreeConcept<Circle, 4, 2>;

inline constexpr auto RebuildQuadtree = RebuildQuadtreeConcept<Quadtree>;
