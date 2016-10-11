#pragma once

#include <ios>
#include <ostream>

#include "printer.h"

template <typename T, T x>
struct Value { };

struct None { };


template <typename T, T x>
struct Printer<Value<T, x>>
{
    static std::ostream& Print(std::ostream& output)
    {
        return output << std::boolalpha << x;
    }
};

template <>
struct Printer<None>
{
    static std::ostream& Print(std::ostream& output) { return output; }
};
