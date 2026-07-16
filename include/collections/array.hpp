#ifndef COLLECTIONS_ARRAY_HPP
#define COLLECTIONS_ARRAY_HPP

// ISO C Includes
#include <cstddef>
#include <cstring>

// ISO C++ Includes
#include <algorithm>
#include <expected>
#include <stdexcept>
#include <type_traits>

namespace collections {
    template<typename T, std::size_t N>
    struct array {
        // ── Type Definitions ─────────────────────────────────────────────────────────────────────────────────────────
        using value_type = T;

        using size_type = std::size_t;

        using difference_type = std::ptrdiff_t;

        using pointer = T*;

        using const_pointer = const T*;

        using reference = T&;

        using const_reference = const T&;

    	using iterator = pointer;

    	using const_iterator = const_pointer;

    	using reverse_iterator = std::reverse_iterator<iterator>;

        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // ── Fields ───────────────────────────────────────────────────────────────────────────────────────────────────
        value_type values_[N];

        // ── array_error ──────────────────────────────────────────────────────────────────────────────────────────────
        enum class array_error : std::uint8_t {
            out_of_range,
        };

        // ── Constructors ─────────────────────────────────────────────────────────────────────────────────────────────

        // ── Destructor ───────────────────────────────────────────────────────────────────────────────────────────────

        // ── Overloaded Operators ─────────────────────────────────────────────────────────────────────────────────────
        [[nodiscard]] constexpr auto operator==(const array&) const -> bool = default;

        [[nodiscard]] constexpr auto operator<=>(const array&) const = default;

        [[nodiscard]] constexpr auto operator[](const size_type index) noexcept -> reference {
            return this->values_[index];
        }

        [[nodiscard]] constexpr auto operator[](const size_type index) const noexcept -> const_reference {
            return this->values_[index];
        }

        // ── Methods ──────────────────────────────────────────────────────────────────────────────────────────────────
        [[nodiscard]] constexpr auto at(const size_type index) -> reference /* throws std::out_of_range */ {
            if (index >= N) [[unlikely]] {
                throw std::out_of_range("collections::array::at index out of range");
            }
            return this->values_[index];
        }

        [[nodiscard]] constexpr auto at(const size_type index) const -> const_reference /* throws std::out_of_range */ {
            if (index >= N) [[unlikely]] {
                throw std::out_of_range("collections::array::at index out of range");
            }
            return this->values_[index];
        }

        [[nodiscard]] constexpr auto at_noexcept(const size_type index) noexcept -> std::expected<std::reference_wrapper<value_type>, array_error> {
            if (index >= N) [[unlikely]] {
                return std::unexpected(array_error::out_of_range);
            }
            return std::reference_wrapper<value_type>(this->values_[index]);
        }

        [[nodiscard]] constexpr auto at_noexcept(const size_type index) const noexcept -> std::expected<std::reference_wrapper<const value_type>, array_error> {
            if (index >= N) [[unlikely]] {
                return std::unexpected(array_error::out_of_range);
            }
            return std::reference_wrapper<const value_type>(this->values_[index]);
        }

        [[nodiscard]] constexpr auto front() noexcept -> reference { return this->values_[0]; }

        [[nodiscard]] constexpr auto front() const noexcept -> const_reference { return this->values_[0]; }

        [[nodiscard]] constexpr auto back() noexcept -> reference { return this->values_[this->size() - 1]; }

        [[nodiscard]] constexpr auto back() const noexcept -> const_reference { return this->values_[this->size() - 1]; }

        [[nodiscard]] constexpr auto data() noexcept -> pointer { return this->values_; }

        [[nodiscard]] constexpr auto data() const noexcept -> const_pointer { return this->values_; }

        [[nodiscard]] constexpr auto begin() noexcept -> iterator { return this->values_; }

        [[nodiscard]] constexpr auto end() noexcept -> iterator { return this->values_ + N; }

        [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator { return this->values_; }

        [[nodiscard]] constexpr auto end() const noexcept -> const_iterator { return this->values_ + N; }

        [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator { return this->values_; }

        [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator { return this->values_ + N; }

        [[nodiscard]] constexpr auto rbegin() noexcept -> reverse_iterator { return reverse_iterator(this->end()); }

        [[nodiscard]] constexpr auto rend() noexcept -> reverse_iterator { return reverse_iterator(this->begin()); }

        [[nodiscard]] constexpr auto rbegin() const noexcept -> const_reverse_iterator {
            return const_reverse_iterator(this->end());
        }

        [[nodiscard]] constexpr auto rend() const noexcept -> const_reverse_iterator {
            return const_reverse_iterator(this->begin());
        }

        [[nodiscard]] constexpr auto crbegin() const noexcept -> const_reverse_iterator { return this->rbegin(); }

        [[nodiscard]] constexpr auto crend() const noexcept -> const_reverse_iterator { return this->rend(); }

        [[nodiscard]] constexpr auto empty() const noexcept -> bool { return N == 0; }

        [[nodiscard]] constexpr auto size() const noexcept -> size_type { return N; }

        [[nodiscard]] constexpr auto max_size() const noexcept -> size_type { return N; }

        constexpr void fill(const_reference value) noexcept(std::is_nothrow_copy_assignable_v<value_type>) {
            std::fill(this->begin(), this->end(), value);
        }
        constexpr void swap(array& other) noexcept(std::is_nothrow_swappable_v<value_type>) {
            std::swap(this->values_, other.values_);
        }
        constexpr void sort() noexcept {}

        constexpr void stable_sort() noexcept {}
    };

    // ── Deduction Guides ─────────────────────────────────────────────────────────────────────────────────────────────
    template <typename T, typename... U> requires (std::same_as<T, U> && ...)
    array(T, U...) -> array<T, 1 + sizeof...(U)>;

} // namespace collections

#endif // #ifndef COLLECTIONS_ARRAY_HPP
