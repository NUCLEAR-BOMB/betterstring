#include <benchmark/benchmark.h>
#include <betterstring.hpp>
#include <cstring>
#include <string>
#include <string_view>

#include "fixture.hpp"

namespace {

BENCHMARK_F(string_fixture, string_view_find)(benchmark::State& state) {
    const bs::string_view string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(string.find("string"));
    }
}

BENCHMARK_F(string_fixture, std_strstr_find)(benchmark::State& state) {
    const auto string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(std::strstr(string, "string"));
    }
}

BENCHMARK_F(string_fixture, std_string_find)(benchmark::State& state) {
    const std::string string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(string.find("string"));
    }
}

BENCHMARK_F(string_fixture, std_string_view_find)(benchmark::State& state) {
    const std::string_view string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(string.find("string"));
    }
}

BENCHMARK_F(string_fixture, string_view_rfind)(benchmark::State& state) {
    const bs::string_view string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(string.rfind("string"));
    }
}

BENCHMARK_F(string_fixture, std_string_view_rfind)(benchmark::State& state) {
    const std::string_view string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(string.rfind("string"));
    }
}

BENCHMARK_F(string_fixture, std_string_rfind)(benchmark::State& state) {
    const std::string string = get_string();
    for (auto _ : state) {
        benchmark::DoNotOptimize(string.rfind("string"));
    }
}

#if 0
BENCHMARK_F(string_fixture, string_view_strlen)(benchmark::State& state) {
    for (auto _ : state) {
        bs::string_view string(get_string());
        benchmark::DoNotOptimize(string);
    }
}

BENCHMARK_F(string_fixture, std_string_view_strlen)(benchmark::State& state) {
    for (auto _ : state) {
        std::string_view string(get_string());
        benchmark::DoNotOptimize(string);
    }
}

BENCHMARK_F(string_fixture, std_strlen)(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(std::strlen(get_string()));
    }
}
#endif

BENCHMARK_F(string_fixture, string_view_strip)(benchmark::State& state) {
    bs::string_view string = get_string();
    const auto strip_string = R"STRIP(\5d2gm$;+ikP|giXQg}J\3;$PyrcEMeU-`/?o(T"[bN+>+F;73]x@+1QQiZ0<v=IQhs5|<nuE%D@8])?6"vsy$-rz/#b*:3kR'Lv-MzD.oe=ep%U{?@pCn7jKY!#>Lg|te}heD5lDpbAut}";V!y!%9Q[\E;Gc]A^PpTF+p,(n8<X]Z*230te>y~ERNjrc:/pJl#7dV-fRu1L0C*yLo >/\ZT-!_nkT#9w\7A:c`'k$#[8@pU;% X&Ci7kf-ro5NZ-ICp9N5A%!}-;3VJ2:{(0!G *10ZPC53jE/p,QlcJ+W??tzsX"&d4{4'$YAxsE\e\C<nw\UdE@o[h{:$_rn%O%WA*iU(6Gz-O^I_sPPw*zfSBy:J_~J,:-?t#Qdcz2I,Jcs^-AlreXbtJ$9CHG2L1Znpn&?n o%Yox qj}_`3#CFNdb6Fb/q*>+p2N_zK0jCrP1G)iXKs`t>%"yyg?MfI)STRIP";
    for (auto _ : state) {
        benchmark::DoNotOptimize(string.strip(strip_string));
    }
}

}

