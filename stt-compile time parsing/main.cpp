#include <iostream>

#include "functor.h"
#include "printer.h"
#include "utility.h"
#include "list.h"
#include "value_list.h"
#include "visual_format_parser.h"




int main(int argc, const char* argv[])
{
    using parser = VisualFormat::visualFormatString;
    using x = run_parser<parser, decltype("H:|-(>=2)-[a(==2@10)]-|"_stream)>;
   
   
    Printer<x>::Print(std::cout);
    return 0;
}
