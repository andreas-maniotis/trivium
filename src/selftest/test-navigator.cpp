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


#include "lt/navigator.hpp"
#include "lt/fundamental_types/combinator.hpp"
#include "lt/lisp/linker.hpp"


template< typename X  >
using sizeof_ =  lt::integer<  sizeof(X) >;


TEST_CASE("Navigator")
{
    using lt::operator""_s;

    using expr_1 = lt::navigator<>::eval<"(first '(11 22 33))"_s>;;
    using Expr_1 = lt::integer<11>;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >("expr_1: ");


    using expr_2  =  lt::navigator<>::with<"(def sum + )"_s>::
                     eval<"( sum 2 3 )"_s>;
    using Expr_2  =  lt::value_type<"5"_s>;

    lt::selftest::check_expression_equality<  expr_2,  Expr_2  >("expr_2: ");



    using nav_2  =
    lt::navigator<>::add_entries<  lt::parameter<  "x"_s,       int  >
                                ,  lt::parameter<  "sizeof"_s,  lt::combinator< 1, sizeof_ >  >
                                ,  lt::parameter<  "x"_s,       double  >
                                >;

    using Nav_2 = lt::navigator<  lt::parameter< "x"_s,  double >
                               ,  lt::parameter< "sizeof"_s,  lt::combinator< 1, sizeof_ >  >
                               >;


    lt::selftest::check_expression_equality<  nav_2,  Nav_2 >("nav_2: ");



    using expr_3  =  nav_2::eval<"( (` 'sizeof) ( ` 'x)  )"_s>;
    using Expr_3  =  sizeof_<double>;

    lt::selftest::check_expression_equality< expr_3,  Expr_3 >("expr_3: ");



    using expr_4  =  nav_2::lookup<"x"_s>;
    using Expr_4  =  double;

    lt::selftest::check_expression_equality< expr_4,  Expr_4 >("expr_4: ");



    using nav_3  =
    nav_2::add_entries<  lt::parameter<  "y"_s,  char  >  >;


    using Nav_3 = lt::navigator<  lt::parameter< "x"_s,  double >
                               ,  lt::parameter< "sizeof"_s,  lt::combinator< 1, sizeof_ >  >
                               ,  lt::parameter< "y"_s,  char >
                               >;

    lt::selftest::check_expression_equality<  nav_3,  Nav_3  >("nav_3: ");
}
