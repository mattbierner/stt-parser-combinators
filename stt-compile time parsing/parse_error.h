#pragma once

#include "printer.h"

/**
*/
template <typename pos, typename unexpected>
struct UnexpectError { };

template <typename pos, typename unexpected>
struct Printer<UnexpectError<pos, unexpected>>
{
    static std::ostream& Print(std::ostream& output)
    {
        output << "At:";
        Printer<pos>::Print(output) << " ";
        output << "Unexpected:";
        return Printer<unexpected>::Print(output);
    }
};

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