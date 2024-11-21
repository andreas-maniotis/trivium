/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "type_hull.hpp"
#include "tuple.hpp"
#include "text.hpp"
#include "value.hpp"


namespace lt
{
    struct S {};  //  S represents the S-combinator  lambda S = xyz . xz(yz).
    struct K {};  //  K represents the K-combinator  lambda K = xy.x.



    template<  typename...  >
    struct application;



    template<  typename... >
    struct s;



    template< int n >
    struct list;



    template<  typename... Xs  >
    using s_expr  = s<  Xs...  >;



    template<>
    struct application<>
    {
    private:

        template<  typename...  >
        friend struct application;


        template<  typename X  >
        using with_result  =  type_hull<X>;


// Make opposite expression:

        template<  typename... no_elements >
        struct r_expr
        {
            static_assert( sizeof...(no_elements) == 0 );


            using reverse = r_expr<>;


            template<  typename X  >
            using rcons = r_expr< X >;
        };



        template<  typename H,  typename... T  >
        struct r_expr< H, T... >
        {
            using reverse =   typename r_expr< T... >::reverse::template rcons< H >;


            template<  typename X  >
            using rcons  = r_expr<  H,  T...,  X  >;
        };



        template<  typename X  >
        struct opposite_expr
        :   with_result<  X  >
        { };



        template< int Zero >
        requires( Zero == 0 )
        struct opposite_expr<  s_expr< list<Zero>  >  >
        :   with_result<  s_expr<>  >
        { };



        template<  typename     X
                ,  typename...  Xs
                >
        struct opposite_expr<  s_expr<  X,  Xs... >  >
        :   with_result<  typename r_expr<  typename opposite_expr< X  >::type
                                         ,  typename opposite_expr< Xs >::type...  >::
                          reverse
                       >
        {
            static_assert(sizeof...(Xs) != 0
                         , "Forbidden nullary function call. No user-defined function f "
                           "may be called without arguments via the expression (f)." );
        };


//  Apply variable to opposite expression


        template<  typename,  typename   >
        struct ap_;



        template<  char... x  >
        struct ap_<  text< x... >,  text< x... >  >
        :   with_result<  s_expr< S, K, K >  >
        { };



        template<  char...   x
                ,  typename  Y
                >
        struct ap_< text<x...>,  Y  >
        :   with_result<  s_expr< K, Y  >  >
        { };



        template<  typename,  typename  >
        struct combine_;



        template<  char...   x
                ,  typename  Q
                ,  typename  P
                >
        struct ap_<  text<x...>,  r_expr<  Q,  P  >  >
        :   combine_<  typename  ap_< text<x...>, P >::type
                    ,  typename  ap_< text<x...>, Q >::type
                    >
        { };



        template<  char...      x
                ,  typename     Q
                ,  typename     P
                ,  typename...  Ps
                >
        struct ap_<  text<x...>,  r_expr<  Q,  P,  Ps...  >  >
        :   combine_<  typename  ap_<  text<x...>,  r_expr<  P,  Ps... >  >::type
                    ,  typename  ap_<  text<x...>,  Q  >::type
                    >
        { };



        template<  typename P,  typename Q  >
        struct combine_
        :   with_result<  s_expr< S,  P,  Q  >  >
        { };




        template<  typename P,  typename Q  >
        struct combine_<  s_expr< K, P >,  s_expr< K, Q >  >
        :   with_result<  s_expr<  K,  s_expr< P, Q >  >  >
        { };



        template<  typename     P
                ,  typename...  Ps
                ,  typename     Q
                >
        struct combine_<  s_expr< K, s_expr< P,  Ps... > >,  s_expr< K, Q >  >
        :   with_result<  s_expr<  K,  s_expr< P, Ps...,  Q >  >  >
        { };



        template<  typename     P  >
        struct combine_<  s_expr<  K,  P  >,  s_expr< S, K, K >  >
        :   with_result< P >
        { };
    };



    template<  char...  X  >
    struct application<  text< X... >  >
    {
        template<  typename Expr  >
        using apply_to  =
        typename application<>::
        template ap_<  text< X... >

                    ,  typename application<>::
                       template opposite_expr<  Expr  >::
                       type

                    >::type;
    };



    template<  typename      X
            ,  typename      Y
            ,  typename...   Ys
            >
    requires
    (
        text_type<X>  &&
        ( text_type<Y>  &&  ...  &&  text_type<Ys> )
    )
    struct application<  X,  Y,  Ys...  >
    {
        template<  typename  Expr  >
        using apply_to  =
        typename application<  X  >::
        template apply_to<  typename application<  Y,  Ys...  >::
                            template apply_to<  Expr  >
                         >;
    };
}
