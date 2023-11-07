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
#include <span>

namespace abc {

template <byte_numbering ByteNumbering, std::size_t Extent = std::dynamic_extent>
class [[nodiscard]] bytes_view {
private:
    using container_type = std::span<byte const, Extent>;
    container_type view_{};

    template <byte_numbering, std::size_t>
    friend class bytes_view;

public:
    using element_type = typename container_type::element_type;
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

    static constexpr std::size_t extent = container_type::extent;

    constexpr bytes_view() noexcept = default;

    template <typename It>
    explicit(extent != std::dynamic_extent)
    constexpr bytes_view(It first, size_type count) : view_{ first, count } {
    }

    constexpr bytes_view(bytes_view const &) noexcept = default;
    constexpr auto operator=(bytes_view const &) noexcept -> bytes_view & = default;

    template <typename Range, typename DRange = std::remove_cvref_t<Range>> requires (!std::is_same_v<DRange, bytes_view>) &&
                                                                                     std::ranges::contiguous_range<Range> &&
                                                                                     std::ranges::sized_range<Range> &&
                                                                                     std::is_same_v<std::ranges::range_value_t<Range>, abc::byte> &&
                                                                                     (!std::is_convertible_v<Range, abc::byte const *>) &&
                                                                                     (!requires (DRange & d) { d.operator ::abc::bytes_view<ByteNumbering, Extent>(); })
    constexpr explicit bytes_view(Range && r) noexcept(noexcept(std::ranges::size(r)) && noexcept(std::ranges::data(r)))
        : view_{ std::ranges::data(r), std::ranges::size(r) } {
    }

    constexpr auto
    begin() const noexcept -> const_iterator {
        return view_.begin();
    }

    constexpr auto
    cbegin() const noexcept -> const_iterator {
        return view_.cbegin();
    }

    constexpr auto
    end() const noexcept -> const_iterator {
        return view_.end();
    }

    constexpr auto
    cend() const noexcept -> const_iterator {
        return view_.cend();
    }

    constexpr auto
    rbegin() const noexcept -> const_reverse_iterator {
        return view_.rbegin();
    }

    constexpr auto
    crbegin() const noexcept -> const_reverse_iterator {
        return view_.crbegin();
    }

    constexpr auto
    rend() const noexcept -> const_reverse_iterator {
        return view_.rend();
    }

    constexpr auto
    crend() const noexcept -> const_reverse_iterator {
        return view_.crend();
    }

    constexpr auto
    front() const -> const_reference {
        return view_.front();
    }

    constexpr auto
    back() const -> const_reference {
        return view_.back();
    }

    constexpr auto
    operator[](size_type idx) const -> const_reference {
        return view_[idx];
    }

    constexpr auto
    data() const noexcept -> const_pointer {
        return view_.data();
    }

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
    first() const -> bytes_view<ByteNumbering, Count> {
        return bytes_view<ByteNumbering, Count>{ view_.template first<Count>() };
    }

    constexpr auto
    first(size_type count) const -> bytes_view<ByteNumbering> {
        return bytes_view<ByteNumbering>{ view_.first(count) };
    }

    template <std::size_t Count>
    constexpr auto
    last() const -> bytes_view<ByteNumbering, Count> {
        return bytes_view<ByteNumbering, Count>{ view_.template last<Count>() };
    }

    constexpr auto
    last(size_type count) const -> bytes_view<ByteNumbering> {
        return bytes_view<ByteNumbering>{ view_.last(count) };
    }

    template <std::size_t Offset, std::size_t Count = std::dynamic_extent>
    constexpr auto
    subview() const -> bytes_view<ByteNumbering, Count != std::dynamic_extent ? Count : (Extent != std::dynamic_extent ? Extent - Offset : std::dynamic_extent)> {
        return { view_.template subspan<Offset, Count>() };
    }

    constexpr auto
    subview(size_type offset, size_type count =std::dynamic_extent) const -> bytes_view<ByteNumbering> {
        return { view_.subspan(offset, count) };
    }
};

template <std::size_t Extent = std::dynamic_extent>
using bytes_be_view_t = bytes_view<byte_numbering::msb0, Extent>;

using bytes4_be_view_t = bytes_be_view_t<4>;
using bytes8_be_view_t = bytes_be_view_t<8>;
using bytes16_be_view_t = bytes_be_view_t<16>;
using bytes32_be_view_t = bytes_be_view_t<32>;
using bytes48_be_view_t = bytes_be_view_t<48>;
using bytes64_be_view_t = bytes_be_view_t<64>;
using bytes96_be_view_t = bytes_be_view_t<96>;
using bytes128_be_view_t = bytes_be_view_t<128>;

template <std::size_t Extent = std::dynamic_extent>
using bytes_le_view_t = bytes_view<byte_numbering::lsb0, Extent>;

using bytes4_le_view_t = bytes_le_view_t<4>;
using bytes8_le_view_t = bytes_le_view_t<8>;
using bytes16_le_view_t = bytes_le_view_t<16>;
using bytes32_le_view_t = bytes_le_view_t<32>;
using bytes48_le_view_t = bytes_le_view_t<48>;
using bytes64_le_view_t = bytes_le_view_t<64>;
using bytes96_le_view_t = bytes_le_view_t<96>;
using bytes128_le_view_t = bytes_le_view_t<128>;

template <std::size_t Extent = std::dynamic_extent>
using bytes_view_t = bytes_view<byte_numbering::none, Extent>;

using bytes4_view_t = bytes_view_t<4>;
using bytes8_view_t = bytes_view_t<8>;
using bytes16_view_t = bytes_view_t<16>;
using bytes32_view_t = bytes_view_t<32>;
using bytes48_view_t = bytes_view_t<48>;
using bytes64_view_t = bytes_view_t<64>;
using bytes96_view_t = bytes_view_t<96>;
using bytes128_view_t = bytes_view_t<128>;

}

namespace ranges {

template <abc::byte_numbering ByteNumbering, std::size_t Extent>
inline constexpr bool enable_borrowed_range<abc::bytes_view<ByteNumbering, Extent>> = true;

template <abc::byte_numbering ByteNumbering, std::size_t Extent>
inline constexpr bool enable_view<abc::bytes_view<ByteNumbering, Extent>> = true;

}

#endif //ABC_INCLUDE_ABC_BYTES_VIEW
