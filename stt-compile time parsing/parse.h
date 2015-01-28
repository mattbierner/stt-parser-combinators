#pragma once

#include "parse_error.h"
#include "parse_state.h"
#include "value_list.h"

/**
*/
template <typename parser, typename input>
using parse_t = call<parser, input>;

/**
*/
template <typename parser, typename input>
using run_parser = typename parse_t<parser, State<input, Position<0>>>::value;

/**
    Succeed with value `x` without consuming any input.
*/
template <typename x>
struct always {
    template <typename input>
    using apply = identity<Result<ResultType::Success, x, input>>;
};

/**
    Fail with value `y` without consuming any input.
*/
template <typename x>
struct never {
    template <typename input>
    using apply = identity<Result<ResultType::Failure, x, input>>;
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
        
        using type = typename std::conditional<result::success == ResultType::Success,
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
            typename std::conditional<result::success == ResultType::Failure,
                q,
                constant<result>>::type,
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
    Run `p`. If it fails, succeed with `def`.
*/
template <typename def, typename p>
struct optional : either<p, always<def>> { };

/**
*/
template <typename p>
struct commit {
    template <typename s>
    struct apply {
        using result = parse_t<p, s>;
        
        using type = Result<
            (result::success == ResultType::Failure ? ResultType::CommitedFailure : result::success),
            typename result::value,
            typename result::rest>;
    };
};

/**
    Parser that consumes the first entry in the stream if it matches a given 
    function.
*/
template <typename test, typename s, typename input, typename error>
struct _token_apply {
    using type = Result<
        ResultType::Failure,
        call<error, typename s::position, decltype("eof"_stream)>,
        s>;
};

template <typename test, typename s, char c, char... input,  typename error>
struct _token_apply<test, s, stream<c, input...>, error> {
    static const bool consume = test::template apply<c>::value;
    
    using result = typename std::conditional<consume,
        Value<char, c>,
        call<error, typename s::position, Value<char, c>>>::type;
    
    using rest = typename std::conditional<consume,
        stream<input...>,
        stream<c, input...>>::type;
    
    using type = Result<
        (consume ? ResultType::Success : ResultType::Failure),
        result,
        State<rest, typename s::position::next>>;
};

template <typename test, typename error = constant<None>>
struct token {
    template <typename s>
    using apply = _token_apply<test, s, typename s::input, error>;
};

/**
    Matches character `c`.
*/
template <char c>
struct character {
    struct error {
        template <typename pos, typename val>
        struct apply {
            using type = ExpectError<pos, Value<char, c>, val>;
        };
    };
    
    template <typename s>
    using apply = identity<parse_t<token<equals<char, c>, error>, s>>;
};

/**
    Matches end of input.
*/
struct eof {
    template <typename s>
    struct apply {
        using type = Result<
            s::input::size() == 0 ? ResultType::Success : ResultType::Failure,
            None,
            s>;
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

    template <typename s>
    using apply = identity<parse_t<bind<p, inner1>, s>>;
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


