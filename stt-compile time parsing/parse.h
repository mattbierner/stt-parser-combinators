#pragma once

#include "parse_state.h"

/**
*/
template <typename parser, typename input>
using parse_t = call<parser, input>;

/**
    Succeed with value `x` without consuming any input.
*/
template <typename x>
struct always {
    template <typename input>
    using apply = identity<Result<true, x, input>>;
};

/**
    Fail with value `y` without consuming any input.
*/
template <typename x>
struct never {
    template <typename input>
    using apply = identity<Result<false, x, input>>;
};

/**
    Parser `p` and if it is successful, call `f` with result.
    `f` returns next parser to apply.
*/
template <typename p, typename f>
struct bind {
    template <typename result>
    struct andThen {
        using parser = call<f, typename result::value>;
        using type = parse_t<parser, typename result::rest>;
    };

    template <typename input>
    struct apply {
        using result = parse_t<p, input>;
        
        using type = typename std::conditional<result::success,
            andThen<result>,
            identity<result>>::type::type;
    };
};

/**
    Parse `p` then `q`, returns results from `q`.
*/
template <typename p, typename q>
struct next : bind<p, constant<q>> { };

/**
    Parsers the first match in an list of choices.
*/
template <typename option, typename... options>
struct seq :
    fold<mfunc<next>, option, options...>::type { };


/**
    Parse `p` then `q`, returns results from `p`.
*/
template <typename p, typename q>
struct then {
    struct andThen {
        template <typename result>
        using apply = identity<next<q, always<result>>>;
    };
    
    template <typename input>
    using apply = identity<parse_t<bind<p, andThen>, input>>;
};

/**
    Parse `p` or `q`.
*/
template <typename p, typename q>
struct either {
    template <typename s>
    struct apply {
        using result = parse_t<p, s>;
        
        using type = call<
            typename std::conditional<result::success,
                constant<result>,
                q>::type,
            s>;
    };
};

/**
    Parsers the first match in an list of choices.
*/
template <typename... options>
struct choice :
    fold<mfunc<either>, never<None>, options...>::type { };

/**
*/
template <typename def, typename p>
struct optional : either<p, always<def>> { };

/**
    Parser that consumes the first entry in the stream if it matches a given 
    function.
*/
template <typename test, typename s, typename input>
struct _token_apply {
    using type = Result<false, None, s>;
};

template <typename test, typename s, char c, char... input>
struct _token_apply<test, s, stream<c, input...>> {
    static const bool consume = test::template apply<c>::value;
    
    using result = typename std::conditional<consume,
        Value<char, c>,
        None>::type;
    
    using rest = typename std::conditional<consume,
        stream<input...>,
        stream<c, input...>>::type;
    
    using type = Result<consume, result, State<rest, typename s::position::next>>;
};

template <typename test>
struct token {
    template <typename s>
    using apply = _token_apply<test, s, typename s::input>;
};

/**
    Matches character `c`.
*/
template <char c>
struct character : token<equals<char, c>> { };

/**
    Matches end of input.
*/
struct Eof {
    template <typename input>
    struct apply {
        using type = Result<input::size() == 0, None, input>;
    };
};

/**
*/
template <typename p, typename q, typename f>
struct binary {
    struct inner1 {
        template <typename x>
        struct apply {
            struct inner2 {
                template <typename y>
                using apply = identity<call<f, x, y>>;
            };
            using type = bind<q, inner2>;
        };
    };

    template <typename input>
    using apply = identity<parse_t<bind<p, inner1>, input>>;
};

/**
*/
template <typename a, typename b>
struct consParser {
    struct appendOnto {
        template <typename x, typename y>
        using apply = identity<always<cons_t<x, y>>>;
    };
    
    template <typename input>
    using apply = identity<parse_t<binary<a, b, appendOnto>, input>>;
};

/**
    Parser `p` zero or more times.
    
    Builds a list of results.
*/
template <typename p>
struct many {
    template <typename input>
    using apply = identity<
        parse_t<
            either<
                consParser<p, many<p>>,
                always<List<>>>,
            input>>;
};

/**
    Parse `p` one or more times.
    
    Builds a list of results.
*/
template <typename p>
struct many1 : consParser<p, many<p>> { };

/**
*/
template <typename sep, typename p>
struct sepBy1 : consParser<p, many<next<sep, p>>> { };

/**
*/
template <typename sep, typename p>
struct sepBy :
    either<
        consParser<p, many<next<sep, p>>>,
        always<List<>>> { };

/**
    Parser `open`, then `body`, then `close`. Returns results from `body`.
*/
template <typename open, typename close, typename body>
struct between : next<open, then<body, close>> { };



template <char begin, char end>
struct inRange {
    template <char token>
    struct apply :
        std::integral_constant<bool, token >= begin && token <= end> { };
};

/**
*/
template<char begin, char end>
struct characterRanage : token<inRange<begin, end>> { };

/**
*/
struct anyLetter : choice<
    characterRanage<'a', 'z'>,
    characterRanage<'A', 'Z'>> { };

/**
*/
struct anyDigit : characterRanage<'0', '9'> { };

/**
*/
struct identifier : many1<anyLetter> { };

/**
*/
template <char... elements>
struct string : seq<
    character<elements>...,
    always<stream<elements...>>> { };


