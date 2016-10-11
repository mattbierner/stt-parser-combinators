/**
    Some basic helper meta programming functions.
*/
#pragma once

#include <utility>

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
    Transform a template into a metafunction.
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
