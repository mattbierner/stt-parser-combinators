/**
    Some basic helper meta programming functions.
*/
#pragma once

/**
    Value equality check.
*/
template <typename T, T a>
struct equals {
    template <T b>
    struct apply : std::integral_constant<bool, a == b> { };
};

/**
    Metafunction that returns `T`.
*/
template <typename T>
struct identity {
    using type = T;
};

/**
    Create a metafunction that returns `T` when called with any arguments.
*/
template <typename T>
struct constant {
    template <typename...>
    using apply = identity<T>;
};

/**
    Call a metafunction with a set of arguments.
*/
template <typename f, typename... args>
using call = typename f::template apply<args...>::type;

/**
    Compose two metafunctions to create `f(g(x))`
*/
template <typename f, typename g>
struct compose {
    template <typename... args>
    using apply = call<f, call<g, args...>>;
};

/**
*/
template <template<typename...> class f>
struct mfunc {
    template <typename... args>
    using apply = identity<f<args...>>;
};

/**
    Perform a left fold on a set of template parameter.
*/
template <typename f, typename z, typename...>
struct fold {
    using type = z;
};

template <typename f, typename z, typename x, typename... xs>
struct fold<f, z, x, xs...> {
    using type = typename fold<f, call<f, z, x>, xs...>::type;
};

template <typename T, T x>
struct Value { };

struct None { };


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