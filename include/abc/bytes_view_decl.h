// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_BYTES_VIEW_DECL
#define ABC_INCLUDE_ABC_BYTES_VIEW_DECL

#pragma once

#include "bytes_view_fwd_decl.h"

#include "byte.h"

#include <range/v3/range/concepts.hpp>

namespace abc {

template <byte_numbering ByteNumbering>
class bytes_view
{
public:
    static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

private:
    using container_type = std::basic_string_view<byte>;
    container_type view_{};

    template <byte_numbering>
    friend class bytes_view;

public:
    using value_type = typename container_type::value_type;
    using size_type = typename container_type::size_type;
    using difference_type = typename container_type::difference_type;
    using pointer = typename container_type::pointer;
    using const_pointer = typename container_type::const_pointer;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using iterator = typename container_type::const_iterator;
    using const_iterator = typename container_type::const_iterator;
    using reverse_iterator = typename container_type::const_reverse_iterator;
    using const_reverse_iterator = typename container_type::const_reverse_iterator;

private:
    constexpr bytes_view(std::basic_string_view<byte> view) noexcept;

    template <byte_numbering RhsByteNumbering>
        requires(RhsByteNumbering == ByteNumbering)
    constexpr bytes_view(bytes_view<RhsByteNumbering> rhs, byte_numbering_type<RhsByteNumbering>) noexcept;

    template <byte_numbering RhsByteNumbering>
        requires(ByteNumbering != RhsByteNumbering && (ByteNumbering == byte_numbering::none || RhsByteNumbering == byte_numbering::none))
    constexpr bytes_view(bytes_view<RhsByteNumbering> rhs, byte_numbering_type<RhsByteNumbering>) noexcept;

public:
    constexpr bytes_view() noexcept = default;
    constexpr bytes_view(bytes_view const &) noexcept = default;
    constexpr bytes_view(std::nullptr_t) = delete;

    constexpr auto
    operator=(bytes_view const &) noexcept -> bytes_view & = default;

//    friend constexpr auto
//    operator<=>(bytes_view const & lhs, bytes_view const & rhs) noexcept -> std::strong_ordering = default;

    friend constexpr auto
    operator==(bytes_view const & lhs, bytes_view const & rhs) noexcept -> bool = default;

    constexpr bytes_view(byte const * first, size_type count) noexcept;

    template <std::contiguous_iterator It, std::sized_sentinel_for<It> End>
        requires std::same_as<std::iter_value_t<It>, abc::byte> && (!std::convertible_to<End, size_type>)
    constexpr bytes_view(It first, End last) noexcept(noexcept(last - first)) : view_{first, last}
    {
    }

    template <typename Range, typename DRange = std::remove_cvref_t<Range>>
        requires(!std::is_same_v<DRange, bytes_view>) && std::ranges::contiguous_range<Range> && std::ranges::sized_range<Range> &&
                std::is_same_v<std::ranges::range_value_t<Range>, abc::byte> && (!std::is_convertible_v<Range, abc::byte const *>) &&
                (!requires(DRange & d) { d.operator ::abc::bytes_view<ByteNumbering>(); })
    constexpr explicit bytes_view(Range && r) noexcept(noexcept(std::ranges::size(r)) && noexcept(std::ranges::data(r))) : view_{std::forward<Range>(r)}
    {
    }

    constexpr explicit bytes_view(std::string_view sv) noexcept;

    template <byte_numbering RhsByteNumbering>
    constexpr static auto
    from(bytes_view<RhsByteNumbering> rhs) -> bytes_view;

    template <std::integral T>
        requires(ByteNumbering != byte_numbering::none)
    constexpr auto
    to() const noexcept -> T;

    [[nodiscard]] constexpr auto
    begin() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto
    cbegin() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto
    end() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto
    cend() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto
    rbegin() const noexcept -> const_reverse_iterator;

    [[nodiscard]] constexpr auto
    crbegin() const noexcept -> const_reverse_iterator;

    [[nodiscard]] constexpr auto
    rend() const noexcept -> const_reverse_iterator;

    [[nodiscard]] constexpr auto
    crend() const noexcept -> const_reverse_iterator;

    [[nodiscard]] constexpr auto
    front() const -> const_reference;

    [[nodiscard]] constexpr auto
    back() const -> const_reference;

    [[nodiscard]] constexpr auto
    operator[](size_type idx) const -> const_reference;

    [[nodiscard]] constexpr auto
    at(size_type pos) const -> const_reference;

    [[nodiscard]] constexpr auto
    data() const noexcept -> const_pointer;

    [[nodiscard]] constexpr auto
    size() const noexcept -> size_type;

    [[nodiscard]] constexpr auto
    empty() const noexcept -> bool;

    template <std::size_t Count>
    constexpr auto
    first() const -> bytes_view<ByteNumbering>;

    constexpr auto
    first(size_type count) const -> bytes_view<ByteNumbering>;

    template <std::size_t Count>
    constexpr auto
    last() const -> bytes_view<ByteNumbering>;

    constexpr auto
    last(size_type count) const -> bytes_view<ByteNumbering>;

    template <std::size_t Offset, std::size_t Count = npos>
    constexpr auto
    subview() const -> bytes_view<ByteNumbering>;

    constexpr auto
    subview(size_type offset, size_type count = npos) const -> bytes_view<ByteNumbering>;
};

using bytes_be_view_t = bytes_view<byte_numbering::msb0>;
using bytes_le_view_t = bytes_view<byte_numbering::lsb0>;
using bytes_view_t = bytes_view<byte_numbering::none>;

}

namespace ranges {

template <abc::byte_numbering ByteNumbering>
inline constexpr bool enable_borrowed_range<abc::bytes_view<ByteNumbering>> = true;

template <abc::byte_numbering ByteNumbering>
inline constexpr bool enable_view<abc::bytes_view<ByteNumbering>> = true;

}

#endif //ABC_INCLUDE_ABC_BYTES_VIEW_DECL
