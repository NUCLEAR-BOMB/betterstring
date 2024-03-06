#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <vector>
#include <betterstring/string_view.hpp>
#include <betterstring/detail/cpu_features.hpp>

TEST_CASE("cpu features", "[cpu_features]") {
    using bs::detail::cpu_features;
    const auto& value = cpu_features.value;

    std::vector<bs::string_view> cpu_features_list;
    if (value & cpu_features.AVX2) { cpu_features_list.push_back("AVX2"); }
    if (value & cpu_features.BMI2) { cpu_features_list.push_back("BMI2"); }
    if (value & cpu_features.POPCNT) { cpu_features_list.push_back("POPCNT"); }

    std::fputs("\nCurrent processor supports these extensions: ", stdout);
    for (auto feature_name : cpu_features_list) {
        std::fwrite(feature_name.data(), sizeof(char), feature_name.size(), stdout);
        std::fputs(" ", stdout);
    }
    std::fputs("\n", stdout);
}
