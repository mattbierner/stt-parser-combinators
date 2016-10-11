#pragma once

#include <ostream>
#include <utility>

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

template <char x, char... xs>
struct Printer<stream<x, xs...>>
{
    static std::ostream& Print(std::ostream& output)
    {
        return Printer<stream<xs...>>::Print(output << x);
    }
};
