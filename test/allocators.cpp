#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <betterstring/allocators.hpp>
#include <algorithm>
#include <vector>
#include <array>

namespace {

TEST_CASE("aligned_allocator", "[allocators]") {
    SECTION("alignment 1") {
        bs::aligned_allocator<char, 1> alloc;
        auto ptr = alloc.allocate(5);
        CHECK(std::uintptr_t(ptr) % 1 == 0);
        std::fill_n(ptr, 5, 'a');

        CHECK(ptr[0] == 'a');
        CHECK(ptr[1] == 'a');
        CHECK(ptr[2] == 'a');
        CHECK(ptr[3] == 'a');
        CHECK(ptr[4] == 'a');
        alloc.deallocate(ptr, 5);
    }
    SECTION("alignment 2") {
        bs::aligned_allocator<char, 2> alloc;

        auto ptr = alloc.allocate(4);
        CHECK(std::uintptr_t(ptr) % 2 == 0);
        ptr[0] = 'a';
        ptr[3] = 'b';
        alloc.deallocate(ptr, 4);
    }
    SECTION("alignment 16") {
        bs::aligned_allocator<char, 16> alloc;

        auto ptr = alloc.allocate(20);
        CHECK(std::uintptr_t(ptr) % 16 == 0);
        ptr[0] = 'a';
        CHECK(ptr[0] == 'a');
        ptr[19] = 'b';
        CHECK(ptr[19] == 'b');
        alloc.deallocate(ptr, 20);
    }
    SECTION("alignment 32") {
        bs::aligned_allocator<char, 32> alloc;

        for (std::size_t i = 1; i <= 10; ++i) {
            const std::size_t alloc_size = i * 10;

            char* const ptr = alloc.allocate(alloc_size);
            CHECK(std::uintptr_t(ptr) % 32 == 0);

            ptr[0] = '1';
            CHECK(ptr[0] == '1');
            ptr[alloc_size - 1] = '2';
            CHECK(ptr[alloc_size - 1] == '2');

            alloc.deallocate(ptr, alloc_size);
        }
    }
    SECTION("alignment 64") {
        bs::aligned_allocator<char, 64> alloc;

        for (std::size_t i = 1; i <= 10; ++i) {
            const std::size_t alloc_size = i * 15;

            char* const ptr = alloc.allocate(alloc_size);
            CHECK(std::uintptr_t(ptr) % 64 == 0);

            ptr[0] = '!';
            CHECK(ptr[0] == '!');
            ptr[alloc_size - 1] = '?';
            CHECK(ptr[alloc_size - 1] == '?');

            alloc.deallocate(ptr, alloc_size);
        }
    }
    SECTION("alignment 128") {
        bs::aligned_allocator<char, 128> alloc;

        for (std::size_t i = 1; i <= 10; ++i) {
            const std::size_t alloc_size = i * 20;

            char* const ptr = alloc.allocate(alloc_size);
            CHECK(std::uintptr_t(ptr) % 128 == 0);

            ptr[0] = 'q';
            CHECK(ptr[0] == 'q');
            ptr[alloc_size - 1] = 'w';
            CHECK(ptr[alloc_size - 1] == 'w');

            alloc.deallocate(ptr, alloc_size);
        }
    }
    SECTION("alignment 256") {
        bs::aligned_allocator<char, 256> alloc;

        for (std::size_t i = 1; i <= 10; ++i) {
            const std::size_t alloc_size = i * 50;

            char* const ptr = alloc.allocate(alloc_size);
            CHECK(std::uintptr_t(ptr) % 256 == 0);

            ptr[0] = '8';
            CHECK(ptr[0] == '8');
            ptr[alloc_size - 1] = '3';
            CHECK(ptr[alloc_size - 1] == '3');

            alloc.deallocate(ptr, alloc_size);
        }
    }
    SECTION("std::vector") {
        using Catch::Matchers::RangeEquals;

        std::vector<char, bs::aligned_allocator<char, 16>> vec;
        for (std::size_t i = 0; i < 20; ++i) {
            vec.emplace_back(char(i * 37));
            CHECK(std::uintptr_t(vec.data()) % 16 == 0);
        }
        CHECK_THAT(vec, RangeEquals(std::array{0, 37, 74, 111, 148, 185, 222, 3, 40, 77, 114, 151, 188, 225, 6, 43, 80, 117, 154, 191}, [](auto l, auto r) { return char(l) == char(r); }));
    }
    SECTION("std::string") {
        std::basic_string<char, std::char_traits<char>, bs::aligned_allocator<char, 32>> str;
        str.append(50, 'a'); // avoid small string optimization
        for (std::size_t i = 0; i < 30; ++i) {
            str.push_back(char(i * 33));
            CHECK(std::uintptr_t(str.data()) % 32 == 0);
        }
    }
}

}

