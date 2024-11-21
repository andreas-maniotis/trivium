/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


namespace lt
{
    template<  auto x,  typename T = decltype(x)  >
    struct v
    {
        using type = T;
    };


    template<  auto x,  typename T  >
    struct v<  x,  T const  >
    {
        using type = T;
    };


    template< auto const x  >
    using value = v< x >;



    template<  int const n  >
    using integer  =  value< n >;


    // value_type removes const qualifiers:
    // value_type<x> is T for an object of type "T const"
    template<  auto x  >
    using value_type = typename value<x>::type;
}
