#pragma once

#include "printer.h"

/**
    Character input.
*/
template <char... chars>
using stream = std::integer_sequence<char, chars...>;

/**
*/
template<typename T, T... chars>
constexpr auto operator""_stream()
{
    return stream<chars...>{};
}

/*------------------------------------------------------------------------------
 * Printer
 */
template <>
struct Printer<stream<>>
{
    static std::ostream& Print(std::ostream& output) { return output; }
};

template <char x, char... rest>
struct Printer<stream<x, rest...>>
{
    static std::ostream& Print(std::ostream& output)
    {
        return Printer<stream<rest...>>::Print(output << x);
    }
};