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
    template<  typename T  >
    struct th
    {
        using type = T;
    };



    template<  typename X  >
    using type_hull  =  th< X >;



    template<  typename T
            ,  typename S
            >
    static constexpr bool operator==(  type_hull< T >,  type_hull< S >  )
    {
        return false;
    }



    template<  typename T  >
    static constexpr bool operator==(  type_hull< T >,  type_hull< T >  )
    {
        return true;
    }
}
