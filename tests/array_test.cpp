// ISO C++ Includes
#include <array> // For tests

// Google Test Includes
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Local Includes
#include <collections/array.hpp>

namespace collections::array_testing {
    // ── Constructor Tests ────────────────────────────────────────────────────────────────────────────────────────────
    TEST(array_constructors, variadic_overload) {
        // Assert at compile time the operator is noexcept as declared
        static_assert(noexcept(std::declval<array<int, 3>&>()[0]));

        // Assert at compile time that the operator is usable as a constant expression
        constexpr array<int, 3> array = {1, 2, 3};
        static_assert(1 == array[0]);
        static_assert(2 == array[1]);
        static_assert(3 == array[2]);
    }

    // ── Overloaded Operators Tests ───────────────────────────────────────────────────────────────────────────────────
    TEST(array_operators, random_access_mut_overload) {
        // Assert at compile time the operator's return type is a mutable reference
        static_assert(std::same_as<decltype(std::declval<array<int, 3>&>()[0]), array<int, 3>::reference>);

        // Assert at compile time the operator is noexcept as declared
        static_assert(noexcept(std::declval<array<int, 3>&>()[0]));

        // Assert at compile time that the operator is usable as a constant expression
        constexpr array<int, 3> array = {1, 2, 3};
        static_assert(1 == array[0]);
        static_assert(2 == array[1]);
        static_assert(3 == array[2]);
    }

    TEST(array_operators, random_access_const_overload) {
        // Assert at compile time the operator's return type is a constant reference
        static_assert(std::same_as<decltype(std::declval<const array<int, 3>&>()[0]), array<int, 3>::const_reference>);

        // Assert at compile time the operator is noexcept as declared
        static_assert(noexcept(std::declval<const array<int, 3>&>()[0]));

        // Assert at compile time that the operator is usable as a constant expression
        constexpr array<int, 3> array = {1, 2, 3};
        static_assert(1 == array[0]);
        static_assert(2 == array[1]);
        static_assert(3 == array[2]);
    }

} // namespace collections::array_testing
