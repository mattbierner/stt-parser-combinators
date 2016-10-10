/**
    Basic 1D list.
*/
#pragma once

#include <iosfwd>

#include "printer.h"
#include "utility.h"

/**
    Ordered list of types.
*/
template <typename... elements>
struct List {
    static const size_t size = sizeof...(elements);
};

/**
    Get the head of an list
*/
template <typename list>
struct car;

template <typename x, typename... xs>
struct car<List<x, xs...>> {
    using type = x;
};

template <typename list>
using car_t = typename car<list>::type;

/**
    Get the state of a list, excluding the head.
*/
template <typename list>
struct cdr;

template <typename x, typename... xs>
struct cdr<List<x, xs...>> {
    using type = List<xs...>;
};

template <typename list>
using cdr_t = typename cdr<list>::type;

/**
    Prepend a value onto a list
*/
template <typename x, typename list>
struct cons;

template <typename x, typename... xs>
struct cons<x, List<xs...>> {
     using type = List<x, xs...>;
};

template <typename x, typename list>
using cons_t = typename cons<x, list>::type;

/**
    Append a value on the end of a list.
*/
template <typename x, typename list>
struct append;

template <typename x, typename... xs>
struct append<x, List<xs...>> {
     using type = List<xs..., x>;
};

template <typename x, typename list>
using append_t = typename append<x, list>::type;

/*------------------------------------------------------------------------------
    Printer
*/
template <>
struct Printer<List<>>
{
    static std::ostream& Print(std::ostream& output) { return output; }
};

template <typename x, typename... xs>
struct Printer<List<x, xs...>>
{
    static std::ostream& Print(std::ostream& output)
    {
        Printer<x>::Print(output);
        return Printer<List<xs...>>::Print(output);
    }
};
