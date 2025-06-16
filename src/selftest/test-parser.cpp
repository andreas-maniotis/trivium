/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

// #define TRIVIUM_CHECK_IS_DYNAMIC
#include "lt/selftest/selftest.hpp"
#include "lt/selftest/configuration_space.hpp"


#include "lt/s_expr.hpp"



// -----------------------------------------------------------------------------
//
// Parsing of decimal / binary / octal / hexadecimal integers.
//
// -----------------------------------------------------------------------------



template< typename Expected,  typename... Acquired  >
using integer_test_set = std::tuple<  std::tuple< Expected,  Acquired >...  >;



using integer_test_0 =

integer_test_set<  lt::integer<0>
                ,  lt::s_expr<"0">
                ,  lt::s_expr<"0b0">
                ,  lt::s_expr<"00">
                ,  lt::s_expr<"0x0">
                ,  lt::s_expr<"-0">
                ,  lt::s_expr<"-0b0">
                ,  lt::s_expr<"-00">
                ,  lt::s_expr<"-0x0">
                >;


using integer_test_1 =
integer_test_set<  lt::integer<1>
                ,  lt::s_expr<"1">
                ,  lt::s_expr<"0b1">
                ,  lt::s_expr<"01">
                ,  lt::s_expr<"0x1">
                >;


using integer_test_minus_1 =
integer_test_set<  lt::integer<-1>
                ,  lt::s_expr<"-1">
                ,  lt::s_expr<"-0b1">
                ,  lt::s_expr<"-01">
                ,  lt::s_expr<"-0x1">
                >;


using integer_test_123 =
integer_test_set<  lt::integer<123>
                ,  lt::s_expr<"123">
                ,  lt::s_expr<"0b1111011">
                ,  lt::s_expr<"0173">
                ,  lt::s_expr<"0x7b">
                ,  lt::s_expr<"0x7B">
                >;


using integer_test_minus_123 =
integer_test_set<  lt::integer<-123>
                ,  lt::s_expr<"-123">
                ,  lt::s_expr<"-0b1111011">
                ,  lt::s_expr<"-0173">
                ,  lt::s_expr<"-0x7b">
                ,  lt::s_expr<"-0x7B">
                >;



TEST_CASE_TEMPLATE_DEFINE( "numbers equal",  T, numbers_equal_test  )
{
    using left  = std::tuple_element_t<0, T>;
    using right = std::tuple_element_t<1, T>;

    lt::selftest::check_expression_equality< left, right >();
}


TEST_CASE_TEMPLATE_APPLY( numbers_equal_test, integer_test_0 );

TEST_CASE_TEMPLATE_APPLY( numbers_equal_test, integer_test_1 );

TEST_CASE_TEMPLATE_APPLY( numbers_equal_test, integer_test_minus_1 );

TEST_CASE_TEMPLATE_APPLY( numbers_equal_test, integer_test_123 );

TEST_CASE_TEMPLATE_APPLY( numbers_equal_test, integer_test_minus_123 );



// -----------------------------------------------------------------------------
//
// trailing whitespace
//
// -----------------------------------------------------------------------------

TEST_CASE("trailing whitespace")
{
    using expr_1a  =  lt::s_expr<"3 ">;
    using expr_1b  =  lt::s_expr<"3  ">;

    using Expr_1  =  lt::integer<3>;

    lt::selftest::check_expression_equality<  expr_1a,  Expr_1  >("expr_1a: ");
    lt::selftest::check_expression_equality<  expr_1b,  Expr_1  >("expr_1b: ");

}




// -----------------------------------------------------------------------------
//
// lt::s_expr
//
// -----------------------------------------------------------------------------

//template<  typename Expr  >
//using lt::quoted = lt::s_expr< lt::text<'q','u','o','t','e'>, Expr  >;
//using lt::quoted = lt::quoted< Expr >;

TEST_CASE("lt::s_expr")
{
    using list_0  =  lt::s_expr<"()">;
    using list_0_ =  lt::s_expr<"(())">;
    using list_1  =  lt::s_expr<"(a)">;
    using list_2  =  lt::s_expr<"(a b)">;
    using list_3  =  lt::s_expr<"(a b c)">;
    using list_4  =  lt::s_expr<"(a (b c) () d )">;

    using a_ = lt::text<'a'>;
    using b_ = lt::text<'b'>;
    using c_ = lt::text<'c'>;
    using d_ = lt::text<'d'>;

    lt::selftest::check_expression_equality<  list_0,  lt::nil  >();
    lt::selftest::check_expression_equality<  list_0_, lt::s< lt::nil > >();
    lt::selftest::check_expression_equality<  list_1,  lt::s< a_  >  >();
    lt::selftest::check_expression_equality<  list_2,  lt::s< a_, b_ > >();
    lt::selftest::check_expression_equality<  list_3,  lt::s< a_, b_, c_>  >();
    lt::selftest::check_expression_equality<  list_4,  lt::s< a_, lt::s< b_, c_ >,  lt::nil, d_ >  >();

    using quoted_list_0  =  lt::s_expr<"'()">;
    using quoted_list_0_ =  lt::s_expr<"'(())">;
    using quoted_list_1  =  lt::s_expr<"'(a)">;
    using quoted_list_2  =  lt::s_expr<"'(a b)">;
    using quoted_list_3  =  lt::s_expr<"'(a b c)">;
    using quoted_list_4  =  lt::s_expr<"'(a (b c) () d )">;


    lt::selftest::check_expression_equality<  quoted_list_0,  lt::quoted<lt::nil  >>();
    lt::selftest::check_expression_equality<  quoted_list_0_, lt::quoted<lt::s< lt::nil > >>();
    lt::selftest::check_expression_equality<  quoted_list_1,  lt::quoted<lt::s< a_  >  >>();
    lt::selftest::check_expression_equality<  quoted_list_2,  lt::quoted<lt::s< a_, b_ > >>();
    lt::selftest::check_expression_equality<  quoted_list_3,  lt::quoted<lt::s< a_, b_, c_>  >>();
    lt::selftest::check_expression_equality<  quoted_list_4,  lt::quoted<lt::s< a_, lt::s< b_, c_ >,  lt::nil, d_ > > >();


    using quoted_empty = lt::s_expr<"'()">;
    using quoted_x     =  lt::s_expr<"'(a '(b c) d)">;

    lt::selftest::check_expression_equality<  quoted_empty,  lt::quoted< lt::nil >  >();
    lt::selftest::check_expression_equality<  quoted_x
                                           ,  lt::quoted< lt::s< a_, lt::quoted< lt::s< b_, c_> >,  d_  >   >
                                           >();
}



// -----------------------------------------------------------------------------
//
// Right-associative lists:  { a b c d ... } = ( a ( b ( c ( d .... )...)
//
// ( More concrete example:  { a b c} shall evaluate to ( a ( b c) )  ).
//
// -----------------------------------------------------------------------------

//template<  typename Expr  >
//using lt::quoted = lt::s_expr< lt::text<'q','u','o','t','e'>, Expr  >;

TEST_CASE("right-associative lists")
{
    using list_0  =  lt::s_expr<"{}">;
    using list_0_ =  lt::s_expr<"{{}}">;
    using list_1  =  lt::s_expr<"{a}">;
    using list_2  =  lt::s_expr<"{a b}">;
    using list_3  =  lt::s_expr<"{a b c}">;
    using list_4  =  lt::s_expr<"{ a {b c} {} d }">;

    using a_ = lt::text<'a'>;
    using b_ = lt::text<'b'>;
    using c_ = lt::text<'c'>;
    using d_ = lt::text<'d'>;


    lt::selftest::check_expression_equality<  list_0,  lt::nil  >();
    lt::selftest::check_expression_equality<  list_0_, lt::s< lt::nil > >();
    lt::selftest::check_expression_equality<  list_1,  lt::s< a_  >  >();
    lt::selftest::check_expression_equality<  list_2,  lt::s< a_, b_ > >();
    lt::selftest::check_expression_equality<  list_3,  lt::s< a_, lt::s< b_, c_> > >();



    using quoted_list_0  =  lt::s_expr<"'{}">;
    using quoted_list_0_ =  lt::s_expr<"'{{}}">;
    using quoted_list_1  =  lt::s_expr<"'{a}">;
    using quoted_list_2  =  lt::s_expr<"'{a b}">;
    using quoted_list_3  =  lt::s_expr<"'{a b c}">;
    using quoted_list_4  =  lt::s_expr<"'{ a {b c} {} d }">;

    using a_ = lt::text<'a'>;
    using b_ = lt::text<'b'>;
    using c_ = lt::text<'c'>;
    using d_ = lt::text<'d'>;


    lt::selftest::check_expression_equality<  quoted_list_0,  lt::quoted<  lt::nil  >  >();
    lt::selftest::check_expression_equality<  quoted_list_0_, lt::quoted<  lt::s< lt::nil  >  > >();
    lt::selftest::check_expression_equality<  quoted_list_1,  lt::quoted<  lt::s< a_  >  >  >();
    lt::selftest::check_expression_equality<  quoted_list_2,  lt::quoted<  lt::s< a_, b_ >  >  >();
    lt::selftest::check_expression_equality<  quoted_list_3,  lt::quoted<  lt::s< a_, lt::s< b_, c_>  >  >  >();


    using quoted_empty = lt::s_expr<"'{}">;
    using quoted_x     =  lt::s_expr<"'{a '{b c} d}">;

    lt::selftest::check_expression_equality<  quoted_empty,  lt::quoted< lt::nil >  >();
    lt::selftest::check_expression_equality<  quoted_x,
                                              lt::quoted<  lt::s<a_,  lt::s<  lt::quoted< lt::s< b_, c_> >,  d_ > > >
                                           >();

}



// -----------------------------------------------------------------------------
//
// Lambda expression lambda x. (quote x)
// Systematic tests of applications are contained in the separate test-application.cpp.
//
// -----------------------------------------------------------------------------


TEST_CASE(" [x]'x ")
{
    using expr_1  =  lt::s_expr<"[x]'x">;
    using Expr_1  =  lt::s<  lt::K,  lt::quoted<  lt::s_expr<"x">  >  >;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >();
}



TEST_CASE("quoted lambda expression")
{
    using expr_1  =  lt::s_expr<  "(quote [x](* 2 x))"  >;
    using Expr_1  =  lt::quoted<   lt::s<  lt::s_expr<"*">,  lt::integer<2>  >  >;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >();
}
