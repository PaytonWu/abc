// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_BYTES_DECL
#define ABC_INCLUDE_ABC_BYTES_DECL

#pragma once

#include "bytes_fwd_decl.h"

#include "byte.h"
#include "bytes_view.h"
#include "expected.h"

namespace abc
{

template <byte_numbering ByteNumbering>
class bytes
{
private:
    template <byte_numbering RhsByteNumbering>
    friend class bytes;

    using internal_type = std::vector<byte>;

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

    // public constructors

    bytes() = default;

    bytes(bytes const &) = default;

    auto
    operator=(bytes const &) -> bytes & = default;

    bytes(bytes &&) = default;

    auto
    operator=(bytes &&) -> bytes & = default;

    ~bytes() = default;

    template <byte_numbering RhsByteNumbering>
        requires(RhsByteNumbering != ByteNumbering && RhsByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
    constexpr explicit bytes(bytes<RhsByteNumbering> && rhs);

    template <byte_numbering RhsByteNumber>
        requires(RhsByteNumber != ByteNumbering && (RhsByteNumber == byte_numbering::none || ByteNumbering == byte_numbering::none))
    constexpr explicit bytes(bytes<RhsByteNumber> && rhs) noexcept;

    constexpr explicit
    bytes(bytes_view<ByteNumbering> view);

    template <byte_numbering ViewByteNumbering>
        requires(ViewByteNumbering != ByteNumbering && ViewByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
    constexpr explicit bytes(bytes_view<ViewByteNumbering> view);

    template <byte_numbering ViewByteNumbering>
        requires(ViewByteNumbering != ByteNumbering && (ViewByteNumbering == byte_numbering::none || ByteNumbering == byte_numbering::none))
    constexpr explicit bytes(bytes_view<ViewByteNumbering> view);

    constexpr
    bytes(std::initializer_list<value_type> il);

    // constexpr auto
    // operator=(std::vector<byte> raw) noexcept -> bytes & requires(ByteNumbering == byte_numbering::none);

    template <byte_numbering RhsByteNumbering>
        requires(RhsByteNumbering != ByteNumbering && RhsByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
    constexpr auto
    operator=(bytes<RhsByteNumbering> && rhs) -> bytes &;

    template <byte_numbering RhsByteNumber>
        requires(RhsByteNumber != ByteNumbering && (RhsByteNumber == byte_numbering::none || ByteNumbering == byte_numbering::none))
    constexpr auto
    operator=(bytes<RhsByteNumber> && rhs) noexcept -> bytes &;

    constexpr auto
    operator=(bytes_view<ByteNumbering> view) -> bytes &;

    template <byte_numbering ViewByteNumbering>
        requires(ViewByteNumbering != ByteNumbering && ViewByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
    constexpr auto
    operator=(bytes_view<ViewByteNumbering> view) -> bytes &;

    template <byte_numbering ViewByteNumbering>
        requires(ViewByteNumbering != ByteNumbering && (ViewByteNumbering == byte_numbering::none || ByteNumbering == byte_numbering::none))
    constexpr auto
    operator=(bytes_view<ViewByteNumbering> view) -> bytes &;

    constexpr auto
    operator=(std::initializer_list<value_type> il) -> bytes &;

private:
    template <std::input_iterator Iterator, byte_numbering SrcByteNumbering>
        requires(SrcByteNumbering == ByteNumbering || SrcByteNumbering == byte_numbering::none || ByteNumbering == byte_numbering::none)
    constexpr bytes(Iterator begin, Iterator end, byte_numbering_type<SrcByteNumbering>);

    template <std::input_iterator Iterator, byte_numbering SrcByteNumbering>
        requires(SrcByteNumbering != ByteNumbering && SrcByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
    constexpr bytes(Iterator begin, Iterator end, byte_numbering_type<SrcByteNumbering>);

    template <byte_numbering SrcByteNumbering>
        requires(SrcByteNumbering == ByteNumbering || SrcByteNumbering == byte_numbering::none || ByteNumbering == byte_numbering::none)
    constexpr bytes(std::initializer_list<value_type> il, byte_numbering_type<SrcByteNumbering>);

    template <byte_numbering SrcByteNumbering>
        requires(SrcByteNumbering != ByteNumbering && SrcByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
    constexpr bytes(std::initializer_list<value_type> il, byte_numbering_type<SrcByteNumbering>);

public:
    template <std::integral T>
    constexpr static auto
    from(T i) -> bytes;

    template <byte_numbering DataByteNumbering, std::input_iterator InputIterator>
    constexpr static auto
    from(InputIterator begin, InputIterator end) -> bytes;

    template <byte_numbering ViewByteNumbering>
    constexpr static auto
    from(bytes_view<ViewByteNumbering> view) -> bytes;

    template <byte_numbering DataByteNumbering>
    constexpr static auto
    from(std::initializer_list<byte> il) -> bytes;

    template <byte_numbering RhsByteNumbering>
    constexpr static auto
    from(bytes<RhsByteNumbering> && rhs) -> bytes;

    template <std::integral T>
        requires(ByteNumbering != byte_numbering::none)
    constexpr auto
    to() const noexcept -> T;

    template <byte_numbering ToByteNumbering>
        requires(ByteNumbering != ToByteNumbering)
    constexpr auto
    to() const & -> bytes<ToByteNumbering>;

    template <byte_numbering ToByteNumbering>
        requires(ByteNumbering != ToByteNumbering)
    constexpr auto
    to() && -> bytes<ToByteNumbering>;

    constexpr auto
    assign(size_type count, byte const & value) -> void;

    [[nodiscard]] constexpr auto
    at(size_type pos) -> reference;

    [[nodiscard]] constexpr auto
    at(size_type pos) const -> const_reference;

    [[nodiscard]] constexpr auto
    operator[](size_type pos) -> reference;

    [[nodiscard]] constexpr auto
    operator[](size_type pos) const -> const_reference;

    [[nodiscard]] constexpr auto
    front() -> reference;

    [[nodiscard]] constexpr auto
    front() const -> const_reference;

    [[nodiscard]] constexpr auto
    back() -> reference;

    [[nodiscard]] constexpr auto
    back() const -> const_reference;

    [[nodiscard]] constexpr auto
    data() noexcept -> value_type *;

    [[nodiscard]] constexpr auto
    data() const noexcept -> value_type const *;

    [[nodiscard]] constexpr auto
    begin() noexcept -> iterator;

    [[nodiscard]] constexpr auto
    begin() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto
    cbegin() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto
    end() noexcept -> iterator;

    [[nodiscard]] constexpr auto
    end() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto
    cend() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto
    empty() const noexcept -> bool;

    [[nodiscard]] constexpr auto
    size() const noexcept -> size_type;

    constexpr auto
    resize(size_type new_size) -> void;

    constexpr void
    reserve(size_type new_cap);

    constexpr void
    clear() noexcept;

    constexpr void
    push_back(value_type value);

    constexpr void
    pop_back();

    constexpr void
    swap(bytes & other) noexcept;

    constexpr explicit
    operator std::vector<byte> const &() const noexcept
        requires(ByteNumbering == byte_numbering::none);

    constexpr explicit
    operator std::vector<byte> &() noexcept
        requires(ByteNumbering == byte_numbering::none);

    [[nodiscard]] constexpr auto
    least_significant_byte() const -> const_reference
        requires(ByteNumbering != byte_numbering::none);

    [[nodiscard]] constexpr auto
    least_significant_byte() -> reference
        requires(ByteNumbering != byte_numbering::none);

    [[nodiscard]] constexpr auto
    most_significant_byte() const -> const_reference
        requires(ByteNumbering != byte_numbering::none);

    [[nodiscard]] constexpr auto
    most_significant_byte() -> reference
        requires(ByteNumbering != byte_numbering::none);

    [[nodiscard]] constexpr auto
    first(size_t count) const noexcept -> bytes_view<ByteNumbering>;

    [[nodiscard]] constexpr auto
    last(size_t count) const noexcept -> bytes_view<ByteNumbering>;

    auto
    subview(size_type pos, size_type n = static_cast<size_type>(-1)) const -> expected<bytes_view<ByteNumbering>, std::error_code>;

    constexpr auto
    view() const noexcept -> bytes_view<ByteNumbering>;

    constexpr auto
    operator+(byte other) const -> bytes;

    constexpr auto
    operator+=(byte other) -> bytes &;

    constexpr auto
    operator+(bytes_view<ByteNumbering> other) const -> bytes;

    constexpr auto
    operator+=(bytes_view<ByteNumbering> other) -> bytes &;

    constexpr
    operator bytes_view<ByteNumbering>() const noexcept;

private:
    friend constexpr auto
    operator==(bytes const & lhs, bytes const & rhs) noexcept -> bool = default;

    friend constexpr auto
    operator<=>(bytes const & lhs, bytes const & rhs) noexcept -> std::strong_ordering = default;
};

[[nodiscard]] constexpr auto
operator<=>(bytes<byte_numbering::none> const & lhs, std::vector<byte> const & rhs) -> std::strong_ordering;

[[nodiscard]] constexpr auto
operator==(bytes<byte_numbering::none> const & lhs, std::vector<byte> const & rhs) -> bool;

template <byte_numbering ByteNumbering>
constexpr auto
operator+(byte lhs, bytes_view<ByteNumbering> const & rhs) -> bytes<ByteNumbering>;

template <byte_numbering ByteNumbering>
constexpr void
swap(bytes<ByteNumbering> & lhs, bytes<ByteNumbering> & rhs) noexcept;

constexpr auto
erase(bytes<byte_numbering::none> & c, bytes<byte_numbering::none>::value_type value) -> bytes<byte_numbering::none>::size_type;

template <typename Predictor>
constexpr auto
erase_if(bytes<byte_numbering::none> & c, Predictor predictor) -> bytes<byte_numbering::none>::size_type;

using bytes_t = bytes<byte_numbering::none>;
using bytes_msb0_t = bytes<byte_numbering::msb0>;
using bytes_lsb0_t = bytes<byte_numbering::lsb0>;
using bytes_be_t = bytes_msb0_t;
using bytes_le_t = bytes_lsb0_t;

} // namespace abc

#endif // ABC_INCLUDE_ABC_BYTES_DECL
