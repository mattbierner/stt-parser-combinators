#pragma once

#include <ostream>

#include "printer.h"

/**
    How a parser returned.
*/
enum class ResultType
{
    /** Parsing was successful. */
    Success,
    
    /** Parsing failed but we can try to recover. */
    Failure,
    
    /** Parsing failed and we cannot recover. */
    Error
};

/**
    Position in the parsed string.
*/
template <size_t i>
struct Position {
    static const size_t index = i;
    
    using next = Position<i + 1>;
};

template <size_t index>
struct Printer<Position<index>>
{
    static std::ostream& Print(std::ostream& output)
    {
        return output << "Position:" << index;
    }
};

/**
    Parsing state.
*/
template <typename i, typename pos>
struct State {
    using input = i;
    using position = pos;
};


template <typename input, typename position>
struct Printer<State<input, position>>
{
    static std::ostream& Print(std::ostream& output)
    {
        Printer<input>::Print(output);
        Printer<position>::Print(output);
        return output;
    }
};

/**
    Result of parsing.
*/
template <ResultType suc, typename x, typename s>
struct Result
{
    static const ResultType success = suc;
    using value = x;
    using state = s;
};

template <bool suc, typename val, typename r>
struct Printer<Result<suc, val, r>>
{
    static std::ostream& Print(std::ostream& output)
    {
        output << "Result:" << static_cast<int>(suc) << " (";
        Printer<val>::Print(output) << ") ";
        Printer<r>::Print(output);
        return output;
    }
};
