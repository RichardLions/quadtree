#pragma once

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "simulation/simulation.h"
#include "simulation/quadtree.h"

namespace
{
    inline constexpr uint32_t NUM_CIRCLES{5000};
    inline constexpr float_t DELTA{0.1f};
}

TEST_CASE("Build Quadtree - Benchmarks")
{
    std::vector<Circle> circles{};
    circles.reserve(NUM_CIRCLES);
    for(uint32_t i{0}; i != NUM_CIRCLES; ++i)
    {
        circles.push_back(SpawnCircle(RandomWindowPosition()));
    }

    BENCHMARK("Benchmark")
    {
        Quadtree quadtree{};
        RebuildQuadtree(quadtree, circles);
    };
}

TEST_CASE("Search Quadtree - Benchmarks")
{
    std::vector<Circle> circles{};
    circles.reserve(NUM_CIRCLES);
    for(uint32_t i{0}; i != NUM_CIRCLES; ++i)
    {
        circles.push_back(SpawnCircle(RandomWindowPosition()));
    }

    Quadtree quadtree{};

    BENCHMARK("Found Branches")
    {
        for(Circle& circle : circles)
        {
            ResolveCollisionCircleEdgeOfScreen(circle);
        }
        RebuildQuadtree(quadtree, circles);
        UpdateCirclesQuadtreeFoundBranches(circles, quadtree, DELTA);
    };

    BENCHMARK("Found Leaves")
    {
        for(Circle& circle : circles)
        {
            ResolveCollisionCircleEdgeOfScreen(circle);
        }
        RebuildQuadtree(quadtree, circles);
        UpdateCirclesQuadtreeFoundLeaves(circles, quadtree, DELTA);
    };

    BENCHMARK("Inner Loop")
    {
        for(Circle& circle : circles)
        {
            ResolveCollisionCircleEdgeOfScreen(circle);
        }
        RebuildQuadtree(quadtree, circles);
        UpdateCirclesQuadtreeInnerLoop(quadtree, quadtree.GetRootBranch(), DELTA);
    };
}

TEST_CASE("Brute Force - Benchmarks")
{
    std::vector<Circle> circles{};
    circles.reserve(NUM_CIRCLES);
    for(uint32_t i{0}; i != NUM_CIRCLES; ++i)
    {
        circles.push_back(SpawnCircle(RandomWindowPosition()));
    }

    BENCHMARK("Benchmark")
    {
        for(Circle& circle : circles)
        {
            ResolveCollisionCircleEdgeOfScreen(circle);
        }
        UpdateCirclesBruteForce(circles, DELTA);
    };
}
