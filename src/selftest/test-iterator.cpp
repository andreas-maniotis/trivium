/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////


#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

//  #define TRIVIUM_CHECK_IS_DYNAMIC
#include "lt/selftest/selftest.hpp"
#include "lt/selftest/configuration_space.hpp"


#include "lt/interpreter.hpp"



template< typename X  >
using sizeof_ =  lt::integer<  sizeof(X) >;


TEST_CASE("Iterator")
{
    using expr_1 = lt::eval<"(first '(11 22 33))",  lt::map{} >;

    using Expr_1 = lt::integer<11>;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >("expr_1: ");


    using nav_2  =
    decltype(
        lt::map{}.add_or_replace(  lt::assign<  "x",       int  >
                                ,  lt::assign<  "sizeof",  lt::combinator<  1,  sizeof_  >  >
                                ,  lt::assign<  "x",       double  >
                                )
    );



    using expr_3  =  lt::eval<"( ( ` 'sizeof) ( ` 'x)  )",  nav_2{} >;
    using Expr_3  =  sizeof_<double>;

    lt::selftest::check_expression_equality< expr_3,  Expr_3 >("expr_3: ");

    using expr_3a =  lt::eval<"( @'sizeof @'x )",  nav_2{}  >;

    lt::selftest::check_expression_equality< expr_3a, Expr_3 >("expr_3a: ");

    using expr_3b = lt::eval<"( @'sizeof @(first '(x y)) )", nav_2{} >;

    lt::selftest::check_expression_equality< expr_3b, Expr_3 >("expr_3b: ");




    using nav_3  =
    decltype( nav_2::add_or_replace(  lt::assign<  "y",  char  >  ));



    using Nav_3  =
    decltype(
        lt::map<>::add_or_replace(  lt::assign<  "x",     double  >
                                 ,  lt::assign<  "sizeof",  lt::combinator<  1,  sizeof_  >  >
                                 ,  lt::assign<  "y",       char  >
                                 )
    );



    lt::selftest::check_expression_equality<  nav_3,  Nav_3  >("nav_3: ");
}
