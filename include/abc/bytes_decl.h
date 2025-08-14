// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_BASIC_BYTES_DECL
#define ABC_INCLUDE_ABC_BASIC_BYTES_DECL

#pragma once

#include "bytes_fwd_decl.h"

#include "byte.h"
#include "bytes_view.h"
#include "expected.h"

namespace abc
{

template <ByteNumbering ByteNumberingV>
class BasicBytes
{
private:
    template <ByteNumbering RhsByteNumberingVV>
    friend class BasicBytes;

    using internal_type = std::vector<byte_t>;

    internal_type data_{};

public:
    using value_type = internal_type::value_type;
    using allocator_type = internal_type::allocator_type;
    using size_type = internal_type::size_type;
    using difference_type = internal_type::difference_type;
    using reference = internal_type::reference;
    using const_reference = internal_type::const_reference;
    using pointer = internal_type::pointer;
    using const_pointer = internal_type::const_pointer;
    using iterator = internal_type::iterator;
    using const_iterator = internal_type::const_iterator;
    using reverse_iterator = internal_type::reverse_iterator;
    using const_reverse_iterator = internal_type::const_reverse_iterator;

    constexpr static ByteNumbering byte_numbering_v{ ByteNumberingV };

    // public constructors

    BasicBytes() = default;

    template <ByteNumbering RhsByteNumberingV>
        requires(RhsByteNumberingV != ByteNumberingV && RhsByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
    constexpr explicit BasicBytes(BasicBytes<RhsByteNumberingV> && rhs);

    template <ByteNumbering RhsByteNumberV>
        requires(RhsByteNumberV != ByteNumberingV && (RhsByteNumberV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None))
    constexpr explicit BasicBytes(BasicBytes<RhsByteNumberV> && rhs) noexcept;

    constexpr explicit BasicBytes(bytes_view<ByteNumberingV> view);

    template <ByteNumbering ViewByteNumberingV>
        requires(ViewByteNumberingV != ByteNumberingV && ViewByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
    constexpr explicit BasicBytes(bytes_view<ViewByteNumberingV> view);

    template <ByteNumbering ViewByteNumberingV>
        requires(ViewByteNumberingV != ByteNumberingV && (ViewByteNumberingV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None))
    constexpr explicit BasicBytes(bytes_view<ViewByteNumberingV> view);

    constexpr explicit BasicBytes(std::string_view str)
        requires(ByteNumberingV == ByteNumbering::None);

    constexpr BasicBytes(std::initializer_list<value_type> il);

    // constexpr auto
    // operator=(std::vector<byte_t> raw) noexcept -> BasicBytes & requires(ByteNumbering == ByteNumbering::none);

    template <ByteNumbering RhsByteNumberingV>
        requires(RhsByteNumberingV != ByteNumberingV && RhsByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
    constexpr auto operator=(BasicBytes<RhsByteNumberingV> && rhs) -> BasicBytes &;

    template <ByteNumbering RhsByteNumberV>
        requires(RhsByteNumberV != ByteNumberingV && (RhsByteNumberV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None))
    constexpr auto operator=(BasicBytes<RhsByteNumberV> && rhs) noexcept -> BasicBytes &;

    constexpr auto operator=(bytes_view<ByteNumberingV> view) -> BasicBytes &;

    template <ByteNumbering ViewByteNumberingV>
        requires(ViewByteNumberingV != ByteNumberingV && ViewByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
    constexpr auto operator=(bytes_view<ViewByteNumberingV> view) -> BasicBytes &;

    template <ByteNumbering ViewByteNumberingV>
        requires(ViewByteNumberingV != ByteNumberingV && (ViewByteNumberingV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None))
    constexpr auto operator=(bytes_view<ViewByteNumberingV> view) -> BasicBytes &;

    constexpr auto operator=(std::initializer_list<value_type> il) -> BasicBytes &;

private:
    template <std::input_iterator Iterator, ByteNumbering SrcByteNumberingV>
        requires(SrcByteNumberingV == ByteNumberingV || SrcByteNumberingV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None)
    constexpr BasicBytes(Iterator begin, Iterator end, ByteNumberingType<SrcByteNumberingV>);

    template <std::input_iterator Iterator, ByteNumbering SrcByteNumberingV>
        requires(SrcByteNumberingV != ByteNumberingV && SrcByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
    constexpr BasicBytes(Iterator begin, Iterator end, ByteNumberingType<SrcByteNumberingV>);

    template <ByteNumbering SrcByteNumberingV>
        requires(SrcByteNumberingV == ByteNumberingV || SrcByteNumberingV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None)
    constexpr BasicBytes(std::initializer_list<value_type> il, ByteNumberingType<SrcByteNumberingV>);

    template <ByteNumbering SrcByteNumberingV>
        requires(SrcByteNumberingV != ByteNumberingV && SrcByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
    constexpr BasicBytes(std::initializer_list<value_type> il, ByteNumberingType<SrcByteNumberingV>);

public:
    template <std::integral T>
    constexpr static auto from(T i) -> BasicBytes;

    template <ByteNumbering DataByteNumberingV, std::input_iterator InputIterator>
    constexpr static auto from(InputIterator begin, InputIterator end) -> BasicBytes;

    template <ByteNumbering ViewByteNumberingV>
    constexpr static auto from(bytes_view<ViewByteNumberingV> view) -> BasicBytes;

    template <ByteNumbering DataByteNumberingV>
    constexpr static auto from(std::initializer_list<byte_t> il) -> BasicBytes;

    template <ByteNumbering RhsByteNumberingV>
    constexpr static auto from(BasicBytes<RhsByteNumberingV> && rhs) -> BasicBytes;

    constexpr static auto from(std::string_view str) -> BasicBytes
        requires(ByteNumberingV == ByteNumbering::None);

    template <std::integral T>
        requires(ByteNumberingV != ByteNumbering::None)
    constexpr auto to() const noexcept -> T;

    template <ByteNumbering ToByteNumberingV>
        requires(ByteNumberingV != ToByteNumberingV)
    constexpr auto to() const & -> BasicBytes<ToByteNumberingV>;

    template <ByteNumbering ToByteNumberingV>
        requires(ByteNumberingV != ToByteNumberingV)
    constexpr auto to() && -> BasicBytes<ToByteNumberingV>;

    constexpr auto assign(size_type count, byte_t const & value) -> void;

    [[nodiscard]] constexpr auto at(size_type pos) -> reference;

    [[nodiscard]] constexpr auto at(size_type pos) const -> const_reference;

    [[nodiscard]] constexpr auto operator[](size_type pos) -> reference;

    [[nodiscard]] constexpr auto operator[](size_type pos) const -> const_reference;

    [[nodiscard]] constexpr auto front() -> reference;

    [[nodiscard]] constexpr auto front() const -> const_reference;

    [[nodiscard]] constexpr auto back() -> reference;

    [[nodiscard]] constexpr auto back() const -> const_reference;

    [[nodiscard]] constexpr auto data() noexcept -> value_type *;

    [[nodiscard]] constexpr auto data() const noexcept -> value_type const *;

    [[nodiscard]] constexpr auto begin() noexcept -> iterator;

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto end() noexcept -> iterator;

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto empty() const noexcept -> bool;

    [[nodiscard]] constexpr auto size() const noexcept -> size_type;

    constexpr auto resize(size_type new_size) -> void;

    constexpr void reserve(size_type new_cap);

    constexpr void clear() noexcept;

    constexpr void push_back(value_type value);

    constexpr void pop_back();

    constexpr void swap(BasicBytes & other) noexcept;

    constexpr explicit operator std::vector<byte_t> const &() const noexcept
        requires(ByteNumberingV == ByteNumbering::None);

    constexpr explicit operator std::vector<byte_t> &() noexcept
        requires(ByteNumberingV == ByteNumbering::None);

    [[nodiscard]] constexpr auto least_significant_byte() const -> const_reference
        requires(ByteNumberingV != ByteNumbering::None);

    [[nodiscard]] constexpr auto least_significant_byte() -> reference
        requires(ByteNumberingV != ByteNumbering::None);

    [[nodiscard]] constexpr auto most_significant_byte() const -> const_reference
        requires(ByteNumberingV != ByteNumbering::None);

    [[nodiscard]] constexpr auto most_significant_byte() -> reference
        requires(ByteNumberingV != ByteNumbering::None);

    [[nodiscard]] constexpr auto first(size_t count) const noexcept -> bytes_view<ByteNumberingV>;

    [[nodiscard]] constexpr auto last(size_t count) const noexcept -> bytes_view<ByteNumberingV>;

    auto subview(size_type pos, size_type n = static_cast<size_type>(-1)) const -> expected<bytes_view<ByteNumberingV>, std::error_code>;

    constexpr auto view() const noexcept -> bytes_view<ByteNumberingV>;

    constexpr auto operator+(byte_t other) const -> BasicBytes;

    constexpr auto operator+=(byte_t other) -> BasicBytes &;

    constexpr auto operator+(bytes_view<ByteNumberingV> other) const -> BasicBytes;

    constexpr auto operator+=(bytes_view<ByteNumberingV> other) -> BasicBytes &;

    constexpr operator bytes_view<ByteNumberingV>() const noexcept;

private:
    friend constexpr auto operator==(BasicBytes const & lhs, BasicBytes const & rhs) noexcept -> bool = default;

    friend constexpr auto operator<=>(BasicBytes const & lhs, BasicBytes const & rhs) noexcept -> std::strong_ordering = default;
};

[[nodiscard]] constexpr auto operator<=>(BasicBytes<ByteNumbering::None> const & lhs, std::vector<byte_t> const & rhs) -> std::strong_ordering;

[[nodiscard]] constexpr auto operator==(BasicBytes<ByteNumbering::None> const & lhs, std::vector<byte_t> const & rhs) -> bool;

template <ByteNumbering ByteNumbering>
constexpr auto operator+(byte_t lhs, bytes_view<ByteNumbering> const & rhs) -> BasicBytes<ByteNumbering>;

template <ByteNumbering ByteNumbering>
constexpr void swap(BasicBytes<ByteNumbering> & lhs, BasicBytes<ByteNumbering> & rhs) noexcept;

constexpr auto erase(BasicBytes<ByteNumbering::None> & c, BasicBytes<ByteNumbering::None>::value_type value) -> BasicBytes<ByteNumbering::None>::size_type;

template <typename Predictor>
constexpr auto erase_if(BasicBytes<ByteNumbering::None> & c, Predictor predictor) -> BasicBytes<ByteNumbering::None>::size_type;

} // namespace abc

#endif // ABC_INCLUDE_ABC_BASIC_BYTES_DECL
