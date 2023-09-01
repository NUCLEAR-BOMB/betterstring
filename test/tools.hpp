#pragma once
#include <type_traits>

#ifdef __INTELLISENSE__
  #pragma diag_suppress 2896
#endif

#define CONSTEXPR_EXPECT(name, lambda) \
    constexpr int name##_constexpr_result = [] lambda (); \
    EXPECT_EQ( name##_constexpr_result , 0)
