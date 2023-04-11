// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/bytes.h"

#include "abc/hex_string.h"

#include <algorithm>
#include <iterator>
#include <ranges>

namespace abc::details {

auto xabc_bytes::operator+(xabc_bytes const & other) const -> xabc_bytes {
    internal_type data;
    data.reserve(size() + other.size());

    std::ranges::copy(*this, std::back_inserter(data));
    std::ranges::copy(other, std::back_inserter(data));

    return xabc_bytes{ std::move(data) };
}

auto xabc_bytes::operator+=(xabc_bytes const & other)->xabc_bytes & {
    if (data_.capacity() >= size() + other.size()) {
        std::ranges::copy(other, std::back_inserter(data_));
    }
    else {
        internal_type data;
        data.reserve(size() + other.size());

        std::ranges::copy(*this, std::back_inserter(data));
        std::ranges::copy(other, std::back_inserter(data));

        data_ = std::move(data);
    }

    return *this;
}

//auto xabc_bytes::operator+(xabc_bytes && other) const -> xabc_bytes {
//    internal_type data;
//    data.reserve(size() + other.size());
//
//    std::ranges::copy(*this, std::back_inserter(data));
//    std::ranges::move(other, std::back_inserter(data));
//
//    return xabc_bytes{ std::move(data) };
//}
//
//auto xabc_bytes::operator+=(xabc_bytes && other)->xabc_bytes & {
//    if (data_.capacity() >= size() + other.size()) {
//        std::ranges::copy(other, std::back_inserter(data_));
//    } else {
//        internal_type data;
//        data.reserve(size() + other.size());
//
//        std::ranges::copy(*this, std::back_inserter(data));
//        std::ranges::copy(other, std::back_inserter(data));
//
//        data_ = std::move(data);
//    }
//
//    return *this;
//}

auto xabc_bytes::operator+(std::span<xbyte_t const> const other) const -> xabc_bytes {
    internal_type data;
    data.reserve(size() + other.size());

    std::ranges::copy(*this, std::back_inserter(data));
    std::ranges::copy(other, std::back_inserter(data));

    return xabc_bytes{ std::move(data) };
}

auto xabc_bytes::operator+=(std::span<xbyte_t const> const other) -> xabc_bytes & {
    if (data_.capacity() >= size() + other.size()) {
        std::ranges::copy(other, std::back_inserter(data_));
    }
    else {
        internal_type data;
        data.reserve(size() + other.size());

        std::ranges::copy(*this, std::back_inserter(data));
        std::ranges::copy(other, std::back_inserter(data));

        data_ = std::move(data);
    }

    return *this;
}

auto xabc_bytes::operator+(xbyte_t const byte) const -> xabc_bytes {
    internal_type new_data;
    new_data.reserve(size() + 1);
    std::ranges::copy(*this, std::back_inserter(new_data));
    new_data.insert(std::end(new_data), byte);
    return xabc_bytes{ std::move(new_data) };
}

auto xabc_bytes::operator+=(xbyte_t const byte) -> xabc_bytes & {
    data_.push_back(byte);

    return *this;
}

auto xabc_bytes::sub_bytes(size_t const offset, size_t const count) const -> xbytes_t {
    if (offset >= data_.size()) {
        return {};
    }

    if (count >= data_.size() || offset + count >= data_.size()) {
        return { std::next(std::begin(data_), static_cast<ptrdiff_t>(offset)), std::end(data_) };
    }

    return { std::next(std::begin(data_), static_cast<ptrdiff_t>(offset)), std::next(std::begin(data_), static_cast<ptrdiff_t>(offset + count)) };
}

}

namespace abc {

auto operator+(std::span<xbyte_t const> const lhs, xbytes_t const & rhs) -> xbytes_t {
    return xbytes_t{ lhs } + rhs;
}

auto operator+(xbyte_t const lhs, xbytes_t const & rhs) -> xbytes_t {
    return xbytes_t{ static_cast<size_t>(1), lhs } + rhs;
}

auto operator+(xbyte_t const lhs, std::span<xbyte_t const> const rhs) -> xbytes_t {
    return xbytes_t{ static_cast<size_t>(1), lhs } + rhs;
}

template <>
auto to<xhex_string_t, xbytes_t>(xbytes_t const & from) -> xhex_string_t {
    return xhex_string_t::from(from);
}

}
