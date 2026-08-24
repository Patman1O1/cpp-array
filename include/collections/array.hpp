#ifndef COLLECTIONS_ARRAY_HPP
#define COLLECTIONS_ARRAY_HPP

// ISO C Includes
#include <cstddef>
#include <cstdint>

// ISO C++ Includes
#include <algorithm>
#include <compare>
#include <concepts>
#include <expected>
#include <functional>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace collections {
    template<typename T, std::size_t N>
    class array {
    public:
        // ── Aliases ─────────────────────────────────────────────────────────
        using value_type = T;

        using size_type = std::size_t;

        using difference_type = std::ptrdiff_t;

        using pointer = value_type*;

        using const_pointer = const value_type*;

        using reference = value_type&;

        using const_reference = const value_type&;

        using iterator = pointer;

        using const_iterator = const_pointer;

        using reverse_iterator = std::reverse_iterator<iterator>;

        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // ── array_error ─────────────────────────────────────────────────────
        enum class array_error : std::uint8_t { out_of_range, };

        // ── Fields ──────────────────────────────────────────────────────────
        value_type values_[N];

        // ── Overloaded Operators ────────────────────────────────────────────
        [[nodiscard]]
        constexpr auto operator==(const array& rhs) const noexcept(
            noexcept(std::declval<value_type>() == std::declval<value_type>())
        ) -> bool {
            return std::equal(this->begin(), this->end(), rhs.begin());
        }

        [[nodiscard]]
        constexpr auto operator<=>(const array& rhs) const noexcept(
            noexcept(std::declval<value_type>() <=> std::declval<value_type>())
        ) -> std::compare_three_way_result_t<value_type> {
            return std::lexicographical_compare_three_way(
                this->values_, this->values_ + N,
                rhs.values_, rhs.values_ + N
            );
        }
        
        [[nodiscard]]
        constexpr auto operator[](
            const size_type index
        ) noexcept -> reference { return this->values_[index]; }

        [[nodiscard]]
        constexpr auto operator[](
            const size_type index
        ) const noexcept -> const_reference { return this->values_[index]; }

    private:
        // ── Methods ─────────────────────────────────────────────────────────
        template<
            std::contiguous_iterator Iterator = iterator,
            std::predicate<bool, value_type, value_type> Predicate
        >
        constexpr void _sort(
            Iterator first,
            Iterator last,
            Predicate pred
        ) { std::sort(first, last, pred); }

        template<
            std::contiguous_iterator Iterator = iterator,
            std::predicate<bool, value_type, value_type> Predicate
        >
        constexpr void _stable_sort(
            Iterator first,
            Iterator last,
            Predicate pred
        ) { std::stable_sort(first, last, pred); }

    public:
        // ── Methods ─────────────────────────────────────────────────────────
        [[nodiscard]]
        constexpr auto at(const size_type index) -> reference {
            if (index >= N) [[unlikely]] {
                throw std::out_of_range(
                    "collections::array::at index out of range"
                );
            }
            return this->values_[index];
        }

        [[nodiscard]]
        constexpr auto at(
            const size_type index
        ) const -> const_reference {
            if (index >= N) [[unlikely]] {
                throw std::out_of_range(
                    "collections::array::at index out of range"
                );
            }
            return this->values_[index];
        }
        [[nodiscard]]
        constexpr auto at_noexcept(
            const size_type index
        ) noexcept -> std::expected<
            std::reference_wrapper<value_type>, array_error
        > {
            if (index >= N) [[unlikely]] {
                return std::unexpected(array_error::out_of_range);
            }
            return std::reference_wrapper<value_type>(this->values_[index]);
        }

        [[nodiscard]]
        constexpr auto at_noexcept(
            const size_type index
        ) const noexcept -> std::expected<
            std::reference_wrapper<const value_type>, array_error
        > {
            if (index >= N) [[unlikely]] {
                return std::unexpected(array_error::out_of_range);
            }

            return std::reference_wrapper<const value_type>(
                this->values_[index]
            );
        }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto front() noexcept -> reference {
            return this->values_[0];
        }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto front() const noexcept -> const_reference {
            return this->values_[0];
        }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto back() noexcept -> reference {
            return this->values_[this->size() - 1];
        }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto back() const noexcept -> const_reference {
            return this->values_[this->size() - 1];
        }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto data() noexcept -> pointer { return this->values_; }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto data() const noexcept -> const_pointer {
            return this->values_;
        }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto begin() noexcept -> iterator { return this->values_; }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto end() noexcept -> iterator { return this->values_ + N; }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto begin() const noexcept -> const_iterator {
            return this->values_;
        }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto end() const noexcept -> const_iterator {
            return this->values_ + N;
        }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto cbegin() const noexcept -> const_iterator {
            return this->values_;
        }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto cend() const noexcept -> const_iterator {
            return this->values_ + N;
        }

        [[nodiscard]]
        constexpr auto rbegin() noexcept -> reverse_iterator {
            return reverse_iterator(this->end());
        }

        [[nodiscard]]
        constexpr auto rend() noexcept -> reverse_iterator {
            return reverse_iterator(this->begin());
        }

        [[nodiscard]]
        constexpr auto rbegin() const noexcept -> const_reverse_iterator {
            return const_reverse_iterator(this->end());
        }

        [[nodiscard]]
        constexpr auto rend() const noexcept -> const_reverse_iterator {
            return const_reverse_iterator(this->begin());
        }

        [[nodiscard]]
        constexpr auto crbegin() const noexcept -> const_reverse_iterator {
            return this->rbegin();
        }

        [[nodiscard]]
        constexpr auto crend() const noexcept -> const_reverse_iterator {
            return this->rend();
        }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto empty() const noexcept -> bool { return N == 0; }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto size() const noexcept -> size_type { return N; }

        [[nodiscard]]
        [[gnu::always_inline]]
        constexpr auto max_size() const noexcept -> size_type { return N; }

        constexpr void fill(
            const_reference value
        ) noexcept(
            noexcept(std::is_nothrow_copy_assignable_v<value_type>)
        ) { std::fill(this->begin(), this->end(), value); }

        constexpr void swap(
            array& other
        ) noexcept(
            noexcept(std::is_nothrow_swappable_v<value_type>)
        ) { std::swap(this->values_, other.values_); }

        constexpr void sort() {
            this->_sort(this->begin(), this->end(), std::less<value_type>{});
        }

        template<std::contiguous_iterator Iterator = iterator>
        constexpr void sort(Iterator first, Iterator last) {
            if (first == last) [[unlikely]] {
                return;
            }
            this->_sort(first, last, std::less<value_type>{});
        }

        template<
            std::contiguous_iterator Iterator = iterator,
            std::predicate<bool, value_type, value_type> Predicate
        >
        constexpr void sort(Iterator first, Iterator last, Predicate pred) {
            if (first == last) [[unlikely]] {
                return;
            }
            this->_sort(first, last, pred);
        }


        constexpr void stable_sort() {
            this->_stable_sort(
                this->begin(),
                this->end(),
                std::less<value_type>{}
            );
        }

        constexpr void stable_sort(iterator first, iterator last) {
            if (first == last) [[unlikely]] {
                return;
            }
            this->_stable_sort(first, last, std::less<value_type>{});
        }

        template<std::predicate<bool, value_type, value_type> Predicate>
        constexpr void stable_sort(
            iterator first,
            iterator last,
            Predicate pred
        ) {
            if (first == last) [[unlikely]] {
                return;
            }
            this->_stable_sort(first, last, pred);
        }
    };

    // ── Deduction Guides ────────────────────────────────────────────────────────────────────────
    template<typename T, typename... U> requires (std::same_as<T, U> && ...)
    array(T, U...) -> array<T, 1 + sizeof...(U)>;
} // namespace collections
#endif // #ifndef ARRAY_HPP