/**
    Example parser for the Apple visual format language.
    
    Only can indicate if the parsing is successful or not. Does not produce any
    meaningful values or error messages.
 
    https://developer.apple.com/library/ios/documentation/UserExperience/Conceptual/AutolayoutPG/VisualFormatLanguage/VisualFormatLanguage.html
*/
#pragma once

#include "parse.h"
#include "utility.h"

namespace VisualFormat {

struct orientation : choice<character<'H'>, character<'V'>> { };

struct superview : character<'|'> { };

struct relation : choice<
    string<'=', '='>,
    string<'<', '='>,
    string<'>', '='>> { };

struct number : many1<anyDigit> { };

struct viewName : identifier { };

struct priority : choice<identifier, number> { };

struct constant : choice<identifier, number> { };

struct objectOfPredicate : choice<constant, viewName> { };

struct predicate : seq<
    optional<None, relation>,
    objectOfPredicate,
    optional<None, next<character<'@'>, priority>>> { };

struct predicateList : sepBy1<character<','>, predicate> { };

struct predicateListWithParens :
    between<character<'('>, character<')'>,
        predicateList> { };

struct connection : choice<
    between<character<'-'>, character<'-'>,
        predicateList>,
    character<'-'>,
    always<None>> { };

struct view :
    between<character<'['>, character<']'>,
        next<
            viewName,
            optional<List<>, predicateListWithParens>>> { };

struct visualFormatString : seq<
    optional<None, then<orientation, commit<character<':'>>>>,
    optional<None, next<superview, connection>>,
    view,
    many<next<connection, view>>,
    optional<None, then<connection, superview>>> { };

} // VisualFormat