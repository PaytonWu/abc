// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_CONVERTER)
#define ABC_CONVERTER

#pragma once

#include "expected.h"
#include "fixed_bytes.h"
#include "hex_string.h"
#include "uint128.h"

#include <range/v3/algorithm/copy.hpp>

#include <concepts>
#include <system_error>

namespace abc
{

template <typename OutputT, typename InputT>
struct converter
{
    inline static auto
    from(InputT const & input) -> abc::expected<OutputT, std::error_code>
    {
        return static_cast<OutputT>(input);
    }
};

template <typename OutputT>
struct convert_to
{
    template <typename InputT>
    inline static auto
    from(InputT const & input) -> abc::expected<OutputT, std::error_code>
    {
        return converter<OutputT, InputT>::from(input);
    }
};

template <size_t N, ByteNumbering ByteNumberingV>
struct converter<hex_string, fixed_bytes<N, ByteNumberingV>>
{
    inline static auto
    from(fixed_bytes<N, ByteNumberingV> const & fixed_bytes) -> abc::expected<hex_string, std::error_code>
    {
        return hex_string::from<ByteNumberingV>(fixed_bytes);
    }
};

template <size_t N, ByteNumbering ByteNumberingV>
struct converter<fixed_bytes<N, ByteNumberingV>, hex_string>
{
    inline static auto
    from(hex_string const & hex_str) -> abc::expected<fixed_bytes<N, ByteNumberingV>, std::error_code>
    {
        if (hex_str.size() != N * 2)
        {
            return abc::make_unexpected(make_error_code(std::errc::invalid_argument));
        }

        std::array<byte, N> bytes{};
        ranges::copy(hex_str.bytes<ByteNumberingV>(), std::begin(bytes));
        return fixed_bytes<N, ByteNumberingV>::template from<ByteNumberingV>(bytes);
    }
};

template <ByteNumbering ByteNumberingV, std::integral T>
struct converter<Bytes<ByteNumberingV>, T>
{
    inline static auto
    from(T const & num) -> abc::expected<Bytes<ByteNumberingV>, std::error_code>
    {
        return Bytes<ByteNumberingV>::from(num);
    }
};

template <std::integral T, ByteNumbering ByteNumberingV>
struct converter<T, Bytes<ByteNumberingV>>
{
    inline static auto
    from(Bytes<ByteNumberingV> const & number_bytes) -> abc::expected<T, std::error_code>
    {
        return number_bytes.template to<T>();
    }
};

template <std::integral T, ByteNumbering ByteNumberingV>
struct converter<T, bytes_view<ByteNumberingV>>
{
    inline static auto
    from(bytes_view<ByteNumberingV> const & number_bytes) -> abc::expected<T, std::error_code>
    {
        T r{};
        if constexpr (ByteNumberingV == ByteNumbering::Lsb0 || (ByteNumberingV == ByteNumbering::None && std::endian::native == std::endian::little))
        {
            for (auto i = 0zu; i < number_bytes.size(); ++i)
            {
                r += (static_cast<T>(number_bytes[i]) << (i * 8));
            }
        }
        else if constexpr (ByteNumberingV == ByteNumbering::Msb0 || (ByteNumberingV == ByteNumbering::None && std::endian::native == std::endian::big))
        {
            for (auto i = 0zu; i < number_bytes.size(); ++i)
            {
                r = r << 8 | number_bytes[i];
            }
        }

        return r;
    }
};

template <ByteNumbering ByteNumberingV>
struct converter<uint128_t, bytes_view<ByteNumberingV>>
{
    inline static auto
    from(bytes_view<ByteNumberingV> const & number_bytes) -> abc::expected<uint128_t, std::error_code>
    {
        uint128_t r{};
        if constexpr (ByteNumberingV == ByteNumbering::Lsb0 || (ByteNumberingV == ByteNumbering::None && std::endian::native == std::endian::little))
        {
            for (auto i = 0zu; i < number_bytes.size(); ++i)
            {
                r += (static_cast<uint8_t>(number_bytes[i]) << (i * 8));
            }
        }
        else if constexpr (ByteNumberingV == ByteNumbering::Msb0 || (ByteNumberingV == ByteNumbering::None && std::endian::native == std::endian::big))
        {
            for (auto i = 0zu; i < number_bytes.size(); ++i)
            {
                r = r << 8 | number_bytes[i];
            }
        }

        return r;
    }
};

} // namespace abc

#endif // ABC_CONVERTER
