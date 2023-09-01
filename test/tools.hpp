#pragma once
#include <type_traits>

#ifdef __INTELLISENSE__
  #pragma diag_suppress 2896
#endif

#define CONSTEXPR_EXPECT(lambda) \
    EXPECT_EQ((::std::integral_constant<int, ([] lambda ())>::value), 0)
