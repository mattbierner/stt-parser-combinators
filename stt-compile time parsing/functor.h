#pragma once

template <typename x, typename f>
struct Fmap;



template <typename x, typename f>
using fmap_t = typename Fmap<x, f>::type;