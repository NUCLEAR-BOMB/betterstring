#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>
#include <array>
#include <betterstring/detail/preprocessor.hpp>
#include <betterstring/string_view.hpp>

namespace bs::detail::enum_parsing {

template<class Enum>
struct subinfo_t {
    Enum value;
    bs::string_view string;
};

template<class Enum, std::size_t Size>
struct info_t {
    subinfo_t<Enum> infos[Size];

    static constexpr std::size_t size() noexcept { return Size; }
};

template<class Enum, std::size_t Size>
constexpr std::size_t max_string_length(const info_t<Enum, Size>& infos) {
    std::size_t max = 0;
    for (const auto& info : infos.infos) {
        if (info.string.size() > max) { max = info.string.size(); }
    }
    return max;
}

template<std::size_t MaxLength, class Ch, class Enum, class InfoFn, std::size_t... Idx>
constexpr auto generate_entry_table(InfoFn infofn, std::index_sequence<Idx...>) {
    constexpr auto enum_info = infofn();

    using entry_fn_t = Enum(*)(const Ch*);
    std::array<entry_fn_t, MaxLength> entry_table{};

    ([&]() {
        entry_table[Idx] = [](const Ch* str) -> Enum {
            for (std::size_t i = 0; i < enum_info.size(); ++i) {
                if (enum_info.infos[i].string.size() != Idx) { continue; }

                if (std::memcmp(str, enum_info.infos[i].string.data(), Idx) == 0) {
                    return enum_info.infos[i].value;
                }
            }
            return Enum{};
        };
    }(), ...);

    return entry_table;
}

}

namespace bs {

template<class Enum, std::size_t Size>
constexpr detail::enum_parsing::info_t<Enum, Size> enumeration_parsing_info(const detail::enum_parsing::subinfo_t<Enum> (&arr)[Size]) noexcept {
    detail::enum_parsing::info_t<Enum, Size> out{};
    for (std::size_t i = 0; i < Size; ++i) {
        out.infos[i] = arr[i];
    }
    return out;
}

template<class Enum, class InfoFn, class Ch>
constexpr Enum parse(InfoFn infofn, const Ch* const top_str, const std::size_t str_len) noexcept {
    using namespace detail::enum_parsing;

    constexpr auto enum_info = infofn();
    constexpr std::size_t max_length = max_string_length(enum_info);

    static constexpr auto entry_table = generate_entry_table<max_length, Ch, Enum>(infofn, std::make_index_sequence<max_length>{});

    return entry_table[str_len](top_str);
}


}
