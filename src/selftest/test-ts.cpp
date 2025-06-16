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


#include "lt/lib/ts.hpp"

// -----------------------------------------------------------------------------
//
// Prerequisistes for the tests:
//
// ------------------------------------------------------------------------------


using lt::operator""_text;

template<  typename... >
struct f {};


struct x {};
struct y {};


using irreducible_x = lt::s<  lt::value_type<"irreducible"_text>,  x  >;
using irreducible_y = lt::s<  lt::value_type<"irreducible"_text>,  y  >;



#include <vector>


TEST_CASE("class templates")
{
    using t =
    lt::lib::ts::metaprogram<"[x](list ( cpp_to_lisp x ) (sizeof x)  (alignof x) )", std::vector<int> >;

    lt::selftest::print_type<t>("std::vector<int>:  ");
}



// -----------------------------------------------------------------------------
//
// Class Templates:
//
// -----------------------------------------------------------------------------


TEST_CASE("class templates")
{
    using lt::operator""_text;


    using cpp_expr_1  =  lt::lib::ts::metaprogram< "[@X]( lisp_to_cpp (list 'class_template @X  ))"
                                                 ,  lt::class_template<f>
                                                 >;
    using cpp_Expr_1  =  f<>;


    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_2  =
    lt::lib::ts::metaprogram< "cpp_to_lisp", f<> >;



    using lisp_Expr_2  =  lt::s<  lt::value_type<"class_template"_text>
                               ,  lt::class_template<f>
                               >;

    lt::selftest::check_expression_equality<  lisp_expr_2,  lisp_Expr_2  >("lisp_expr_2: ");



    using cpp_expr_3 =
    lt::lib::ts::metaprogram< "[_f _x _y](  lisp_to_cpp (list 'class_template _f _x _y) )"
                            ,  lt::class_template<f>
                            ,  x
                            ,  y
                            >;

    using cpp_Expr_3 = f< x, y >;


    lt::selftest::check_expression_equality< cpp_expr_3, cpp_Expr_3 >("cpp_expr_3: ");



    using lisp_expr_4  =
    lt::lib::ts::metaprogram< "[@fxy]( cpp_to_lisp @fxy )",  f< x, y >  >;


    using lisp_Expr_4  =  lt::s<  lt::value_type<"class_template"_text>
                               ,  lt::class_template<f>
                               ,  irreducible_x
                               ,  irreducible_y
                               >;

    lt::selftest::check_expression_equality<  lisp_expr_4,  lisp_Expr_4  >("lisp_expr_4: ");
}



/*
// -----------------------------------------------------------------------------
//
// & / && / const / volatile / const volatile:
//
// -----------------------------------------------------------------------------


TEST_CASE("references")
{
    using lt::operator""_text;


    using cpp_expr_1  =
    lt::lib::ts::metaprogram< "[@x]( lisp_to_cpp (list '& @x ))",  x  >;


    using cpp_Expr_1  =  x&;
    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_2  =
    lt::lib::ts::metaprogram<  "[@x]( cpp_to_lisp @x)",  x&  >;



    using lisp_Expr_2  =
    lt::s<  lt::value_type<"&"_text>,  x  >;

    using lisp_Expr_2  =  lt::s<  lt::value_type<"&"_text>,  irreducible_x  >;

    lt::selftest::check_expression_equality<  lisp_expr_2,  lisp_Expr_2  >("lisp_expr_1: ");
}



TEST_CASE("rval-references")
{
    using lt::operator""_text;


    using cpp_expr_1  =
    lt::lib::ts::metaprogram< "[@x]( lisp_to_cpp (list '&& @x ))",  x  >;


    using cpp_Expr_1  =  x&&;
    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");


    using lisp_expr_2  =
    lt::lib::ts::metaprogram<  "[@x]( cpp_to_lisp @x)",  x&&  >;



    using lisp_Expr_2  =
    lt::s<  lt::value_type<"&&"_text>,  x  >;

    using lisp_Expr_2  =  lt::s<  lt::value_type<"&&"_text>,  x  >;

    lt::selftest::check_expression_equality<  lisp_expr_2,  lisp_Expr_2  >("lisp_expr_1: ");
}



TEST_CASE("const")
{
    using lt::operator""_text;


    using cpp_expr_1  =
    lt::lib::ts::metaprogram< "[@x]( lisp_to_cpp (list 'const @x ))",  x  >;


    using cpp_Expr_1  =  x const;
    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");


    using lisp_expr_1  =
    lt::lib::ts::metaprogram<  "[@x]( cpp_to_lisp @x)",  x const  >;



    using lisp_Expr_1  =
    lt::s<  lt::value_type<"const"_text>,  x  >;

    using lisp_Expr_1  =  lt::s<  lt::value_type<"const"_text>,  x  >;

    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("volatile")
{
    using lt::operator""_text;


    using cpp_expr_1  =
    lt::lib::ts::metaprogram< "[@x]( lisp_to_cpp (list 'volatile @x ))",  x  >;


    using cpp_Expr_1  =  x volatile;
    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");


    using lisp_expr_1  =
    lt::lib::ts::metaprogram<  "[@x]( cpp_to_lisp @x)",  x volatile  >;



    using lisp_Expr_1  =
    lt::s<  lt::value_type<"volatile"_text>,  x  >;

    using lisp_Expr_1  =  lt::s<  lt::value_type<"volatile"_text>,  x  >;

    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("const volatile")
{
    using lt::operator""_text;


    using cpp_expr_1  =
    lt::lib::ts::metaprogram< "[@x]( lisp_to_cpp (list 'const_volatile @x ))",  x  >;


    using cpp_Expr_1  =  x const volatile;
    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");


    using lisp_expr_1  =
    lt::lib::ts::metaprogram<  "[@x]( cpp_to_lisp @x)",  x const volatile  >;



    using lisp_Expr_1  =
    lt::s<  lt::value_type<"const_volatile"_text>,  x  >;

    using lisp_Expr_1  =  lt::s<  lt::value_type<"const_volatile"_text>,  x  >;

    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



// -----------------------------------------------------------------------------
//
// Arrays:
//
// -----------------------------------------------------------------------------


TEST_CASE("array []")
{
    using lt::operator""_text;



    using cpp_expr_1  =
    lt::lib::ts::metaprogram< "[@x]( lisp_to_cpp (list 'array @x ))",  x  >;

    using cpp_Expr_1  =  x [];

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    lt::lib::ts::metaprogram<  "[@x]( cpp_to_lisp @x)",  x []  >;

    using lisp_Expr_1  =
    lt::s<  lt::value_type<"array"_text>,  x  >;

    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("array [N]")
{
    using lt::operator""_text;



    using cpp_expr_1  =
    lt::lib::ts::metaprogram< "[@x]( lisp_to_cpp (list 'array @x 3 ))",  x  >;

    using cpp_Expr_1  = x [3];

    using cpp_Expr_1  =  x [3];
    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    lt::lib::ts::metaprogram<  "[@x]( cpp_to_lisp @x )",  x[3]  >;

    using lisp_Expr_1  =
    lt::s<  lt::value_type<"array"_text>,  x,  lt::value<3>  >;

    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



// -----------------------------------------------------------------------------
//
// Pointers.  * / *const / *volatile / *const volatile:
//
// -----------------------------------------------------------------------------


TEST_CASE("*")
{
    using lt::operator""_text;



    using cpp_expr_1  =
    lt::lib::ts::metaprogram< "[@x]( lisp_to_cpp (list '* @x ))",  x  >;

    using cpp_Expr_1  =  x*;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    lt::lib::ts::metaprogram<  "[@x]( cpp_to_lisp @x)",  x* >;

    using lisp_Expr_1  =
    lt::s<  lt::value_type<"*"_text>,  x  >;

    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("*const")
{
    using lt::operator""_text;



    using cpp_expr_1  =
    lt::lib::ts::metaprogram< "[@x]( lisp_to_cpp (list '*const @x ))",  x  >;

    using cpp_Expr_1  =  x *const;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    lt::lib::ts::metaprogram<  "[@x]( cpp_to_lisp @x)",  x *const >;

    using lisp_Expr_1  =
    lt::s<  lt::value_type<"*const"_text>,  x  >;

    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("*volatile")
{
    using lt::operator""_text;



    using cpp_expr_1  =
    lt::lib::ts::metaprogram< "[@x]( lisp_to_cpp (list '*volatile @x ))",  x  >;

    using cpp_Expr_1  =  x *volatile;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    lt::lib::ts::metaprogram<  "[@x]( cpp_to_lisp @x)",  x *volatile >;

    using lisp_Expr_1  =
    lt::s<  lt::value_type<"*volatile"_text>,  x  >;

    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("*const_volatile")
{
    using lt::operator""_text;



    using cpp_expr_1  =
    lt::lib::ts::metaprogram< "[@x]( lisp_to_cpp (list '*const_volatile @x ))",  x  >;

    using cpp_Expr_1  =  x *const volatile;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    lt::lib::ts::metaprogram<  "[@x]( cpp_to_lisp @x)",  x *const volatile >;

    using lisp_Expr_1  =
    lt::s<  lt::value_type<"*const_volatile"_text>,  x  >;

    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}

*/






/*

// -----------------------------------------------------------------------------
//
// Member Pointers:
//
// -----------------------------------------------------------------------------


TEST_CASE("Member Pointer ::*")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval< "( lisp_to_cpp (list (list '::*  (` 'x)  (` 'y) )))" >;
    using cpp_Expr_1  =  y  x::*;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"::*"_text>,  x,  y  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



// -----------------------------------------------------------------------------
//
// Functions that may throw:
//
// -----------------------------------------------------------------------------


TEST_CASE("x(y): ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) ) ))"  >;


    using cpp_Expr_1  =  x(y);

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



template<  typename > struct abominable_type { };

TEST_CASE("x(y) &: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) '& ) ))"  >;


    using cpp_Expr_1  =  x(y) &;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"&"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) &&: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) '&& ) ))"  >;


    using cpp_Expr_1  =  x(y) &&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"&&"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}


TEST_CASE("x(y) const: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const ) ))"  >;


    using cpp_Expr_1  =  x(y) const;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"const"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) volatile: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile ) ))"  >;


    using cpp_Expr_1  =  x(y) volatile;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"volatile"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const volatile: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile ) ))"  >;


    using cpp_Expr_1  =  x(y) const volatile;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"const_volatile"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const&: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const& ) ))"  >;


    using cpp_Expr_1  =  x(y) const&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"const&"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) volatile&: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile& ) ))"  >;


    using cpp_Expr_1  =  x(y) volatile&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"volatile&"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const volatile&: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile& ) ))"  >;


    using cpp_Expr_1  =  x(y) const volatile&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"const_volatile&"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const&&: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const&& ) ))"  >;


    using cpp_Expr_1  =  x(y) const&&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"const&&"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) volatile&&: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile&& ) ))"  >;


    using cpp_Expr_1  =  x(y) volatile&&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"volatile&&"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const volatile&&: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile&& ) ))"  >;


    using cpp_Expr_1  =  x(y) const volatile&&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"const_volatile&&"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



// -----------------------------------------------------------------------------
//
// Functions with noexcept specifier:
//
// -----------------------------------------------------------------------------


TEST_CASE("x(y) noexcept: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) noexcept;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               , lt::value_type<"noexcept"_text>
                               >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("x(y) &  noexcept: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) '& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) & noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"&"_text>
                               ,  lt::value_type<"noexcept"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) &&  noexcept: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) '&& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) && noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"&&"_text>
                               ,  lt::value_type<"noexcept"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}


TEST_CASE("x(y) const  noexcept: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) const noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"const"_text>
                               ,  lt::value_type<"noexcept"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) volatile  noexcept: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) volatile noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"volatile"_text>
                               ,  lt::value_type<"noexcept"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const volatile  noexcept: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) const volatile noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"const_volatile"_text>
                               ,  lt::value_type<"noexcept"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const&  noexcept: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) const& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"const&"_text>
                               ,  lt::value_type<"noexcept"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) volatile&  noexcept: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) volatile& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"volatile&"_text>
                               ,  lt::value_type<"noexcept"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const volatile&  noexcept: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) const volatile& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"const_volatile&"_text>
                               ,  lt::value_type<"noexcept"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const&&  noexcept: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const&& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) const&& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"const&&"_text>
                               ,  lt::value_type<"noexcept"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) volatile&&  noexcept: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile&& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) volatile&& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"volatile&&"_text>
                               ,  lt::value_type<"noexcept"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const volatile&&  noexcept: ")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval<
    "( lisp_to_cpp (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile&& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) const volatile&& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( cpp_to_lisp  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"const_volatile&&"_text>
                               ,  lt::value_type<"noexcept"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}
*/
