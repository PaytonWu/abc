// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_FIXED_BYTES_DECL
#define ABC_INCLUDE_ABC_FIXED_BYTES_DECL

#pragma once

#include "fixed_bytes_fwd_decl.h"

namespace abc
{

template <std::size_t N, ByteNumbering ByteNumberingV>
class fixed_bytes
{
    static_assert(N > 0);

private:
    using internal_type = std::array<byte, N>;
    internal_type data_;

public:
    using value_type = typename internal_type::value_type;
    using size_type = typename internal_type::size_type;
    using difference_type = typename internal_type::difference_type;
    using reference = typename internal_type::reference;
    using const_reference = typename internal_type::const_reference;
    using pointer = typename internal_type::pointer;
    using const_pointer = typename internal_type::const_pointer;
    using iterator = typename internal_type::iterator;
    using const_iterator = typename internal_type::const_iterator;
    using reverse_iterator = typename internal_type::reverse_iterator;
    using const_reverse_iterator = typename internal_type::const_reverse_iterator;

private:
    template <ByteNumbering SrcByteNumberingV>
        requires(SrcByteNumberingV == ByteNumberingV)
    constexpr explicit fixed_bytes(std::array<byte, N> const & src, ByteNumberingType<SrcByteNumberingV>);

    template <ByteNumbering SrcByteNumberingV>
        requires(SrcByteNumberingV != ByteNumberingV && SrcByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
    constexpr explicit fixed_bytes(std::array<byte, N> const & src, ByteNumberingType<SrcByteNumberingV>);

    template <ByteNumbering SrcByteNumberingV>
        requires(SrcByteNumberingV == ByteNumberingV)
    constexpr explicit fixed_bytes(std::array<std::byte, N> const & src, ByteNumberingType<SrcByteNumberingV>);

    template <ByteNumbering SrcByteNumberingV>
        requires(SrcByteNumberingV != ByteNumberingV && SrcByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
    constexpr explicit fixed_bytes(std::array<std::byte, N> const & src, ByteNumberingType<SrcByteNumberingV>);

    template <ByteNumbering SrcByteNumberingV>
        requires(SrcByteNumberingV == ByteNumberingV)
    constexpr explicit fixed_bytes(bytes_view<SrcByteNumberingV> src);

    template <ByteNumbering SrcByteNumberingV>
        requires(SrcByteNumberingV != ByteNumberingV && SrcByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
    constexpr explicit fixed_bytes(bytes_view<SrcByteNumberingV> src);

    template <ByteNumbering SrcByteNumberingV>
        requires(SrcByteNumberingV != ByteNumberingV && SrcByteNumberingV == ByteNumbering::None)
    constexpr explicit fixed_bytes(bytes_view<SrcByteNumberingV> src);

public:
    constexpr fixed_bytes();

    constexpr fixed_bytes(std::unsigned_integral auto value)
        requires((ByteNumberingV == ByteNumbering::Lsb0) || (ByteNumberingV == ByteNumbering::Msb0));

    template <ByteNumbering ByteNumberingRhsV>
        requires(ByteNumberingRhsV != ByteNumbering::None && ByteNumberingRhsV != ByteNumberingV)
    constexpr explicit fixed_bytes(fixed_bytes<N, ByteNumberingRhsV> const & rhs);

    template <ByteNumbering DataByteNumberingV>
    static auto from(std::array<byte, N> const & data) -> expected<fixed_bytes, std::error_code>;

    template <ByteNumbering DataByteNumberingV>
        requires(DataByteNumberingV == ByteNumberingV) || (DataByteNumberingV != ByteNumberingV && DataByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
    static auto from(bytes_view<DataByteNumberingV> data) -> expected<fixed_bytes, std::error_code>;

    template <ByteNumbering DataByteNumberingV>
        requires(DataByteNumberingV != ByteNumberingV && DataByteNumberingV == ByteNumbering::None)
    static auto from(bytes_view<DataByteNumberingV> data) -> expected<fixed_bytes, std::error_code>;

    friend auto operator==(fixed_bytes const &, fixed_bytes const &) noexcept -> bool = default;

    friend auto operator<=>(fixed_bytes const &, fixed_bytes const &) noexcept -> std::strong_ordering = default;

    constexpr auto operator[](size_t index) const noexcept -> byte;

    constexpr auto operator[](size_t index) noexcept -> byte &;

    constexpr operator bytes_view<ByteNumberingV>() const noexcept;

    [[nodiscard]] constexpr auto front() const noexcept -> const_reference;

    [[nodiscard]] constexpr auto front() noexcept -> reference;

    [[nodiscard]] constexpr auto back() const noexcept -> const_reference;

    [[nodiscard]] constexpr auto back() noexcept -> reference;

    [[nodiscard]] constexpr auto begin() noexcept -> iterator;

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto end() noexcept -> iterator;

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator;

    [[nodiscard]] constexpr auto rbegin() noexcept -> reverse_iterator;

    [[nodiscard]] constexpr auto rbegin() const noexcept -> const_reverse_iterator;

    [[nodiscard]] constexpr auto crbegin() const noexcept -> const_reverse_iterator;

    [[nodiscard]] constexpr auto rend() noexcept -> reverse_iterator;

    [[nodiscard]] constexpr auto rend() const noexcept -> const_reverse_iterator;

    [[nodiscard]] constexpr auto crend() const noexcept -> const_reverse_iterator;

    constexpr auto data() noexcept -> pointer;

    [[nodiscard]] constexpr auto data() const noexcept -> const_pointer;

    [[nodiscard]] constexpr auto empty() const noexcept -> bool;

    [[nodiscard]] constexpr auto size() const noexcept -> size_type;

    constexpr auto fill(byte value) noexcept -> void;

    [[nodiscard]] constexpr static auto byte_numbering() noexcept -> abc::ByteNumbering;

    template <std::unsigned_integral T>
    [[nodiscard]] constexpr auto to() const noexcept -> T;

    [[nodiscard]] constexpr auto zero() const noexcept -> bool;

    [[nodiscard]] constexpr auto operator^=(fixed_bytes const & other) -> fixed_bytes &;

    constexpr auto operator^(fixed_bytes const & other) const -> fixed_bytes;

    [[nodiscard]] constexpr auto operator|=(fixed_bytes const & other) -> fixed_bytes &;

    constexpr auto operator|(fixed_bytes const & other) const -> fixed_bytes;

    [[nodiscard]] constexpr auto operator&=(fixed_bytes const & other) -> fixed_bytes &;

    constexpr auto operator&(fixed_bytes const & other) const -> fixed_bytes;

    constexpr auto operator~() const -> fixed_bytes;

    constexpr auto operator++() -> fixed_bytes &;

    constexpr auto clear() -> void;

    auto subbytes(size_type pos, size_type n = static_cast<size_t>(-1)) const -> expected<Bytes<ByteNumberingV>, std::error_code>;

    auto subview(size_type pos, size_type n = static_cast<size_type>(-1)) const & -> expected<bytes_view<ByteNumberingV>, std::error_code>;

    auto subview(size_type, size_type = static_cast<size_type>(-1)) const && -> expected<bytes_view<ByteNumberingV>, std::error_code>;

    inline auto subspan(size_type pos, size_type n = static_cast<size_t>(-1)) & -> expected<std::span<byte>, std::error_code>;

    /// @brief make span from rvalue fixed_bytes. not allowed.
    /// @return an std::error_code with errc::span_built_from_rvalue.
    auto subspan(size_type, size_type = static_cast<size_t>(-1)) && -> expected<std::span<byte>, std::error_code>;

    inline auto subspan(size_type pos, size_type n = static_cast<size_t>(-1)) const & -> expected<std::span<byte const>, std::error_code>;

    /// @brief make span from rvalue fixed_bytes. not allowed.
    /// @return an std::error_code with errc::span_built_from_rvalue.
    inline auto subspan(size_type, size_type = static_cast<size_t>(-1)) const && -> expected<std::span<byte const>, std::error_code>;

private:
    template <std::unsigned_integral T>
    constexpr static void to_little_endian(T value, internal_type & data)
        requires((ByteNumberingV == ByteNumbering::Lsb0) || (ByteNumberingV == ByteNumbering::Msb0));

    template <std::unsigned_integral T>
    constexpr static void to_big_endian(T value, internal_type & data)
        requires((ByteNumberingV == ByteNumbering::Lsb0) || (ByteNumberingV == ByteNumbering::Msb0));
};

using bytes4_msb0_t = fixed_bytes<4, ByteNumbering::Msb0>;
using bytes8_msb0_t = fixed_bytes<8, ByteNumbering::Msb0>;
using bytes16_msb0_t = fixed_bytes<16, ByteNumbering::Msb0>;
using bytes32_msb0_t = fixed_bytes<32, ByteNumbering::Msb0>;
using bytes48_msb0_t = fixed_bytes<48, ByteNumbering::Msb0>;
using bytes64_msb0_t = fixed_bytes<64, ByteNumbering::Msb0>;
using bytes96_msb0_t = fixed_bytes<96, ByteNumbering::Msb0>;
using bytes128_msb0_t = fixed_bytes<128, ByteNumbering::Msb0>;
using bytes256_msb0_t = fixed_bytes<256, ByteNumbering::Msb0>;

using bytes4_be_t = bytes4_msb0_t;
using bytes8_be_t = bytes8_msb0_t;
using bytes16_be_t = bytes16_msb0_t;
using bytes32_be_t = bytes32_msb0_t;
using bytes48_be_t = bytes48_msb0_t;
using bytes64_be_t = bytes64_msb0_t;
using bytes96_be_t = bytes96_msb0_t;
using bytes128_be_t = bytes128_msb0_t;
using bytes256_be_t = bytes256_msb0_t;

using bytes4_lsb0_t = fixed_bytes<4, ByteNumbering::Lsb0>;
using bytes8_lsb0_t = fixed_bytes<8, ByteNumbering::Lsb0>;
using bytes16_lsb0_t = fixed_bytes<16, ByteNumbering::Lsb0>;
using bytes32_lsb0_t = fixed_bytes<32, ByteNumbering::Lsb0>;
using bytes48_lsb0_t = fixed_bytes<48, ByteNumbering::Lsb0>;
using bytes64_lsb0_t = fixed_bytes<64, ByteNumbering::Lsb0>;
using bytes96_lsb0_t = fixed_bytes<96, ByteNumbering::Lsb0>;
using bytes128_lsb0_t = fixed_bytes<128, ByteNumbering::Lsb0>;
using bytes256_lsb0_t = fixed_bytes<256, ByteNumbering::Lsb0>;

using bytes4_le_t = bytes4_lsb0_t;
using bytes8_le_t = bytes8_lsb0_t;
using bytes16_le_t = bytes16_lsb0_t;
using bytes32_le_t = bytes32_lsb0_t;
using bytes48_le_t = bytes48_lsb0_t;
using bytes64_le_t = bytes64_lsb0_t;
using bytes96_le_t = bytes96_lsb0_t;
using bytes128_le_t = bytes128_lsb0_t;
using bytes256_le_t = bytes256_lsb0_t;

using bytes4_t = fixed_bytes<4, ByteNumbering::None>;
using bytes8_t = fixed_bytes<8, ByteNumbering::None>;
using bytes16_t = fixed_bytes<16, ByteNumbering::None>;
using bytes32_t = fixed_bytes<32, ByteNumbering::None>;
using bytes48_t = fixed_bytes<48, ByteNumbering::None>;
using bytes64_t = fixed_bytes<64, ByteNumbering::None>;
using bytes96_t = fixed_bytes<96, ByteNumbering::None>;
using bytes128_t = fixed_bytes<128, ByteNumbering::None>;
using bytes256_t = fixed_bytes<256, ByteNumbering::None>;

} // namespace abc

#endif // ABC_INCLUDE_ABC_FIXED_BYTES_DECL
