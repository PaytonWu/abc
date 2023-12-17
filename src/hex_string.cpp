// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/hex_string.h>

namespace abc {

/// @brief convert a hex string to binary bytes.
/// @tparam ByteNumbering specify the byte numbering of the output bytes.
/// @param string_slice input hex string slice.
/// @return bytes object or an error code object.
template <byte_numbering ByteNumbering>
static auto
to_bytes(std::string_view string_slice) -> expected<bytes<ByteNumbering>, std::error_code>
{
    if (hex_utility::has_hex_prefix(string_slice))
    {
        string_slice = string_slice.substr(2);
    }

    if (!hex_utility::is_hex_without_prefix(string_slice))
    {
        return make_unexpected(make_error_code(std::errc::invalid_argument));
    }

    abc::bytes<ByteNumbering> binary_data;
    binary_data.reserve((string_slice.size() + 1) / 2);
    if constexpr (ByteNumbering == byte_numbering::msb0)
    {
        if (string_slice.size() & 1)
        {
            [[maybe_unused]] auto r = hex_utility::hex_char_to_binary(string_slice.front()).transform([&binary_data, &string_slice](auto const b) mutable {
                binary_data.push_back(b);
                string_slice.remove_prefix(1);
            });
            assert(r.has_value());
        }

        auto const & chunks = string_slice | ranges::views::chunk(2);
        ranges::for_each(chunks, [&binary_data](ranges::viewable_range auto && compound_byte) mutable {
            byte byte{};
            for (auto const [i, nibble_byte]: compound_byte | ranges::views::reverse | ranges::views::enumerate)
            {
                byte |= hex_char_to_binary(nibble_byte).value() << (4 * i);
            }
            binary_data.push_back(byte);
        });

        return binary_data;
    }

    if constexpr (ByteNumbering == byte_numbering::lsb0)
    {
        if (string_slice.size() & 1)
        {
            [[maybe_unused]] auto r = hex_utility::hex_char_to_binary(string_slice.front()).transform([&binary_data, &string_slice](auto const b) mutable {
                binary_data.push_back(b);
                string_slice.remove_prefix(1);
            });
            assert(r.has_value());
        }

        auto const & chunks = string_slice | ranges::views::chunk(2);
        ranges::for_each(chunks, [&binary_data](ranges::viewable_range auto && compound_byte) mutable {
            byte byte{};
            for (auto const [i, nibble_byte]: compound_byte | ranges::views::reverse | ranges::views::enumerate)
            {
                byte |= hex_utility::hex_char_to_binary(nibble_byte).value() << (4 * i);
            }
            binary_data.push_back(byte);
        });

        ranges::reverse(binary_data);

        return binary_data;
    }

    unreachable();
}

auto hex_string::from(std::string_view input) -> expected<hex_string, std::error_code> {
    return to_bytes<byte_numbering::lsb0>(input).transform([](auto && bytes) { return hex_string{ std::forward<decltype(bytes)>(bytes) }; });
}

}