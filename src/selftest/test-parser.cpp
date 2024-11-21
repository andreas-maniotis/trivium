/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

 #define TRIVIUM_CHECK_IS_DYNAMIC
#include "lt/selftest/selftest.hpp"
#include "lt/selftest/configuration_space.hpp"


#include "lt/fundamental_types/s_expr.hpp"




// ================================================================================================
//
// Parsing of decimal / binary / octal / hexadecimal integers.
//
// ------------------------------------------------------------------------------------------------

using lt::operator""_s;


template< typename Expected,  typename... Acquired  >
using integer_test_set = std::tuple<  std::tuple< Expected,  Acquired >...  >;



using integer_test_0 =

integer_test_set<  lt::integer<0>
                    ,  decltype("0"_s)
                    ,  decltype("0b0"_s)
                    ,  decltype("00"_s)
                    ,  decltype("0x0"_s)
                    ,  decltype("-0"_s)
                    ,  decltype("-0b0"_s)
                    ,  decltype("-00"_s)
                    ,  decltype("-0x0"_s)
                    >;


using integer_test_1 =
integer_test_set<  lt::integer<1>
                    ,  decltype("1"_s)
                    ,  decltype("0b1"_s)
                    ,  decltype("01"_s)
                    ,  decltype("0x1"_s)
                    >;


using integer_test_minus_1 =
integer_test_set<  lt::integer<-1>
                    ,  decltype("-1"_s)
                    ,  decltype("-0b1"_s)
                    ,  decltype("-01"_s)
                    ,  decltype("-0x1"_s)
                    >;


using integer_test_123 =
integer_test_set<  lt::integer<123>
                    ,  decltype("123"_s)
                    ,  decltype("0b1111011"_s)
                    ,  decltype("0173"_s)
                    ,  decltype("0x7b"_s)
                    , decltype("0x7B"_s)
                    >;


using integer_test_minus_123 =
integer_test_set<  lt::integer<-123>
                    ,  decltype("-123"_s)
                    ,  decltype("-0b1111011"_s)
                    ,  decltype("-0173"_s)
                    ,  decltype("-0x7B"_s)
                    ,  decltype("-0x7b"_s)
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




// ================================================================================================
//
// s_expr
//
// ------------------------------------------------------------------------------------------------

//template<  typename Expr  >
//using lt::quoted = lt::s_expr< lt::text<'q','u','o','t','e'>, Expr  >;
//using lt::quoted = lt::quoted< Expr >;

TEST_CASE("s_expr")
{
    using list_0  =  decltype("()"_s);
    using list_0_ =  decltype("(())"_s);
    using list_1  =  decltype("(a)"_s);
    using list_2  =  decltype("(a b)"_s);
    using list_3  =  decltype("(a b c)"_s);
    using list_4  =  decltype("(a (b c) () d )"_s);

    using a_ = lt::text<'a'>;
    using b_ = lt::text<'b'>;
    using c_ = lt::text<'c'>;
    using d_ = lt::text<'d'>;

    lt::selftest::check_expression_equality<  list_0,  lt::s_expr<>  >();
    lt::selftest::check_expression_equality<  list_0_, lt::s_expr< lt::s_expr<> > >();
    lt::selftest::check_expression_equality<  list_1,  lt::s_expr< a_  >  >();
    lt::selftest::check_expression_equality<  list_2,  lt::s_expr< a_, b_ > >();
    lt::selftest::check_expression_equality<  list_3,  lt::s_expr< a_, b_, c_>  >();
    lt::selftest::check_expression_equality<  list_4,  lt::s_expr< a_, lt::s_expr< b_, c_ >,  lt::s_expr<>, d_ >  >();

    using quoted_list_0  =  decltype("'()"_s);
    using quoted_list_0_ =  decltype("'(())"_s);
    using quoted_list_1  =  decltype("'(a)"_s);
    using quoted_list_2  =  decltype("'(a b)"_s);
    using quoted_list_3  =  decltype("'(a b c)"_s);
    using quoted_list_4  =  decltype("'(a (b c) () d )"_s);


    lt::selftest::check_expression_equality<  quoted_list_0,  lt::quoted<lt::s_expr<>  >>();
    lt::selftest::check_expression_equality<  quoted_list_0_, lt::quoted<lt::s_expr< lt::s_expr<> > >>();
    lt::selftest::check_expression_equality<  quoted_list_1,  lt::quoted<lt::s_expr< a_  >  >>();
    lt::selftest::check_expression_equality<  quoted_list_2,  lt::quoted<lt::s_expr< a_, b_ > >>();
    lt::selftest::check_expression_equality<  quoted_list_3,  lt::quoted<lt::s_expr< a_, b_, c_>  >>();
    lt::selftest::check_expression_equality<  quoted_list_4,  lt::quoted<lt::s_expr< a_, lt::s_expr< b_, c_ >,  lt::s_expr<>, d_ > > >();


    using quoted_empty = decltype("'()"_s);
    using quoted_x     =  decltype("'(a '(b c) d)"_s);

    lt::selftest::check_expression_equality<  quoted_empty,  lt::quoted< lt::s_expr<> >  >();
    lt::selftest::check_expression_equality<  quoted_x
                                           ,  lt::quoted< lt::s_expr< a_, lt::quoted< lt::s_expr< b_, c_> >,  d_  >   >
                                           >();
}



// ================================================================================================
//
// Right-associative lists:  { a b c d ... } = ( a ( b ( c ( d .... )...)
//
// ( More concrete example:  { a b c} shall evaluate to ( a ( b c) )  ).
//
// ------------------------------------------------------------------------------------------------

//template<  typename Expr  >
//using lt::quoted = lt::s_expr< lt::text<'q','u','o','t','e'>, Expr  >;

TEST_CASE("right-associative lists")
{
    using list_0  =  decltype("{}"_s);
    using list_0_ =  decltype("{{}}"_s);
    using list_1  =  decltype("{a}"_s);
    using list_2  =  decltype("{a b}"_s);
    using list_3  =  decltype("{a b c}"_s);
    using list_4  =  decltype("{ a {b c} {} d }"_s);

    using a_ = lt::text<'a'>;
    using b_ = lt::text<'b'>;
    using c_ = lt::text<'c'>;
    using d_ = lt::text<'d'>;


    lt::selftest::check_expression_equality<  list_0,  lt::s_expr<>  >();
    lt::selftest::check_expression_equality<  list_0_, lt::s_expr< lt::s_expr<> > >();
    lt::selftest::check_expression_equality<  list_1,  lt::s_expr< a_  >  >();
    lt::selftest::check_expression_equality<  list_2,  lt::s_expr< a_, b_ > >();
    lt::selftest::check_expression_equality<  list_3,  lt::s_expr< a_, lt::s_expr< b_, c_> > >();



    using quoted_list_0  =  decltype("'{}"_s);
    using quoted_list_0_ =  decltype("'{{}}"_s);
    using quoted_list_1  =  decltype("'{a}"_s);
    using quoted_list_2  =  decltype("'{a b}"_s);
    using quoted_list_3  =  decltype("'{a b c}"_s);
    using quoted_list_4  =  decltype("'{ a {b c} {} d }"_s);

    using a_ = lt::text<'a'>;
    using b_ = lt::text<'b'>;
    using c_ = lt::text<'c'>;
    using d_ = lt::text<'d'>;


    lt::selftest::check_expression_equality<  quoted_list_0,  lt::quoted<  lt::s_expr<>  >  >();
    lt::selftest::check_expression_equality<  quoted_list_0_, lt::quoted<  lt::s_expr< lt::s_expr<>  >  > >();
    lt::selftest::check_expression_equality<  quoted_list_1,  lt::quoted<  lt::s_expr< a_  >  >  >();
    lt::selftest::check_expression_equality<  quoted_list_2,  lt::quoted<  lt::s_expr< a_, b_ >  >  >();
    lt::selftest::check_expression_equality<  quoted_list_3,  lt::quoted<  lt::s_expr< a_, lt::s_expr< b_, c_>  >  >  >();


    using quoted_empty = decltype("'{}"_s);
    using quoted_x     =  decltype("'{a '{b c} d}"_s);

    lt::selftest::check_expression_equality<  quoted_empty,  lt::quoted< lt::s_expr<> >  >();
    lt::selftest::check_expression_equality<  quoted_x,
                                              lt::quoted<  lt::s_expr<a_,  lt::s_expr<  lt::quoted< lt::s_expr< b_, c_> >,  d_ > >>
                                           >();

}



// ================================================================================================
//
// Lambda expression lambda x. (quote x)
// Systematic tests of applications are contained in the separate test-application.cpp.
//
// ------------------------------------------------------------------------------------------------


TEST_CASE(" [x]'x ")
{
    using lt::operator""_s;

    using expr_1  =  lt::value_type<"[x]'x"_s>;
    using Expr_1  =  lt::s_expr<  lt::K,  lt::quoted<  lt::value_type<"x"_s>  >  >;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >();
}



TEST_CASE("quoted lambda expression")
{
    using lt::operator""_s;

    using expr_1  =  lt::value_type<  "(quote [x](* 2 x))"_s  >;
    using Expr_1  =  lt::quoted<   lt::s_expr<  lt::value_type<"*"_s>,  lt::integer<2>  >  >;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >();
}
