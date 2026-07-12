#ifndef COLLECTIONS_ARRAY_HPP
#define COLLECTIONS_ARRAY_HPP

// ISO C Includes
#include <cstddef>
#include <cstring>

// ISO C++ Includes
#include <algorithm>
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

    private:
        // ── Fields ───────────────────────────────────────────────────────────────────────────────────────────────────
        value_type values_[N];

        // ── Methods ──────────────────────────────────────────────────────────────────────────────────────────────────
        inline constexpr void _trivial_copy(const T* values, const std::size_t size) /* throws std::length_error */ {
            if (size > N) {
                throw std::length_error(std::format(
                    "\"size\", which is {}, exceeds the size of this array, which is {}", size, N
                    ));
            }

            // Copy the values using memcpy (overflow is impossible at this point)
            std::memcpy(this->values_, values, N * sizeof(T));

            if (size < N) {
                std::memset(this->values_ + size, 0x00, (N - size) * sizeof(T));
            }
        }

        inline constexpr void _copy(const T* values, const std::size_t size) {
            if (size > N) {
                throw std::length_error(std::format(
                    "\"size\", which is {}, exceeds the size of this array, which is {}", size, N
                    ));
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
        }
    public:
        // ── iterator ─────────────────────────────────────────────────────────────────────────────────────────────────
        class iterator {
        public:
            // ── Type Definitions ─────────────────────────────────────────────────────────────────────────────────────
            using iterator_category = std::contiguous_iterator_tag;

            using value_type = array::value_type;

            using size_type = array::size_type;

            using difference_type = array::difference_type;

            using pointer = array::pointer;

            using const_pointer = array::const_pointer;

            using reference = array::reference;

            using const_reference = array::const_reference;

        private:
            // ── Friends ──────────────────────────────────────────────────────────────────────────────────────────────
            friend class array;

            // ── Fields ───────────────────────────────────────────────────────────────────────────────────────────────
            pointer ptr_;

        public:
            // ── Constructors ─────────────────────────────────────────────────────────────────────────────────────────
            inline constexpr iterator() noexcept;

            inline constexpr iterator(const iterator& other) noexcept;

            inline constexpr iterator(iterator&& other) noexcept;

            // ── Destructor ───────────────────────────────────────────────────────────────────────────────────────────
            inline constexpr ~iterator() noexcept = default;

            // ── Overload Operators ───────────────────────────────────────────────────────────────────────────────────
            inline constexpr iterator& operator=(const iterator& lhs) noexcept {

            }

            inline constexpr iterator& operator=(iterator&& lhs) noexcept {

            }

            [[nodiscard]] inline constexpr bool operator==(const iterator& lhs) const noexcept {

            }

            [[nodiscard]] inline constexpr auto operator<=>(const iterator& lhs) const noexcept {

            }

            inline constexpr iterator& operator++() noexcept {

            }

            inline constexpr iterator operator++(int) noexcept {

            }

            inline constexpr iterator& operator--() noexcept {

            }

            inline constexpr iterator operator--(int) noexcept {

            }

            inline constexpr iterator& operator+=(const difference_type n) noexcept {

            }

            [[nodiscard]] inline constexpr iterator operator+(const difference_type n) const noexcept {

            }

            inline constexpr iterator& operator-=(const difference_type n) noexcept {

            }

            [[nodiscard]] inline constexpr iterator operator-(const difference_type n) const noexcept {

            }
        };

        // ── const_iterator ───────────────────────────────────────────────────────────────────────────────────────────
        class const_iterator {
        public:
            // ── Type Definitions ─────────────────────────────────────────────────────────────────────────────────────
            using iterator_category = std::contiguous_iterator_tag;

            using value_type = array::value_type;

            using size_type = array::size_type;

            using difference_type = array::difference_type;

            using pointer = array::pointer;

            using const_pointer = array::const_pointer;

            using reference = array::reference;

            using const_reference = array::const_reference;

        private:
            // ── Friends ──────────────────────────────────────────────────────────────────────────────────────────────
            friend class array;

            // ── Fields ───────────────────────────────────────────────────────────────────────────────────────────────
            pointer ptr_;

            // ── Constructors ─────────────────────────────────────────────────────────────────────────────────────────
            constexpr const_iterator() noexcept : ptr_(nullptr) {}
        };

        // ── reverse_iterator ─────────────────────────────────────────────────────────────────────────────────────────
        class reverse_iterator {
        public:
            // ── Type Definitions ─────────────────────────────────────────────────────────────────────────────────────
            using iterator_category = std::contiguous_iterator_tag;

            using value_type = array::value_type;

            using size_type = array::size_type;

            using difference_type = array::difference_type;

            using pointer = array::pointer;

            using const_pointer = array::const_pointer;

            using reference = array::reference;

            using const_reference = array::const_reference;

        private:
            // ── Friends ──────────────────────────────────────────────────────────────────────────────────────────────
            friend class array;

            // ── Fields ───────────────────────────────────────────────────────────────────────────────────────────────
            pointer ptr_;
        };

        // ── const_reverse_iterator ───────────────────────────────────────────────────────────────────────────────────
        class const_reverse_iterator {
        public:
            // ── Type Definitions ─────────────────────────────────────────────────────────────────────────────────────
            using iterator_category = std::contiguous_iterator_tag;

            using value_type = array::value_type;

            using size_type = array::size_type;

            using difference_type = array::difference_type;

            using pointer = array::pointer;

            using const_pointer = array::const_pointer;

            using reference = array::reference;

            using const_reference = array::const_reference;

        private:
            // ── Friends ──────────────────────────────────────────────────────────────────────────────────────────────
            friend class array;

            // ── Fields ───────────────────────────────────────────────────────────────────────────────────────────────
            pointer ptr_;
        };

        // ── Constructors ─────────────────────────────────────────────────────────────────────────────────────────────
        inline constexpr array() noexcept = default;

        inline constexpr array(const array& other) noexcept;

        inline constexpr array(array&& other) noexcept;

        inline constexpr array(std::initializer_list<value_type> values) noexcept;

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
