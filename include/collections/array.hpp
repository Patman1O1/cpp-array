#ifndef COLLECTIONS_ARRAY_HPP
#define COLLECTIONS_ARRAY_HPP

// ISO C Includes
#include <cstddef>
#include <cstring>

// ISO C++ Includes
#include <algorithm>
#include <expected>
#include <format>
#include <initializer_list>
#include <type_traits>
#include <stdexcept>

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

    	using iterator = pointer;

    	using const_iterator = const_pointer;

    	using reverse_iterator = std::reverse_iterator<iterator>;

        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    private:
        // ── Fields ───────────────────────────────────────────────────────────────────────────────────────────────────
        value_type values_[N];

        // ── Methods ──────────────────────────────────────────────────────────────────────────────────────────────────


    public:
        // ── array_error ──────────────────────────────────────────────────────────────────────────────────────────────
        enum class array_error : std::uint8_t {
            out_of_range,
        };

        // ── Constructors ─────────────────────────────────────────────────────────────────────────────────────────────
        inline constexpr array() noexcept = default;

        inline constexpr array(const array& other) noexcept = default;

        inline constexpr array(array&& other) noexcept = default;

        inline constexpr array(std::initializer_list<value_type> values) noexcept {
            if constexpr (std::is_trivially_copyable_v<value_type>) {
                std::memcpy(this->values_, values.data(), N * sizeof(value_type));
            } else {
                for (std::size_t i = 0; i < N; i++) {
                    this->values_[i] = values[i];
                }
            }
        }

        // ── Destructor ───────────────────────────────────────────────────────────────────────────────────────────────
        inline constexpr ~array() noexcept = default;

        // ── Overloaded Operators ─────────────────────────────────────────────────────────────────────────────────────
        inline constexpr array& operator=(const array& lhs) noexcept = default;

        inline constexpr array& operator=(array&& lhs) noexcept = default;

        [[nodiscard]] constexpr bool operator==(const array& lhs) const noexcept = default;

        [[nodiscard]] constexpr auto operator<=>(const array& lhs) const noexcept = default;

        [[nodiscard]] inline constexpr reference operator[](const size_type index) noexcept {
            return this->values_[index];
        }

        [[nodiscard]] inline constexpr const_reference operator[](const size_type index) const noexcept {
            return this->values_[index];
        }

        // ── Methods ──────────────────────────────────────────────────────────────────────────────────────────────────
        [[nodiscard]] inline constexpr reference at(const size_type index) /* throws std::out_of_range */ {
            if (index >= N) [[unlikely]] {
                throw std::out_of_range("collections::array::at index out of range");
            }
            return this->values_[index];
        }

        [[nodiscard]] inline constexpr const_reference at(const size_type index) const /* throws std::out_of_range */ {
            if (index >= N) [[unlikely]] {
                throw std::out_of_range("collections::array::at index out of range");
            }
            return this->values_[index];
        }

        [[nodiscard]] inline constexpr std::expected<std::reference_wrapper<value_type>, array_error> at_noexcept(const size_type index) noexcept {
            if (index >= N) [[unlikely]] {
                return std::unexpected(array_error::out_of_range);
            }
            return std::reference_wrapper<value_type>(this->values_[index]);
        }

        [[nodiscard]] inline constexpr std::expected<std::reference_wrapper<const value_type>, array_error> at_noexcept(const size_type index) const noexcept {
            if (index >= N) [[unlikely]] {
                return std::unexpected(array_error::out_of_range);
            }
            return std::reference_wrapper<value_type>(this->values_[index]);
        }

        [[nodiscard]] inline constexpr reference front() noexcept { return this->values_[0]; }

        [[nodiscard]] inline constexpr const_reference front() const { return this->values_[0]; }

        [[nodiscard]] inline constexpr reference back() noexcept { return this->values_[this->size() - 1]; }

        [[nodiscard]] inline constexpr const_reference back() const noexcept { return this->values_[this->size() - 1]; }

        [[nodiscard]] inline constexpr pointer data() noexcept {

        }

        [[nodiscard]] inline constexpr const_pointer data() const noexcept {

        }

        [[nodiscard]] inline constexpr iterator begin() {

        }

        [[nodiscard]] inline constexpr iterator end() {

        }

        [[nodiscard]] inline constexpr const_iterator cbegin() const {

        }

        [[nodiscard]] inline constexpr const_iterator cend() const {

        }

        [[nodiscard]] inline constexpr reverse_iterator rbegin() {

        }

        [[nodiscard]] inline constexpr reverse_iterator rend() {

        }

        [[nodiscard]] inline constexpr const_reverse_iterator crbegin() const {

        }

        [[nodiscard]] inline constexpr const_reverse_iterator crend() const {

        }

        [[nodiscard]] inline constexpr bool empty() const {

        }

        [[nodiscard]] inline constexpr size_type size() const {

        }

        [[nodiscard]] inline constexpr size_type max_size() const {

        }

        inline constexpr void fill(const_reference value) noexcept {}

        inline constexpr void swap(array& other) noexcept {

        }
    };

} // namespace collections

#endif // #ifndef COLLECTIONS_ARRAY_HPP
