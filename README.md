# Compile Time C++ Parser Combinators

Demonstration of compile time parser combinators as a C++ template metaprogram. Allows parsing arbitrary strings at compile time to produce a type result.

Example shows the parsing of [Apple's autolayout visual format strings][visual format] at compile time to check their validity. This could easily be extended to produce an type representing the contents of the visual format string.

Syntax of parsers is very readable:


```cpp
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
    optional<None, then<orientation, character<':'>>>,
    optional<None, next<superview, connection>>,
    view,
    many<next<connection, view>>,
    optional<None, then<connection, superview>>> { };
```

[visual format]: https://developer.apple.com/library/ios/documentation/UserExperience/Conceptual/AutolayoutPG/VisualFormatLanguage/VisualFormatLanguage.html

