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
#include "lt/navigator.hpp"
#include "lt/fundamental_types/combinator.hpp"



// -----------------------------------------------------------------------------
//
// Prerequisistes for the tests:
//
// ------------------------------------------------------------------------------


using lt::operator""_s;


template<  typename... >
struct f {};


struct x {};
struct y {};


using nav  =
lt::navigator<  lt::parameter<      "C++"_s,  lt::combinator<1, lt::ts::cpp>  >
             ,  lt::parameter< "Symbolic"_s,  lt::combinator<1, lt::ts::symbolic>  >
             ,  lt::parameter<        "f"_s,  lt::class_template< f >  >
             ,  lt::parameter<        "x"_s,  x  >
             ,  lt::parameter<        "y"_s,  y  >
             >;


// -----------------------------------------------------------------------------
//
// Class Templates:
//
// -----------------------------------------------------------------------------


TEST_CASE("class templates")
{

    using cpp_expr_1  =  nav::eval< "( (` 'C++) (list 'class_template (` 'f)  ))"_s >;
    using cpp_Expr_1  =  f<>;


    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"class_template"_s>
                                    ,  lt::class_template<f>
                                    >;

    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");



    using cpp_expr_2  =  nav::eval< "( (` 'C++) (list(list 'class_template (` 'f)  (` 'x))))"_s >;
    using cpp_Expr_2  =  f<x>;



    lt::selftest::check_expression_equality< cpp_expr_2, cpp_Expr_2 >("cpp_expr_2: ");



    using lisp_expr_2  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_2 >  >::eval< "( (` 'Symbolic)  (` 'z))"_s >;


    using lisp_Expr_2   =  lt::s_expr<  lt::value_type<"class_template"_s>
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

    using cpp_expr_1  =  nav::eval< "( (` 'C++) (list (list '& (` 'x) )))"_s >;
    using cpp_Expr_1  =  x&;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"&"_s>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("rval-references")
{

    using cpp_expr_1  =  nav::eval< "( (` 'C++) (list (list '&& (` 'x) )))"_s >;
    using cpp_Expr_1  =  x&&;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"&&"_s>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("const")
{

    using cpp_expr_1  =  nav::eval< "( (` 'C++) (list (list 'const (` 'x) )))"_s >;
    using cpp_Expr_1  =  x const;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"const"_s>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("volatile")
{

    using cpp_expr_1  =  nav::eval< "( (` 'C++) (list (list 'volatile (` 'x) )))"_s >;
    using cpp_Expr_1  =  x volatile;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"volatile"_s>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("const volatile")
{

    using cpp_expr_1  =  nav::eval< "( (` 'C++) (list (list 'const_volatile (` 'x) )))"_s >;
    using cpp_Expr_1  =  x const volatile;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"const_volatile"_s>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



// -----------------------------------------------------------------------------
//
// Array:
//
// -----------------------------------------------------------------------------


TEST_CASE("array []")
{

    using cpp_expr_1  =  nav::eval< "( (` 'C++) (list (list 'array (` 'x) )))"_s >;
    using cpp_Expr_1  =  x [];

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"array"_s>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("array [N]")
{

    using cpp_expr_1  =  nav::eval< "((` 'C++) (list (list 'array (` 'x) 3 )))"_s >;
    using cpp_Expr_1  =  x [3];

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"array"_s>,  x,  lt::integer<3>   >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");

}



// -----------------------------------------------------------------------------
//
// Pointers.  * / *const / *volatile / *const volatile:
//
// -----------------------------------------------------------------------------


TEST_CASE("*")
{
    using cpp_expr_1  =  nav::eval< "( (` 'C++) (list (list '* (` 'x) )))"_s >;
    using cpp_Expr_1  =  x *;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"*"_s>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("*const")
{
    using cpp_expr_1  =  nav::eval< "( (` 'C++) (list (list '*const (` 'x) )))"_s >;
    using cpp_Expr_1  =  x *const;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"*const"_s>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("*volatile")
{
    using cpp_expr_1  =  nav::eval< "( (` 'C++) (list (list '*volatile (` 'x) )))"_s >;
    using cpp_Expr_1  =  x *volatile;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"*volatile"_s>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("*const_volatile")
{
    using cpp_expr_1  =  nav::eval< "( (` 'C++) (list (list '*const_volatile (` 'x) )))"_s >;
    using cpp_Expr_1  =  x *const volatile;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"*const_volatile"_s>,  x  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



// -----------------------------------------------------------------------------
//
// Member Pointers:
//
// -----------------------------------------------------------------------------


TEST_CASE("Member Pointer ::*")
{

    using cpp_expr_1  =  nav::eval< "( (` 'C++) (list (list '::*  (` 'x)  (` 'y) )))"_s >;
    using cpp_Expr_1  =  y  x::*;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"::*"_s>,  x,  y  >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



// -----------------------------------------------------------------------------
//
// Functions that may throw:
//
// -----------------------------------------------------------------------------


TEST_CASE("x(y): ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) ) )) "_s  >;


    using cpp_Expr_1  =  x(y);

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



template<  typename > struct abominable_type { };

TEST_CASE("x(y) &: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) '& ) )) "_s  >;


    using cpp_Expr_1  =  x(y) &;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"&"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) &&: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) '&& ) )) "_s  >;


    using cpp_Expr_1  =  x(y) &&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"&&"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}


TEST_CASE("x(y) const: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const ) )) "_s  >;


    using cpp_Expr_1  =  x(y) const;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"const"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) volatile: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile ) )) "_s  >;


    using cpp_Expr_1  =  x(y) volatile;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"volatile"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const volatile: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile ) )) "_s  >;


    using cpp_Expr_1  =  x(y) const volatile;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"const_volatile"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const&: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const& ) )) "_s  >;


    using cpp_Expr_1  =  x(y) const&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"const&"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) volatile&: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile& ) )) "_s  >;


    using cpp_Expr_1  =  x(y) volatile&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"volatile&"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const volatile&: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile& ) )) "_s  >;


    using cpp_Expr_1  =  x(y) const volatile&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"const_volatile&"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const&&: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const&& ) )) "_s  >;


    using cpp_Expr_1  =  x(y) const&&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"const&&"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) volatile&&: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile&& ) )) "_s  >;


    using cpp_Expr_1  =  x(y) volatile&&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"volatile&&"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const volatile&&: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile&& ) )) "_s  >;


    using cpp_Expr_1  =  x(y) const volatile&&;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"const_volatile&&"_s>
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

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'noexcept) )) "_s  >;


    using cpp_Expr_1  =  x(y) noexcept;

    lt::selftest::check_expression_equality< cpp_expr_1, cpp_Expr_1 >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    , lt::value_type<"noexcept"_s>
                                    >;


    lt::selftest::check_expression_equality<  lisp_expr_1,  lisp_Expr_1  >("lisp_expr_1: ");
}



TEST_CASE("x(y) &  noexcept: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) '& 'noexcept) )) "_s  >;


    using cpp_Expr_1  =  x(y) & noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"&"_s>
                                    ,  lt::value_type<"noexcept"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) &&  noexcept: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) '&& 'noexcept) )) "_s  >;


    using cpp_Expr_1  =  x(y) && noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"&&"_s>
                                    ,  lt::value_type<"noexcept"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}


TEST_CASE("x(y) const  noexcept: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const 'noexcept) )) "_s  >;


    using cpp_Expr_1  =  x(y) const noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"const"_s>
                                    ,  lt::value_type<"noexcept"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) volatile  noexcept: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile 'noexcept) )) "_s  >;


    using cpp_Expr_1  =  x(y) volatile noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"volatile"_s>
                                    ,  lt::value_type<"noexcept"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const volatile  noexcept: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile 'noexcept) )) "_s  >;


    using cpp_Expr_1  =  x(y) const volatile noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"const_volatile"_s>
                                    ,  lt::value_type<"noexcept"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const&  noexcept: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const& 'noexcept) )) "_s  >;


    using cpp_Expr_1  =  x(y) const& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"const&"_s>
                                    ,  lt::value_type<"noexcept"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) volatile&  noexcept: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile& 'noexcept) )) "_s  >;


    using cpp_Expr_1  =  x(y) volatile& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"volatile&"_s>
                                    ,  lt::value_type<"noexcept"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const volatile&  noexcept: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile& 'noexcept) )) "_s  >;


    using cpp_Expr_1  =  x(y) const volatile& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"const_volatile&"_s>
                                    ,  lt::value_type<"noexcept"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const&&  noexcept: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const&& 'noexcept) )) "_s  >;


    using cpp_Expr_1  =  x(y) const&& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"const&&"_s>
                                    ,  lt::value_type<"noexcept"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) volatile&&  noexcept: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'volatile&& 'noexcept) )) "_s  >;


    using cpp_Expr_1  =  x(y) volatile&& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"volatile&&"_s>
                                    ,  lt::value_type<"noexcept"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}



TEST_CASE("x(y) const volatile&&  noexcept: ")
{

    using cpp_expr_1  =  nav::eval<
    "( (` 'C++) (list ( list  'function  (list 'signature  (` 'x)  (` 'y)  ) 'const_volatile&& 'noexcept) )) "_s  >;


    using cpp_Expr_1  =  x(y) const volatile&& noexcept;

    lt::selftest::check_expression_equality< abominable_type<cpp_expr_1>
                                           , abominable_type<cpp_Expr_1 >
                                           >("cpp_expr_1: ");



    using lisp_expr_1  =
    nav::add_entries<  lt::parameter< "z"_s,  cpp_Expr_1 >  >::eval< "( (` 'Symbolic)  (` 'z) )"_s >;



    using lisp_Expr_1  =  lt::s_expr<  lt::value_type<"function"_s>
                                    ,  lt::s_expr<  lt::value_type<"signature"_s>,  x,  y  >
                                    ,  lt::value_type<"const_volatile&&"_s>
                                    ,  lt::value_type<"noexcept"_s>
                                    >;


    lt::selftest::check_expression_equality<  abominable_type<lisp_expr_1>
                                           ,  abominable_type<lisp_Expr_1>
                                           >("lisp_expr_1: ");
}
