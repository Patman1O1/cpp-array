// ISO C++ Includes
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <expected>
#include <functional>
#include <limits>
#include <ranges>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Google Test Includes
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Local Includes
#include <collections/array.hpp>

namespace collections::array_testing {
    namespace {
        // ── Aliases ──────────────────────────────────────────────────────────────────────────────────────────────────
        using ::testing::ElementsAre;
        using ::testing::Eq;
        using ::testing::Ne;

        // ── Concepts ──────────────────────────────────────────────────────────────────────────────────────────────────
        template<typename A, typename... Args>
        concept brace_initializable = requires { A{std::declval<Args>()...}; };

        template<typename... Args>
        concept deducible = requires { array{std::declval<Args>()...}; };

        // ── struct nttp_probe ────────────────────────────────────────────────────────────────────────────────────────
        // Only instantiable if array<int, 3> is a structural type.
        template<array<int, 3> Values>
        struct nttp_probe { static constexpr int first = Values[0]; };

        // ── struct keyed ─────────────────────────────────────────────────────────────────────────────────────────────
        // Equal keys with distinct tags, for observing stability.
        struct keyed {
            int key;
            int tag;

            constexpr auto operator<(const keyed& other) const noexcept -> bool { return this->key < other.key; }
        };
    } // namespace

    // ── Aggregate Tests ──────────────────────────────────────────────────────────────────────────────────────────────
    TEST(array_aggregate, is_an_aggregate) {
        // This is the whole design: no user-declared constructors, so `= {1, 2, 3}` is aggregate
        // initialization rather than a call. Nothing to inline, identical codegen at every -O level.
        static_assert(std::is_aggregate_v<array<int, 3>>);
        static_assert(std::is_aggregate_v<array<std::string, 2>>);

        SUCCEED();
    }

    TEST(array_aggregate, brace_init_checks_arity_at_compile_time) {
        static_assert(brace_initializable<array<int, 3>, int, int, int>);
        static_assert(brace_initializable<array<int, 3>, int, int>);
        static_assert(brace_initializable<array<int, 3>, int>);
        static_assert(brace_initializable<array<int, 3>>);
        static_assert(!brace_initializable<array<int, 3>, int, int, int, int>);

        // An element initializer that cannot produce a value_type is rejected too.
        static_assert(!brace_initializable<array<int, 2>, int, std::string>);

        SUCCEED();
    }

    TEST(array_aggregate, brace_init_rejects_narrowing) {
        // The old variadic ctor's static_cast<value_type> laundered narrowing past the braced-init
        // check, so array<int, 2>{1.5, 2.5} compiled and truncated. Aggregate init does not.
        static_assert(!brace_initializable<array<int, 2>, double, double>);
        static_assert(!brace_initializable<array<int, 2>, int, long long>);
        static_assert(!brace_initializable<array<char, 2>, int, int>);

        // Widening is not narrowing.
        static_assert(!brace_initializable<array<double, 2>, int, int>);

        SUCCEED();
    }

    TEST(array_aggregate, brace_init_value_initializes_the_unsupplied_tail) {
        constexpr array<int, 3> values = {7};
        static_assert(7 == values[0]);
        static_assert(0 == values[1]);
        static_assert(0 == values[2]);

        SUCCEED();
    }

    TEST(array_aggregate, has_no_implicit_conversion_from_a_single_element) {
        // An aggregate has no converting constructor, so there is nothing left to mark `explicit`.
        // `array<int, 3> a = 1;` is ill-formed, which is what the old explicit(...) clause was for.
        static_assert(!std::is_convertible_v<int, array<int, 3>>);

        SUCCEED();
    }

    TEST(array_aggregate, brace_init_moves_rvalue_elements) {
        std::string first = "a string long enough to defeat the small string optimization";
        std::string second = "another string long enough to defeat the small string optimization";

        const array values = {std::move(first), std::move(second)};

        EXPECT_THAT(values[0], Eq("a string long enough to defeat the small string optimization"));
        EXPECT_THAT(values[1], Eq("another string long enough to defeat the small string optimization"));

        // Moved-from is unspecified-but-valid; only the fact that a move happened is checkable.
        EXPECT_THAT(first, Ne("a string long enough to defeat the small string optimization"));
        EXPECT_THAT(second, Ne("another string long enough to defeat the small string optimization"));
    }

    TEST(array_aggregate, special_members_are_implicit_and_trivial) {
        // The payoff of declaring none of them: trivially copyable types get passed in registers
        // and memcpy'd by every stdlib algorithm. A single `= default` would have cost aggregate status.
        static_assert(std::is_trivially_default_constructible_v<array<int, 3>>);
        static_assert(std::is_trivially_copy_constructible_v<array<int, 3>>);
        static_assert(std::is_trivially_move_constructible_v<array<int, 3>>);
        static_assert(std::is_trivially_copy_assignable_v<array<int, 3>>);
        static_assert(std::is_trivially_move_assignable_v<array<int, 3>>);
        static_assert(std::is_trivially_destructible_v<array<int, 3>>);
        static_assert(std::is_trivially_copyable_v<array<int, 3>>);

        // Layout must match the bare C array it wraps.
        static_assert(std::is_standard_layout_v<array<int, 3>>);
        static_assert(sizeof(array<int, 3>) == sizeof(int[3]));
        static_assert(alignof(array<int, 3>) == alignof(int[3]));

        // A non-trivial value_type must propagate, not be papered over.
        static_assert(!std::is_trivially_copyable_v<array<std::string, 2>>);
        static_assert(std::is_copy_constructible_v<array<std::string, 2>>);
        static_assert(std::is_move_constructible_v<array<std::string, 2>>);
        static_assert(std::is_copy_assignable_v<array<std::string, 2>>);
        static_assert(std::is_move_assignable_v<array<std::string, 2>>);

        SUCCEED();
    }

    TEST(array_aggregate, copy_from_a_non_const_lvalue_is_a_trivial_copy) {
        array source = {1, 2, 3};
        array<int, 3> copy = source;

        EXPECT_THAT(copy[0], Eq(1));
        EXPECT_THAT(copy[1], Eq(2));
        EXPECT_THAT(copy[2], Eq(3));
        EXPECT_THAT(copy.data(), Ne(source.data()));
    }

    TEST(array_aggregate, copy_assignment_is_not_deleted) {
        constexpr array source = {1, 2, 3};
        array<int, 3> target = {};

        target = source;

        EXPECT_THAT(target[0], Eq(1));
        EXPECT_THAT(target[1], Eq(2));
        EXPECT_THAT(target[2], Eq(3));
    }

    TEST(array_aggregate, deduces_its_template_arguments) {
        array values = {1, 2, 3};
        static_assert(std::same_as<decltype(values), array<int, 3>>);

        array singleton = {1};
        static_assert(std::same_as<decltype(singleton), array<int, 1>>);

        // The guide's requires-clause rejects mixed element types rather than letting argument
        // order silently pick T and then failing on narrowing.
        static_assert(deducible<int, int, int>);
        static_assert(!deducible<int, double>);

        EXPECT_THAT(values, ElementsAre(1, 2, 3));
    }

    TEST(array_aggregate, is_usable_as_a_non_type_template_parameter) {
        // Public non-mutable members of structural type make the class structural.
        static_assert(1 == nttp_probe<array{1, 2, 3}>::first);

        SUCCEED();
    }

    // ── Overloaded Operators Tests ───────────────────────────────────────────────────────────────────────────────────
    TEST(array_operators, random_access_mut_overload_returns_mut_ref) {
        static_assert(std::same_as<decltype(std::declval<array<int, 3>&>()[0]), array<int, 3>::reference>);
        static_assert(!std::is_const_v<std::remove_reference_t<decltype(std::declval<array<int, 3>&>()[0])>>);

        SUCCEED();
    }

    TEST(array_operators, random_access_mut_overload_is_noexcept) {
        static_assert(noexcept(std::declval<array<int, 3>&>()[0]));

        SUCCEED();
    }

    TEST(array_operators, random_access_mut_overload_is_usable_in_constant_expressions) {
        // A `constexpr` object is const, so it can never select the mutable overload.
        // The mutation has to happen inside the constant evaluation.
        static_assert([] -> bool {
            array values = {1, 2, 3};
            values[0] = 10;
            return 10 == values[0] && 2 == values[1] && 3 == values[2];
        }());

        SUCCEED();
    }

    TEST(array_operators, random_access_mut_overload_aliases_storage) {
        array values = {1, 2, 3};

        const int& first = values[0];
        int& second = values[1];

        // Contiguous adjacent elements: this is what pins down "reference" rather than "value".
        EXPECT_THAT(&second - &first, Eq(1));

        second = 20;
        EXPECT_THAT(values[1], Eq(20));
    }

    TEST(array_operators, random_access_const_overload_returns_const_ref) {
        static_assert(std::same_as<decltype(std::declval<const array<int, 3>&>()[0]),
                                   array<int, 3>::const_reference>);
        static_assert(std::is_const_v<std::remove_reference_t<decltype(std::declval<const array<int, 3>&>()[0])>>);

        SUCCEED();
    }

    TEST(array_operators, random_access_const_overload_is_noexcept) {
        static_assert(noexcept(std::declval<const array<int, 3>&>()[0]));

        SUCCEED();
    }

    TEST(array_operators, random_access_const_overload_is_usable_in_constant_expressions) {
        constexpr array values = {1, 2, 3};
        static_assert(1 == values[0]);
        static_assert(2 == values[1]);
        static_assert(3 == values[2]);

        SUCCEED();
    }

    TEST(array_operators, random_access_const_overload_aliases_storage) {
        constexpr array values = {1, 2, 3};

        const int& first = values[0];
        const int& second = values[1];

        EXPECT_THAT(&second - &first, Eq(1));
    }

    TEST(array_operators, random_access_const_overload_does_not_copy_a_non_trivial_element) {
        const array values = {std::string("alpha"), std::string("beta")};

        EXPECT_THAT(values[0], Eq("alpha"));

        // A returned value_type rather than const_reference would fail this: the address would be a temporary's.
        EXPECT_THAT(&values[0], Eq(values.data()));
    }

    // ── Method Tests ─────────────────────────────────────────────────────────────────────────────────────────────────
    TEST(array_methods, at_mut_overload_returns_mut_ref) {
        static_assert(std::same_as<decltype(std::declval<array<int, 3>&>().at(0)), array<int, 3>::reference>);
        static_assert(!std::is_const_v<std::remove_reference_t<decltype(std::declval<array<int, 3>&>().at(0))>>);

        SUCCEED();
    }

    TEST(array_methods, at_mut_overload_is_not_noexcept) {
        static_assert(!noexcept(std::declval<array<int, 3>&>().at(0)));

        SUCCEED();
    }

    TEST(array_methods, at_mut_overload_does_not_throw) {
        // Mutable object inside the constant evaluation, so the mutable overload is actually selected.
        static_assert([] -> bool {
            array values = {1, 2, 3};
            values.at(0) = 10;
            return 10 == values.at(0) && 2 == values.at(1) && 3 == values.at(2);
        }());

        array values = {1, 2, 3};
        EXPECT_NO_THROW(static_cast<void>(values.at(0)));
        EXPECT_NO_THROW(static_cast<void>(values.at(2)));

        values.at(2) = 30;
        EXPECT_THAT(values[2], Eq(30));
        EXPECT_THAT(&values.at(2), Eq(&values[2]));
    }

    TEST(array_methods, at_mut_overload_throws) {
        // Must be non-const, or the const overload is selected and this duplicates at_const_overload_throws.
        array values = {1, 2, 3};

        EXPECT_THROW(static_cast<void>(values.at(3)), std::out_of_range);
        EXPECT_THROW(static_cast<void>(values.at(std::numeric_limits<array<int, 3>::size_type>::max())),
                     std::out_of_range);
    }

    TEST(array_methods, at_const_overload_returns_const_ref) {
        static_assert(std::same_as<decltype(std::declval<const array<int, 3>&>().at(0)),
                                   array<int, 3>::const_reference>);
        static_assert(std::is_const_v<std::remove_reference_t<decltype(std::declval<const array<int, 3>&>().at(0))>>);

        SUCCEED();
    }

    TEST(array_methods, at_const_overload_is_not_noexcept) {
        static_assert(!noexcept(std::declval<const array<int, 3>&>().at(0)));

        SUCCEED();
    }

    TEST(array_methods, at_const_overload_does_not_throw) {
        constexpr array values = {1, 2, 3};
        static_assert(1 == values.at(0));
        static_assert(2 == values.at(1));
        static_assert(3 == values.at(2));

        EXPECT_NO_THROW(static_cast<void>(values.at(0)));
        EXPECT_NO_THROW(static_cast<void>(values.at(2)));
        EXPECT_THAT(&values.at(2), Eq(&values[2]));
    }

    TEST(array_methods, at_const_overload_throws) {
        // An out-of-range `at` in a constant expression is a compile error, not a catchable throw,
        // so the out-of-range path is only testable at runtime.
        constexpr array values = {1, 2, 3};

        EXPECT_THROW(static_cast<void>(values.at(3)), std::out_of_range);
        EXPECT_THROW(static_cast<void>(values.at(std::numeric_limits<array<int, 3>::size_type>::max())),
                     std::out_of_range);
    }

    TEST(array_methods, at_noexcept_mut_overload_expected_return_value) {
        static_assert(noexcept(std::declval<array<int, 3>&>().at_noexcept(0)));
        static_assert(std::same_as<decltype(std::declval<array<int, 3>&>().at_noexcept(0)),
                                   std::expected<std::reference_wrapper<int>, array<int, 3>::array_error>>);

        // Cannot be `constexpr auto`: the result holds a reference to `values`, which has automatic
        // storage duration, and a pointer to the stack is not a permitted result of a constant expression.
        array values = {1, 2, 3};
        const auto result = values.at_noexcept(0);

        ASSERT_TRUE(result.has_value());
        EXPECT_THAT(result->get(), Eq(1));
        EXPECT_THAT(&result->get(), Eq(&values[0]));

        // The referenced element is mutable through the wrapper.
        result->get() = 10;
        EXPECT_THAT(values[0], Eq(10));

        // The array is created inside the evaluation here, so its address is usable within it.
        static_assert([] -> bool {
            array inner = {1, 2, 3};
            const auto ok = inner.at_noexcept(2);
            return ok.has_value() && 3 == ok->get();
        }());
    }

    TEST(array_methods, at_noexcept_mut_overload_unexpected_return_value) {
        array values = {1, 2, 3};

        constexpr auto result = values.at_noexcept(3);
        ASSERT_FALSE(result.has_value());
        EXPECT_THAT(result.error(), Eq(array<int, 3>::array_error::out_of_range));

        constexpr auto far_result = values.at_noexcept(std::numeric_limits<array<int, 3>::size_type>::max());
        ASSERT_FALSE(far_result.has_value());
        EXPECT_THAT(far_result.error(), Eq(array<int, 3>::array_error::out_of_range));

        // The failure path must not throw, which is the entire point of this overload.
        EXPECT_NO_THROW(static_cast<void>(values.at_noexcept(3)));

        static_assert([] -> bool {
            array inner = {1, 2, 3};
            constexpr auto err = inner.at_noexcept(3);
            return array<int, 3>::array_error::out_of_range == err.error();
        }());
    }

    TEST(array_methods, at_noexcept_const_overload_expected_return_value) {
        static_assert(noexcept(std::declval<const array<int, 3>&>().at_noexcept(0)));
        static_assert(std::same_as<decltype(std::declval<const array<int, 3>&>().at_noexcept(0)),
                                   std::expected<std::reference_wrapper<const int>, array<int, 3>::array_error>>);

        // `static` gives the object static storage duration, so &values.values_[0] IS a constant
        // and the result may escape into a constexpr variable.
        static constexpr array values = {1, 2, 3};
        constexpr auto result = values.at_noexcept(0);

        static_assert(result.has_value());
        static_assert(1 == result->get());
        static_assert(std::is_const_v<std::remove_reference_t<decltype(result->get())>>);
        static_assert(2 == values.at_noexcept(1)->get());

        EXPECT_THAT(result->get(), Eq(1));
        EXPECT_THAT(&result->get(), Eq(values.data()));
    }

    TEST(array_methods, at_noexcept_const_overload_unexpected_return_value) {
        // No address in the error path, so this one needs no static storage.
        constexpr array values = {1, 2, 3};

        static_assert(!values.at_noexcept(3).has_value());
        static_assert(array<int, 3>::array_error::out_of_range == values.at_noexcept(3).error());

        constexpr auto result = values.at_noexcept(3);
        ASSERT_FALSE(result.has_value());
        EXPECT_THAT(result.error(), Eq(array<int, 3>::array_error::out_of_range));
        EXPECT_NO_THROW(static_cast<void>(values.at_noexcept(3)));
    }

    TEST(array_methods, front_mut_overload_single_element_array) {
        static_assert(std::same_as<decltype(std::declval<array<int, 1>&>().front()), array<int, 1>::reference>);
        static_assert(noexcept(std::declval<array<int, 1>&>().front()));

        static_assert([] -> bool {
            array values = {1};
            values.front() = 10;
            return 10 == values.front() && 10 == values[0];
        }());

        array values = {1};
        EXPECT_THAT(values.front(), Eq(1));
        EXPECT_THAT(&values.front(), Eq(values.data()));

        // With one element, front and back are the same object.
        EXPECT_THAT(&values.front(), Eq(&values.back()));

        values.front() = 10;
        EXPECT_THAT(values[0], Eq(10));
    }

    TEST(array_methods, front_mut_overload_multi_element_array) {
        static_assert(std::same_as<decltype(std::declval<array<int, 3>&>().front()), array<int, 3>::reference>);
        static_assert(noexcept(std::declval<array<int, 3>&>().front()));

        static_assert([] -> bool {
            array values = {1, 2, 3};
            values.front() = 10;
            return 10 == values.front() && 10 == values[0] && 2 == values[1] && 3 == values[2];
        }());

        array values = {1, 2, 3};
        EXPECT_THAT(values.front(), Eq(1));
        EXPECT_THAT(&values.front(), Eq(values.data()));
        EXPECT_THAT(&values.front(), Ne(&values.back()));

        values.front() = 10;
        EXPECT_THAT(values, ElementsAre(10, 2, 3));
    }

    TEST(array_methods, front_const_overload_single_element_array) {
        static_assert(std::same_as<decltype(std::declval<const array<int, 1>&>().front()),
                                   array<int, 1>::const_reference>);
        static_assert(std::is_const_v<
            std::remove_reference_t<decltype(std::declval<const array<int, 1>&>().front())>>);
        static_assert(noexcept(std::declval<const array<int, 1>&>().front()));

        constexpr array values = {1};
        static_assert(1 == values.front());
        static_assert(values.front() == values.back());

        EXPECT_THAT(values.front(), Eq(1));
        EXPECT_THAT(&values.front(), Eq(&values[0]));
        EXPECT_THAT(&values.front(), Eq(&values.back()));
    }

    TEST(array_methods, front_const_overload_multi_element_array) {
        static_assert(std::same_as<decltype(std::declval<const array<int, 3>&>().front()),
                                   array<int, 3>::const_reference>);
        static_assert(std::is_const_v<
            std::remove_reference_t<decltype(std::declval<const array<int, 3>&>().front())>>);
        static_assert(noexcept(std::declval<const array<int, 3>&>().front()));

        constexpr array values = {1, 2, 3};
        static_assert(1 == values.front());
        static_assert(values.front() == values[0]);

        EXPECT_THAT(values.front(), Eq(1));
        EXPECT_THAT(&values.front(), Eq(values.data()));
        EXPECT_THAT(&values.front(), Ne(&values.back()));
    }

    TEST(array_methods, back_mut_overload_single_element_array) {
        static_assert(std::same_as<decltype(std::declval<array<int, 1>&>().back()), array<int, 1>::reference>);
        static_assert(noexcept(std::declval<array<int, 1>&>().back()));

        static_assert([] -> bool {
            array values = {1};
            values.back() = 10;
            return 10 == values.back() && 10 == values.front();
        }());

        array values = {1};
        EXPECT_THAT(values.back(), Eq(1));
        EXPECT_THAT(&values.back(), Eq(values.data()));
        EXPECT_THAT(&values.back(), Eq(&values.front()));

        values.back() = 10;
        EXPECT_THAT(values[0], Eq(10));
    }

    TEST(array_methods, back_mut_overload_multi_element_array) {
        static_assert(std::same_as<decltype(std::declval<array<int, 3>&>().back()), array<int, 3>::reference>);
        static_assert(noexcept(std::declval<array<int, 3>&>().back()));

        static_assert([] -> bool {
            array values = {1, 2, 3};
            values.back() = 30;
            return 30 == values.back() && 1 == values[0] && 2 == values[1] && 30 == values[2];
        }());

        array values = {1, 2, 3};

        // back() must be N-1, not N. This is the off-by-one that a value-only check would miss.
        EXPECT_THAT(values.back(), Eq(3));
        EXPECT_THAT(&values.back(), Eq(&values[2]));
        EXPECT_THAT(&values.back() - &values.front(), Eq(2));

        values.back() = 30;
        EXPECT_THAT(values, ElementsAre(1, 2, 30));
    }

    TEST(array_methods, back_const_overload_single_element_array) {
        static_assert(std::same_as<decltype(std::declval<const array<int, 1>&>().back()),
                                   array<int, 1>::const_reference>);
        static_assert(std::is_const_v<
            std::remove_reference_t<decltype(std::declval<const array<int, 1>&>().back())>>);
        static_assert(noexcept(std::declval<const array<int, 1>&>().back()));

        constexpr array values = {1};
        static_assert(1 == values.back());

        EXPECT_THAT(values.back(), Eq(1));
        EXPECT_THAT(&values.back(), Eq(values.data()));
        EXPECT_THAT(&values.back(), Eq(&values.front()));
    }

    TEST(array_methods, back_const_overload_multi_element_array) {
        static_assert(std::same_as<decltype(std::declval<const array<int, 3>&>().back()),
                                   array<int, 3>::const_reference>);
        static_assert(std::is_const_v<
            std::remove_reference_t<decltype(std::declval<const array<int, 3>&>().back())>>);
        static_assert(noexcept(std::declval<const array<int, 3>&>().back()));

        constexpr array values = {1, 2, 3};
        static_assert(3 == values.back());
        static_assert(values.back() == values[2]);

        EXPECT_THAT(values.back(), Eq(3));
        EXPECT_THAT(&values.back(), Eq(&values[2]));
        EXPECT_THAT(&values.back() - &values.front(), Eq(2));
    }

    TEST(array_methods, data_returns_the_address_of_the_first_element) {
        static_assert(std::same_as<decltype(std::declval<array<int, 3>&>().data()), array<int, 3>::pointer>);
        static_assert(std::same_as<decltype(std::declval<const array<int, 3>&>().data()),
                                   array<int, 3>::const_pointer>);
        static_assert(noexcept(std::declval<array<int, 3>&>().data()));

        array values = {1, 2, 3};
        EXPECT_THAT(values.data(), Eq(values.data()));
        EXPECT_THAT(values.data() + 2, Eq(&values[2]));
    }

    TEST(array_methods, size_and_max_size_and_empty_are_compile_time_constants) {
        static_assert(3 == array<int, 3>{}.size());
        static_assert(3 == array<int, 3>{}.max_size());
        static_assert(!array<int, 3>{}.empty());
        static_assert(1 == array<int, 1>{}.size());
        static_assert(!array<int, 1>{}.empty());

        static_assert(noexcept(std::declval<const array<int, 3>&>().size()));
        static_assert(std::same_as<decltype(std::declval<const array<int, 3>&>().size()),
                                   array<int, 3>::size_type>);

        SUCCEED();
    }

    TEST(array_methods, fill_sets_every_element) {
        static_assert([] -> bool {
            array<int, 3> values = {1, 2, 3};
            values.fill(7);
            return 7 == values[0] && 7 == values[1] && 7 == values[2];
        }());

        array values = {1, 2, 3};
        values.fill(7);
        EXPECT_THAT(values, ElementsAre(7, 7, 7));

        // Element-wise assignment, not a rebind: addresses are unchanged.
        const int* const before = values.data();
        values.fill(0);
        EXPECT_THAT(values.data(), Eq(before));
        EXPECT_THAT(values, ElementsAre(0, 0, 0));
    }

    TEST(array_methods, fill_noexcept_follows_the_element_assignment) {
        static_assert(noexcept(std::declval<array<int, 3>&>().fill(0)));

        // Copy-assigning a std::string can allocate; an unconditional noexcept here would be a lie.
        static_assert(!noexcept(std::declval<array<std::string, 2>&>().fill(std::declval<const std::string&>())));

        SUCCEED();
    }

    TEST(array_methods, swap_exchanges_contents_not_addresses) {
        static_assert([] -> bool {
            array first = {1, 2, 3};
            array second = {4, 5, 6};
            first.swap(second);
            return 4 == first[0] && 6 == first[2] && 1 == second[0] && 3 == second[2];
        }());

        array first = {1, 2, 3};
        array second = {4, 5, 6};

        const int* const first_data = first.data();
        const int* const second_data = second.data();

        first.swap(second);

        EXPECT_THAT(first, ElementsAre(4, 5, 6));
        EXPECT_THAT(second, ElementsAre(1, 2, 3));

        // Unlike vector, array::swap is element-wise: the storage does not move.
        EXPECT_THAT(first.data(), Eq(first_data));
        EXPECT_THAT(second.data(), Eq(second_data));
    }

    TEST(array_methods, swap_noexcept_follows_the_element_swap) {
        static_assert(noexcept(std::declval<array<int, 3>&>().swap(std::declval<array<int, 3>&>())));

        // std::string's swap happens to be noexcept, so this documents rather than constrains.
        static_assert(std::is_nothrow_swappable_v<std::string>);
        static_assert(noexcept(std::declval<array<std::string, 2>&>().swap(std::declval<array<std::string, 2>&>())));

        SUCCEED();
    }

    // ── Iterator Tests ───────────────────────────────────────────────────────────────────────────────────────────────
    TEST(array_iterators, satisfies_contiguous_range_mutably_and_constly) {
        static_assert(std::ranges::contiguous_range<array<int, 3>>);

        // This one fails without begin() const / end() const. cbegin()/cend() are not a substitute:
        // the range concepts look for begin()/end() on the const-qualified type.
        static_assert(std::ranges::contiguous_range<const array<int, 3>>);
        static_assert(std::ranges::sized_range<array<int, 3>>);

        static_assert(std::same_as<std::ranges::range_value_t<array<int, 3>>, int>);
        static_assert(std::same_as<decltype(std::declval<array<int, 3>&>().begin()), array<int, 3>::iterator>);
        static_assert(std::same_as<decltype(std::declval<const array<int, 3>&>().begin()),
                                   array<int, 3>::const_iterator>);

        SUCCEED();
    }

    TEST(array_iterators, begin_and_end_span_the_array) {
        static_assert([] -> bool {
            array values = {1, 2, 3};
            *values.begin() = 10;
            return 10 == values[0] && values.end() - values.begin() == 3;
        }());

        array values = {1, 2, 3};

        EXPECT_THAT(values.begin(), Eq(values.data()));
        EXPECT_THAT(values.end(), Eq(values.data() + 3));
        EXPECT_THAT(values.end() - values.begin(), Eq(3));
        EXPECT_THAT(*values.begin(), Eq(1));
        EXPECT_THAT(*(values.end() - 1), Eq(3));
    }

    TEST(array_iterators, a_const_array_is_iterable_with_a_range_for) {
        constexpr array values = {1, 2, 3};

        int sum = 0;
        for (const int value : values) {
            sum += value;
        }

        EXPECT_THAT(sum, Eq(6));
        EXPECT_THAT(values.begin(), Eq(values.data()));
        EXPECT_THAT(values.end() - values.begin(), Eq(3));
    }

    TEST(array_iterators, cbegin_and_cend_are_const_on_a_mutable_array) {
        array values = {1, 2, 3};

        static_assert(std::same_as<decltype(values.cbegin()), array<int, 3>::const_iterator>);
        static_assert(std::is_const_v<std::remove_reference_t<decltype(*values.cbegin())>>);

        EXPECT_THAT(values.cbegin(), Eq(values.data()));
        EXPECT_THAT(values.cend() - values.cbegin(), Eq(3));
    }

    TEST(array_iterators, rbegin_starts_at_the_last_element) {
        // rbegin() must wrap end(), not begin(). Wrapping begin() makes *rbegin() read values_[-1],
        // which returns plausible garbage rather than crashing.
        array values = {1, 2, 3};

        EXPECT_THAT(*values.rbegin(), Eq(3));
        EXPECT_THAT(*(values.rend() - 1), Eq(1));
        EXPECT_THAT(values.rend() - values.rbegin(), Eq(3));
        EXPECT_THAT(&*values.rbegin(), Eq(&values.back()));
        EXPECT_THAT((std::vector<int>(values.rbegin(), values.rend())), ElementsAre(3, 2, 1));

        *values.rbegin() = 30;
        EXPECT_THAT(values, ElementsAre(1, 2, 30));
    }

    TEST(array_iterators, crbegin_and_crend_walk_backwards_constly) {
        constexpr array values = {1, 2, 3};

        static_assert(std::same_as<decltype(values.crbegin()), array<int, 3>::const_reverse_iterator>);
        static_assert(std::is_const_v<std::remove_reference_t<decltype(*values.crbegin())>>);

        EXPECT_THAT(*values.crbegin(), Eq(3));
        EXPECT_THAT(*(values.crend() - 1), Eq(1));
        EXPECT_THAT(values.crend() - values.crbegin(), Eq(3));
        EXPECT_THAT((std::vector<int>(values.crbegin(), values.crend())), ElementsAre(3, 2, 1));
    }

} // namespace collections::array_testing