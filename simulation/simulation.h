#pragma once

#include "quadtree.h"

void UpdateCirclesQuadtreeFoundLeaves(std::vector<Circle>& circles, Quadtree& quadtree, float_t delta);
void UpdateCirclesQuadtreeFoundBranches(std::vector<Circle>& circles, Quadtree& quadtree, float_t delta);
void UpdateCirclesQuadtreeInnerLoop(Quadtree& quadtree, const Quadtree::Branch& branch, float_t delta);

void UpdateCirclesBruteForce(std::vector<Circle>& circles, float_t delta);
