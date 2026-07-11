#ifndef COLLECTIONS_ARRAY_HPP
#define COLLECTIONS_ARRAY_HPP

// ISO C Includes
#include <cstddef>
#include <cstring>

// ISO C++ Includes
#include <initializer_list>
#include <type_traits>
#include <algorithm>

namespace collections {
    template<typename T, std::size_t N>
    class array {
    public:
        // ── Type Definitions ─────────────────────────────────────────────────────────────────────────────────────────
        using value_type = T;

        using size_type = std::size_t;

        using difference_type = std::ptrdiff_t;

        using pointer = T*;

        using const_pointer = const T*;

        using reference = T&;

        using const_reference = const T&;

    private:
        // ── Fields ───────────────────────────────────────────────────────────────────────────────────────────────────
        value_type values_[N];

    public:
        // ── iterator ────────────────────────────────────────────────────────────────────────────────────────────────

        // ── const_iterator ──────────────────────────────────────────────────────────────────────────────────────────


        // ── Constructors ─────────────────────────────────────────────────────────────────────────────────────────────
        inline constexpr array() noexcept = default;

        // ── Overloaded Operators ─────────────────────────────────────────────────────────────────────────────────────


        // ── Methods ──────────────────────────────────────────────────────────────────────────────────────────────────
    };

    // ── Constructors ─────────────────────────────────────────────────────────────────────────────────────────────────

    // ── Overloaded Operators ─────────────────────────────────────────────────────────────────────────────────────────


    // ── Methods ──────────────────────────────────────────────────────────────────────────────────────────────────────

} // namespace collections

#endif // #ifndef COLLECTIONS_ARRAY_HPP
