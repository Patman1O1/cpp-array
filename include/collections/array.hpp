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
        inline constexpr std::expected<void, std::error_code> _trivial_copy(const T* values,
                                                                            const std::size_t size) noexcept {
            if (size > N) {
                return std::unexpected<std::error_code>{std::make_error_code(std::errc::value_too_large)};
            }

            // Copy the values using memcpy (overflow is impossible at this point)
            std::memcpy(this->values_, values, N * sizeof(T));

            if (size < N) {
                std::memset(this->values_ + size, 0x00, (N - size) * sizeof(T));
            }

            return std::expected<void, std::error_code>{};
        }

        inline constexpr std::expected<void, std::error_code> _copy(const T* values, const std::size_t size) noexcept {
            if (size > N) {
                const std::error_code err = std::make_error_code(std::errc::value_too_large);
                std::println(std::cerr, "_copy: {}", err.message());
                return std::unexpected<std::error_code>{err};
            }

            // Copy the values iteratively (overflow is impossible at this point)
            for (std::size_t i = 0; i < size; i++) {
                this->values_[i] = values[i];
            }

            // Fill the remaining part of the array with the default value of T
            if (size < N) {
                for (std::size_t i = size; i < N; i++) {
                    this->values_[i] = T();
                }
            }

            return std::expected<void, std::error_code>{};
        }

    public:
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
        inline constexpr array& operator=(const array& lhs) noexcept {

        }

        inline constexpr array& operator=(array&& lhs) noexcept {

        }

        [[nodiscard]] constexpr bool operator==(const array& lhs) const noexcept {

        }

        [[nodiscard]] constexpr auto operator<=>(const array& lhs) const noexcept {

        }

        [[nodiscard]] inline constexpr reference operator[](const size_type index) {

        }

        [[nodiscard]] inline constexpr const_reference operator[](const size_type index) const {

        }

        // ── Methods ──────────────────────────────────────────────────────────────────────────────────────────────────
        [[nodiscard]] inline constexpr reference at(const size_type index) {

        }

        [[nodiscard]] inline constexpr const_reference at(const size_type index) const {

        }

        [[nodiscard]] inline constexpr reference front() {

        }

        [[nodiscard]] inline constexpr const_reference front() const {

        }

        [[nodiscard]] inline constexpr reference back() noexcept {

        }

        [[nodiscard]] inline constexpr const_reference back() const noexcept {

        }

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
