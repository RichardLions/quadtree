#pragma once

#include <random>

namespace Random
{
    namespace
    {
        using RandomGenerator = std::mt19937;
        inline RandomGenerator& GetRandomGenerator()
        {
            static std::random_device device{};
            static RandomGenerator generator{device()}; // Seed
            return generator;
        }
    }

    /// Returns true/false
    inline bool RandomBool()
    {
        static constexpr double_t probability{0.5};
        static std::bernoulli_distribution distribution{probability};
        return distribution(GetRandomGenerator());
    }

    /// Returns a value between and including min~max
    inline int32_t RandomInRange(const int32_t min, const int32_t max)
    {
        std::uniform_int_distribution<int32_t> distribution(min, max);
        return distribution(GetRandomGenerator());
    }

    /// Returns a value between and including min~max
    inline float_t RandomInRange(const float_t min, const float_t max)
    {
        std::uniform_real_distribution<float_t> distribution(min, max);
        return distribution(GetRandomGenerator());
    }
}
