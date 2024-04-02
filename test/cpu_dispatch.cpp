
// Copyright 2024.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <vector>

#include <betterstring/string_view.hpp>
#include <betterstring/detail/cpu_features.hpp>

namespace {

void print_current_cpu_features() {
    using bs::detail::cpu_features;
    const auto& value = cpu_features.value;

    const auto print_feature = [](const char* name) {
        std::fputs(name, stdout);
        std::putchar(' ');
    };

    std::fputs("\nCurrent processor supports these extensions: ", stdout);
    // NOLINTBEGIN(readability-implicit-bool-conversion)
    if (value & cpu_features.AVX2) { print_feature("AVX2"); }
    if (value & cpu_features.BMI2) { print_feature("BMI2"); }
    if (value & cpu_features.POPCNT) { print_feature("POPCNT"); }
    // NOLINTEND(readability-implicit-bool-conversion)
    std::putchar('\n');
}

TEST_CASE("cpu features", "[cpu_features]") {
    print_current_cpu_features();
}

}
