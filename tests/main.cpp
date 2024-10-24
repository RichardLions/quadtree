#include "stdafx.h"

#include <catch2/catch_session.hpp>

#include "quadtreetests.h"

int main(const int argc, const char* const argv[])
{
    return Catch::Session().run(argc, argv);
}
