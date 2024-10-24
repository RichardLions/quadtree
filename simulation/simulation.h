#pragma once

#include "quadtree.h"

void UpdateCirclesQuadtree(std::vector<Circle>& circles, Quadtree& quadtree, float_t delta);
void UpdateCirclesBruteForce(std::vector<Circle>& circles, float_t delta);
