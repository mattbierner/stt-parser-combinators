#pragma once

#include <ostream>

#include "printer.h"

/**
*/
template <typename pos, typename expected, typename found>
struct ExpectError { };

template <typename pos, typename expected, typename found>
struct Printer<ExpectError<pos, expected, found>>
{
    static std::ostream& Print(std::ostream& output)
    {
        output << "At:";
        Printer<pos>::Print(output) << " ";
        output << "Expected:";
        Printer<expected>::Print(output) << " ";
        output << "Found:";
        return Printer<found>::Print(output);
    }
};
