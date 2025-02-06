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


#include "lt/symbolic_type_system.hpp"
#include "lt/map.hpp"
#include "lt/text.hpp"
#include "lt/interpreter.hpp"


// -----------------------------------------------------------------------------
//
// Prerequisistes for the tests:
//
// ------------------------------------------------------------------------------



template<  typename... >
struct f {};


struct x {};
struct y {};



constexpr auto  nav  =

lt::map{  lt::assign<  "C++",       lt::combinator< 1, lt::ts::cpp >  >
       ,  lt::assign<  "Symbolic",  lt::combinator< 1, lt::ts::symbolic >  >
       ,  lt::assign<  "f",         lt::class_template< f >  >
       ,  lt::assign<  "x",         x  >
       ,  lt::assign<  "y",         y  >
       };



template< lt::text<>::literal prog >
using nav_eval = lt::eval< prog,  nav >;



template<  typename...  Entries  >
struct nav_add_or_replace_t
{
    template<  lt::text<>::literal program  >
    using eval = lt::eval<  program,  nav.add_or_replace(  Entries{}...  )  >;
};



// -----------------------------------------------------------------------------
//
// Class Templates:
//
// -----------------------------------------------------------------------------


TEST_CASE("class templates")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval< "( (` 'C++) (list 'class_template (` 'f)  ))" >;
    using cpp_Expr_1  =  f<>;


    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >) >::eval< "( (` 'Symbolic)  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"class_template"_text>
                               ,  lt::class_template<f>
                               >;

    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");



    using cpp_expr_2  =  nav_eval< "( (` 'C++) (list(list 'class_template (` 'f)  (` 'x))))" >;
    using cpp_Expr_2  =  f<x>;



    lt::selftest::check_expression_equality< cpp_expr_2, cpp_Expr_2 >("cpp_expr_2: ");



    using lisp_expr_2  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_2  >)  >::eval< "( (` 'Symbolic)  (` 'z))" >;


    using lisp_Expr_2   =  lt::s<  lt::value_type<"class_template"_text>
                                ,  lt::class_template<f>
                                ,  x
                                >;

    lt::selftest::check_expression_equality<  lisp_expr_2,  lisp_Expr_2  >("lisp_expr_2: ");
}



// -----------------------------------------------------------------------------
//
// & / && / const / volatile / const volatile:
//
// -----------------------------------------------------------------------------


TEST_CASE("references")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval< "( (` 'C++) (list (list '& (` 'x) )))" >;
    using cpp_Expr_1  =  x&;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"&"_text>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("rval-references")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval< "( (` 'C++) (list (list '&& (` 'x) )))" >;
    using cpp_Expr_1  =  x&&;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"&&"_text>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("const")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval< "( (` 'C++) (list (list 'const (` 'x) )))" >;
    using cpp_Expr_1  =  x const;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"const"_text>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("volatile")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval< "( (` 'C++) (list (list 'volatile (` 'x) )))" >;
    using cpp_Expr_1  =  x volatile;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"volatile"_text>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("const volatile")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval< "( (` 'C++) (list (list 'const_volatile (` 'x) )))" >;
    using cpp_Expr_1  =  x const volatile;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"const_volatile"_text>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



// -----------------------------------------------------------------------------
//
// Array:
//
// -----------------------------------------------------------------------------


TEST_CASE("array []")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval< "( (` 'C++) (list (list 'array (` 'x) )))" >;
    using cpp_Expr_1  =  x [];

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"array"_text>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("array [N]")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval< "((` 'C++) (list (list 'array (` 'x) 3 )))" >;
    using cpp_Expr_1  =  x [3];

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"array"_text>,  x,  lt::integer<3>   >;


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


    using cpp_expr_1  =  nav_eval< "( (` 'C++) (list (list '* (` 'x) )))" >;
    using cpp_Expr_1  =  x *;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"*"_text>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("*const")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval< "( (` 'C++) (list (list '*const (` 'x) )))" >;
    using cpp_Expr_1  =  x *const;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"*const"_text>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("*volatile")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval< "( (` 'C++) (list (list '*volatile (` 'x) )))" >;
    using cpp_Expr_1  =  x *volatile;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"*volatile"_text>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("*const_volatile")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval< "( (` 'C++) (list (list '*const_volatile (` 'x) )))" >;
    using cpp_Expr_1  =  x *const volatile;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"*const_volatile"_text>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



// -----------------------------------------------------------------------------
//
// Member Pointers:
//
// -----------------------------------------------------------------------------


TEST_CASE("Member Pointer ::*")
{
    using lt::operator""_text;


    using cpp_expr_1  =  nav_eval< "( (` 'C++) (list (list '::*  (` 'x)  (` 'y) )))" >;
    using cpp_Expr_1  =  y  x::*;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) ) ))"  >;


    using cpp_Expr_1  =  x(y);

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) '& ) ))"  >;


    using cpp_Expr_1  =  x(y) &;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) '&& ) ))"  >;


    using cpp_Expr_1  =  x(y) &&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const ) ))"  >;


    using cpp_Expr_1  =  x(y) const;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile ) ))"  >;


    using cpp_Expr_1  =  x(y) volatile;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile ) ))"  >;


    using cpp_Expr_1  =  x(y) const volatile;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const& ) ))"  >;


    using cpp_Expr_1  =  x(y) const&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile& ) ))"  >;


    using cpp_Expr_1  =  x(y) volatile&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile& ) ))"  >;


    using cpp_Expr_1  =  x(y) const volatile&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const&& ) ))"  >;


    using cpp_Expr_1  =  x(y) const&&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile&& ) ))"  >;


    using cpp_Expr_1  =  x(y) volatile&&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile&& ) ))"  >;


    using cpp_Expr_1  =  x(y) const volatile&&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) noexcept;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) '& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) & noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) '&& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) && noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) const noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) volatile noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) const volatile noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) const& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) volatile& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) const volatile& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const&& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) const&& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile&& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) volatile&& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



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
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile&& 'noexcept) ))"  >;


    using cpp_Expr_1  =  x(y) const volatile&& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav_add_or_replace_t<  decltype(lt::assign<  "z",  cpp_Expr_1  >)  >::eval< "( (` 'Symbolic)  (` 'z) )" >;



    using lisp_Expr_1  =  lt::s<  lt::value_type<"function"_text>
                               ,  lt::s<  lt::value_type<"signature"_text>,  x,  y  >
                               ,  lt::value_type<"const_volatile&&"_text>
                               ,  lt::value_type<"noexcept"_text>
                               >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}
