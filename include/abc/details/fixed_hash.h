// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_FIXED_HASH)
#define ABC_DETAILS_FIXED_HASH

#pragma once

#include "abc/bigint.h"
#include "abc/bytes.h"
#include "abc/error.h"
#include "abc/hex_string.h"

#include <boost/functional/hash.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include <array>
#include <concepts>
#include <cstdint>
#include <random>
#include <span>
#include <type_traits>

namespace abc::details {

extern std::random_device fixed_hash_rd;

// store data in big-ending order
template <std::size_t Bytes>
class [[nodiscard]] xabc_fixed_hash {
public:
    using xarith_t = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<Bytes * 8, Bytes * 8, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;

private:
    using container_type = std::array<xbyte_t, Bytes>;
    container_type data_;

public:
    constexpr xabc_fixed_hash() {
        data_.fill(static_cast<xbyte_t>(0));
    }

    constexpr xabc_fixed_hash(xabc_fixed_hash const &) = default;
    constexpr xabc_fixed_hash(xabc_fixed_hash &&) = default;
    auto operator=(xabc_fixed_hash const &)->xabc_fixed_hash & = default;
    auto operator=(xabc_fixed_hash &&)->xabc_fixed_hash & = default;
    ~xabc_fixed_hash() = default;

    template <std::unsigned_integral T>
    constexpr explicit xabc_fixed_hash(T value) : xabc_fixed_hash{} {
        to_big_endian(value, data_);
    }

    explicit xabc_fixed_hash(xarith_t const & arith) : xabc_fixed_hash{  } {
        to_big_endian(arith, data_);
    }

    explicit xabc_fixed_hash(std::array<xbyte_t, Bytes> const & data) : data_{ data } {
    }

    explicit xabc_fixed_hash(std::span<xbyte_t, Bytes> binary) noexcept {
        std::memcpy(data_.data(), binary.data(), Bytes);
    }

    template <size_t BytesU>
    explicit xabc_fixed_hash(xabc_fixed_hash<BytesU> const & other) noexcept : xabc_fixed_hash{} {
        std::memcpy(data_.data(), other.data(), std::min(Bytes, BytesU));
    }

    explicit xabc_fixed_hash(std::span<xbyte_t const> const data) noexcept : xabc_fixed_hash{} {
        std::memcpy(data_.data(), data.data(), std::min(Bytes, data.size()));
    }

    //explicit xabc_fixed_hash(std::string const & string)
    //    : xabc_fixed_hash{ to_bytes(string) } {
    //}

    explicit xabc_fixed_hash(xhex_string_t const & hex_string)
        : xabc_fixed_hash{ to<xbytes_t>(hex_string) } {
    }

    auto swap(xabc_fixed_hash & other) noexcept -> void {
        data_.swap(other.data_);
    }

    auto operator==(xabc_fixed_hash const & other) const noexcept -> bool = default;
    auto operator<=>(xabc_fixed_hash const &) const noexcept -> std::strong_ordering = default;

    explicit operator bool() const {
        return std::any_of(std::begin(data_), std::end(data_), [](auto const & b) {
            return b != 0;
            });
    }

    auto operator^=(xabc_fixed_hash const & other) -> xabc_fixed_hash & {
        for (auto i = 0u; i < Bytes; ++i) {
            data_[i] ^= other.data_[i];
        }
        return *this;
    }

    [[nodiscard]] auto operator^(xabc_fixed_hash const & other) const -> xabc_fixed_hash {
        return xabc_fixed_hash{ *this } ^= other;
    }

    auto operator|=(xabc_fixed_hash const & other) -> xabc_fixed_hash & {
        for (auto i = 0u; i < Bytes; ++i) {
            data_[i] |= other.data_[i];
        }
        return *this;
    }

    [[nodiscard]] auto operator|(xabc_fixed_hash const & other) const -> xabc_fixed_hash {
        return xabc_fixed_hash{ *this } |= other;
    }

    auto operator&=(xabc_fixed_hash const & other) -> xabc_fixed_hash & {
        for (auto i = 0u; i < Bytes; ++i) {
            data_[i] &= other.data_[i];
        }
        return *this;
    }

    [[nodiscard]] auto operator&(xabc_fixed_hash const & other) const -> xabc_fixed_hash {
        return xabc_fixed_hash{ *this } &= other;
    }

    [[nodiscard]] auto operator~() const -> xabc_fixed_hash {
        xabc_fixed_hash ret;
        for (auto i = 0u; i < Bytes; ++i) {
            ret[i] = ~data_[i];
        }
        return ret;
    }

    [[nodiscard]] auto operator++() -> xabc_fixed_hash & {
        for (auto i = Bytes; i > 0 && !++data_[--i];) {}
        return *this;
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
        return !static_cast<bool>(*this);
    }

    [[nodiscard]] constexpr auto size() const noexcept -> size_t {
        return data_.size();
    }

    [[nodiscard]] auto hex_string() const -> std::string {
        return to_hex_string(std::begin(data_), std::end(data_));
    }

    template <typename EngineT>
    void randomize(EngineT & engine) {
        std::ranges::for_each(data_, [&engine](auto & byte) {
            byte = static_cast<xbyte_t>(std::uniform_int_distribution<uint16_t>{0, 255}(engine));
            });
    }

    [[nodiscard]] static auto random() -> xabc_fixed_hash {
        xabc_fixed_hash ret;
        ret.randomize(fixed_hash_rd);
        return ret;
    }

    auto clear() -> void {
        data_.fill(static_cast<xbyte_t>(0));
    }

    [[nodiscard]] constexpr auto data() noexcept -> std::span<xbyte_t, Bytes> {
        return std::span{ data_ };
    }

    [[nodiscard]] constexpr auto data() const noexcept -> std::span<xbyte_t const, Bytes> {
        return std::span{ data_ };
    }

    [[nodiscard]] auto to_big_number() const -> boost::multiprecision::number<boost::multiprecision::cpp_int_backend<Bytes * 8, Bytes * 8, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>> {
        xarith_t ret{ 0 };
        for (auto i : data_) {
            ret = xarith_t{ (ret << 8) | i };
        }
        return ret;
    }

    [[nodiscard]] constexpr auto begin() noexcept -> typename container_type::iterator {
        return std::begin(data_);
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> typename container_type::const_iterator {
        return std::begin(data_);
    }

    [[nodiscard]] constexpr auto end() noexcept -> typename container_type::iterator {
        return std::end(data_);
    }

    [[nodiscard]] constexpr auto end() const noexcept -> typename container_type::const_iterator {
        return std::end(data_);
    }

    [[nodiscard]] constexpr auto cbegin() const noexcept -> typename container_type::const_iterator {
        return std::cbegin(data_);
    }

    [[nodiscard]] constexpr auto cend() const noexcept -> typename container_type::const_iterator {
        return std::cend(data_);
    }

    [[nodiscard]] auto to_array() const noexcept -> std::array<xbyte_t, Bytes> const & {
        return data_;
    }
};

extern template class xabc_fixed_hash<1>;
extern template class xabc_fixed_hash<2>;
extern template class xabc_fixed_hash<4>;
extern template class xabc_fixed_hash<8>;
extern template class xabc_fixed_hash<16>;
extern template class xabc_fixed_hash<32>;
extern template class xabc_fixed_hash<64>;
extern template class xabc_fixed_hash<65>;

}
