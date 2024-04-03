#include <catch2/catch_session.hpp>

#include <betterstring/detail/cpu_features.hpp>

static void print_cpu_features() {
    using bs::detail::cpu_features;
    const auto& value = cpu_features.value;

    const auto feature = [](const char* name) {
        std::fputs(name, stdout);
        std::putchar(' ');
    };

    std::fputs("Available features: ", stdout);
    // NOLINTBEGIN(readability-implicit-bool-conversion)
    if (value & cpu_features.AVX2) { feature("AVX2"); }
    if (value & cpu_features.BMI2) { feature("BMI2"); }
    if (value & cpu_features.POPCNT) { feature("POPCNT"); }
    // NOLINTEND(readability-implicit-bool-conversion)
    std::fputs("\n\n", stdout);
}

int main(int argc, char* argv[]) {
    print_cpu_features();

    return Catch::Session().run(argc, argv);
}
