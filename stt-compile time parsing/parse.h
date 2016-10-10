#pragma once

#include "list.h"
#include "parse_error.h"
#include "parse_state.h"
#include "utility.h"
#include "value_list.h"
#include "value.h"

/**
*/
template <typename parser, typename input>
using parse = call<parser, input>;

/**
*/
template <typename parser, typename input>
using run_parser = typename parse<parser, State<input, Position<0>>>::value;

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
        using type = parse<parser, typename result::state>;
    };

    template <typename input>
    struct apply {
        using result = parse<p, input>;
        
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
    using apply = identity<parse<bind<p, andThen>, input>>;
};

/**
    Parse `p` or `q`.
*/
template <typename p, typename q>
struct either {
    template <typename s>
    struct apply {
        using result = parse<p, s>;
        
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
template <typename option, typename... options>
struct choice :
    fold<mfunc<either>, option, options...>::type { };

/**
    Run `p`. If it fails, succeed with `def`.
*/
template <typename p, typename def = None>
struct optional : either<p, always<def>> { };

/**
    Convert any failures in `p` into errors.
*/
template <typename p>
struct commit {
    template <typename s>
    struct apply {
        using result = parse<p, s>;
        
        using type = Result<
            (result::success == ResultType::Failure ? ResultType::Error : result::success),
            typename result::value,
            typename result::state>;
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

template <typename test, typename s, char c, char... input, typename error>
struct _token_apply<test, s, stream<c, input...>, error> {
    static const bool shouldConsume = test::template apply<c>::value;
    
    using type = typename std::conditional<shouldConsume,
        Result<
            ResultType::Success,
            Value<char, c>,
            State<stream<input...>, typename s::position::next>>,
        Result<
            ResultType::Failure,
            call<error, typename s::position, Value<char, c>>,
            State<stream<c, input...>, typename s::position>>>::type;
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
    using apply = identity<parse<token<equals<char, c>, error>, s>>;
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
    Parse `p` then `q`, combining results with `f`.
*/
template <typename p, typename q, typename f>
struct liftM2 {
    struct inner1 {
        template <typename x>
        struct apply {
            struct inner2 {
                template <typename y>
                using apply = identity<always<call<f, x, y>>>;
            };
            using type = bind<q, inner2>;
        };
    };

    template <typename s>
    using apply = identity<parse<bind<p, inner1>, s>>;
};

/**
    Conses the results of `a` onto `b`.
*/
template <typename a, typename b>
struct consParser : liftM2<a, b, mfunc<cons>> {};

/**
    Parser `p` zero or more times.
    
    Builds a list of results.
*/
template <typename p>
struct many :
    either<
        consParser<p, many<p>>,
        always<List<>>> { };

/**
    Parse `p` one or more times.
    
    Builds a list of results.
*/
template <typename p>
struct many1 : consParser<p, many<p>> { };

/**
    Parse one or more occurances of `p` seperated by `sep`.
    
    Builds a list of results.
*/
template <typename sep, typename p>
struct sepBy1 : consParser<p, many<next<sep, p>>> { };

/**
    Parse zero or more occurances of `p` seperated by `sep`.
    
    Builds a list of results.
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

/**
    Parse any character in `[begin, end]` character range.
*/
template<char begin, char end>
struct characterRanage {
    struct inRange {
        template <char token>
        struct apply :
            std::integral_constant<bool, token >= begin && token <= end> { };
    };

    struct error {
        template <typename pos, typename val>
        struct apply {
            using type = ExpectError<pos, stream<begin, '-', end>, val>;
        };
    };
    
    template <typename s>
    using apply = identity<parse<token<inRange, error>, s>>;
};

/**
    Parse any letter character.
*/
struct anyLetter : choice<
    characterRanage<'a', 'z'>,
    characterRanage<'A', 'Z'>> { };

/**
    Parse any digit character.
*/
struct anyDigit : characterRanage<'0', '9'> { };

/**
    Parse a sequence of characters in order.
*/
template <char... elements>
struct string : seq<
    character<elements>...,
    always<stream<elements...>>> { };

/**
    Parse a sequence of one or more characters in order.
    
    Fully fails if the first parsing succeeds but then any other character 
    fails.
*/
template <char first, char... state>
struct commitedString : seq<
    character<first>,
    commit<character<state>>...,
    always<stream<first, state...>>> { };

