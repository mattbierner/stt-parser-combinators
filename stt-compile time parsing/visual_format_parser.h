/**
    Example parser for the Apple visual format language.
    
    Only can indicate if the parsing is successful or not. Produces error messages.
    Does not produce any meaningful values.
    
    Current implementation skimps on the implementation of identifiers and numbers.
 
    https://developer.apple.com/library/ios/documentation/UserExperience/Conceptual/AutolayoutPG/VisualFormatLanguage/VisualFormatLanguage.html
*/
#pragma once

#include "parse.h"
#include "utility.h"

namespace VisualFormat {

struct orientation : choice<character<'H'>, character<'V'>> { };

struct superview : character<'|'> { };

struct relation : choice<
    commitedString<'=', '='>,
    commitedString<'<', '='>,
    commitedString<'>', '='>> { };

struct positiveNumber : many1<anyDigit> { };

struct number :
    consParser<
        optional<character<'-'>>,
        positiveNumber>  { };

struct name : many1<anyLetter> { };

struct priority : choice<name, number> { };

struct constant : choice<name, number> { };

struct objectOfPredicate : choice<constant, name> { };

struct predicate : seq<
    optional<relation>,
    objectOfPredicate,
    optional<next<character<'@'>, commit<priority>>>> { };

struct predicateListWithParens :
    between<character<'('>, commit<character<')'>>,
        commit<sepBy1<character<','>, predicate>>> { };

struct simplePredicate : choice<
    name,
    positiveNumber> { };

struct predicateList : choice<
    simplePredicate,
    predicateListWithParens> { };

struct connection : choice<
    between<character<'-'>, commit<character<'-'>>,
        predicateList>,
    character<'-'>,
    always<None>> { };

struct view :
    between<character<'['>, commit<character<']'>>,
        commit<next<
            name,
            optional<predicateListWithParens, List<>>>>> { };

struct visualFormatString : seq<
    optional<then<orientation, commit<character<':'>>>>,
    optional<next<superview, connection>>,
    view,
    many<next<connection, view>>,
    optional<then<connection, superview>>,
    eof,
    always<decltype("Format string is valid"_stream)>> { };

} // VisualFormat