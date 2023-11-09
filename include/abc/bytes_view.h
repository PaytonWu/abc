// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_BYTES_VIEW
#define ABC_INCLUDE_ABC_BYTES_VIEW

#pragma once

#include "byte.h"
#include "byte_bit_numbering.h"

#include <range/v3/range_concepts.hpp>
#include <range/v3/range_fwd.hpp>

#include <concepts>
#include <string_view>

namespace abc {

template <byte_numbering ByteNumbering>
class [[nodiscard]] bytes_view {
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
    constexpr bytes_view(container_type view) noexcept : view_{ view } {
    }

    template <byte_numbering RhsByteNumbering> requires (RhsByteNumbering == ByteNumbering)
    constexpr bytes_view(bytes_view<RhsByteNumbering> rhs, byte_numbering_type<RhsByteNumbering>) noexcept
        : view_{ rhs.view_ } {
    }

    template <byte_numbering RhsByteNumbering> requires (ByteNumbering != RhsByteNumbering && (ByteNumbering == byte_numbering::none || RhsByteNumbering == byte_numbering::none))
    constexpr bytes_view(bytes_view<RhsByteNumbering> rhs, byte_numbering_type<RhsByteNumbering>) noexcept
        : view_{ rhs.view_ } {
    }

public:

    constexpr bytes_view() noexcept = default;
    constexpr bytes_view(bytes_view const &) noexcept = default;
    constexpr auto operator=(bytes_view const &) noexcept -> bytes_view & = default;
    constexpr bytes_view(std::nullptr_t) = delete;

    constexpr bytes_view(byte const * first, size_type count) : view_{ first, count } {
    }

    template <std::contiguous_iterator It, std::sized_sentinel_for<It> End> requires std::same_as<std::iter_value_t<It>, abc::byte> && (!std::convertible_to<End, size_type>)
    constexpr bytes_view(It first, End last) noexcept(noexcept(last - first)) : view_{ first, last } {
    }

    template <typename Range, typename DRange = std::remove_cvref_t<Range>> requires (!std::is_same_v<DRange, bytes_view>) &&
                                                                                     std::ranges::contiguous_range<Range> &&
                                                                                     std::ranges::sized_range<Range> &&
                                                                                     std::is_same_v<std::ranges::range_value_t<Range>, abc::byte> &&
                                                                                     (!std::is_convertible_v<Range, abc::byte const *>) &&
                                                                                     (!requires (DRange & d) { d.operator ::abc::bytes_view<ByteNumbering>(); })
    constexpr explicit bytes_view(Range && r) noexcept(noexcept(std::ranges::size(r)) && noexcept(std::ranges::data(r)))
        : view_{ std::forward<Range>(r) } {
    }

    template <byte_numbering RhsByteNumbering>
    constexpr static auto from(bytes_view<RhsByteNumbering> rhs) -> bytes_view {
        return bytes_view{ rhs, byte_numbering_type<RhsByteNumbering>{} };
    }

    [[nodiscard]]
    constexpr auto
    begin() const noexcept -> const_iterator {
        return view_.begin();
    }

    [[nodiscard]]
    constexpr auto
    cbegin() const noexcept -> const_iterator {
        return view_.cbegin();
    }

    [[nodiscard]]
    constexpr auto
    end() const noexcept -> const_iterator {
        return view_.end();
    }

    [[nodiscard]]
    constexpr auto
    cend() const noexcept -> const_iterator {
        return view_.cend();
    }

    [[nodiscard]]
    constexpr auto
    rbegin() const noexcept -> const_reverse_iterator {
        return view_.rbegin();
    }

    [[nodiscard]]
    constexpr auto
    crbegin() const noexcept -> const_reverse_iterator {
        return view_.crbegin();
    }

    [[nodiscard]]
    constexpr auto
    rend() const noexcept -> const_reverse_iterator {
        return view_.rend();
    }

    [[nodiscard]]
    constexpr auto
    crend() const noexcept -> const_reverse_iterator {
        return view_.crend();
    }

    [[nodiscard]]
    constexpr auto
    front() const -> const_reference {
        return view_.front();
    }

    [[nodiscard]]
    constexpr auto
    back() const -> const_reference {
        return view_.back();
    }

    [[nodiscard]]
    constexpr auto
    operator[](size_type idx) const -> const_reference {
        return view_[idx];
    }

    [[nodiscard]]
    constexpr auto
    at(size_type pos) const -> const_reference {
        return view_.at(pos);
    }

    [[nodiscard]]
    constexpr auto
    data() const noexcept -> const_pointer {
        return view_.data();
    }

    [[nodiscard]]
    constexpr auto
    size() const noexcept -> size_type {
        return view_.size();
    }

    [[nodiscard]]
    constexpr auto
    empty() const noexcept -> bool {
        return view_.empty();
    }

    template <std::size_t Count>
    constexpr auto
    first() const -> bytes_view<ByteNumbering> {
        return bytes_view<ByteNumbering>{ view_.substr(0, Count) };
    }

    constexpr auto
    first(size_type count) const -> bytes_view<ByteNumbering> {
        return bytes_view<ByteNumbering>{ view_.substr(0, count) };
    }

    template <std::size_t Count>
    constexpr auto
    last() const -> bytes_view<ByteNumbering> {
        return bytes_view<ByteNumbering>{ view_.substr(view_.size() - Count) };
    }

    constexpr auto
    last(size_type count) const -> bytes_view<ByteNumbering> {
        return bytes_view<ByteNumbering>{ view_.substr(view_.size() - count) };
    }

    template <std::size_t Offset, std::size_t Count = std::dynamic_extent>
    constexpr auto
    subview() const -> bytes_view<ByteNumbering> {
        return { view_.substr(Offset, Count) };
    }

    constexpr auto
    subview(size_type offset, size_type count = std::dynamic_extent) const -> bytes_view<ByteNumbering> {
        return { view_.substr(offset, count) };
    }
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

#endif //ABC_INCLUDE_ABC_BYTES_VIEW
