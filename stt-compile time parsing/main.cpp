#include <iostream>

#include "functor.h"
#include "printer.h"
#include "utility.h"
#include "list.h"
#include "value_list.h"
#include "visual_format_parser.h"


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



int main(int argc, const char* argv[])
{
    using parser = VisualFormat::visualFormatString;
    using x = run_parser<parser, decltype("H:|-[a(==2@20)]-|"_stream)>;
   
    Printer<x>::Print(std::cout);
    return 0;
}
