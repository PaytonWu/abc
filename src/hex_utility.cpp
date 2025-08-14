// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/hex_utility.h>

namespace abc
{

#if !defined(ABC_CXX23)
std::bitset<256> const hex_utility::hex_flag{ "00000000000000000000000000000000"
                                              "00000000000000000000000000000000"
                                              "00000000000000000000000000000000"
                                              "00000000000000000000000000000000"
                                              "00000000000000000000000001111110" // abcdef
                                              "00000000000000000000000001111110" // ABCDEF
                                              "00000011111111110000000000000000" // 0123456789
                                              "00000000000000000000000000000000" };
#endif

auto
hex_utility::hex_char_to_binary(char const ch) noexcept -> abc::expected<byte_t, std::error_code>
{
    if ('0' <= ch && ch <= '9')
    {
        return static_cast<byte_t>(ch - '0');
    }

    if ('a' <= ch && ch <= 'f')
    {
        return static_cast<byte_t>(ch - 'a' + 10);
    }

    if ('A' <= ch && ch <= 'F')
    {
        return static_cast<byte_t>(ch - 'A' + 10);
    }

    return make_unexpected(make_error_code(std::errc::invalid_argument));
}

} // namespace abc
