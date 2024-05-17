#include <catch2/catch_session.hpp>

#include <betterstring/detail/cpu_isa.hpp>

static void print_cpu_features() {
    const auto feature = [](const char* name) {
        std::fputs(name, stdout);
        std::putchar(' ');
    };

    std::fputs("Available features: ", stdout);
    using namespace bs::detail::isa;
    if (AVX2) { feature("AVX2"); }
    if (BMI2) { feature("BMI2"); }
    if (POPCNT) { feature("POPCNT"); }
    std::fputs("\n\n", stdout);
}

int main(int argc, char* argv[]) {
    print_cpu_features();

    return Catch::Session().run(argc, argv);
}
