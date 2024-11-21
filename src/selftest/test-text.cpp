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


#include "lt/fundamental_types/text.hpp"



template< char... txt >
struct some_other_text;




// ================================================================================================
//
//  Concept text_type:
//  text_type<T> is satisfied if and only if T is an instantiation of the clas template 
//  text< char... >
//
// ------------------------------------------------------------------------------------------------

template<  char... >     struct g;
template<  typename... > struct h;

TEST_CASE("text_type concept")
{
    TRIVIUM_CHECK( ! lt::text_type<  int  > );
    TRIVIUM_CHECK( ! lt::text_type<  g<'a','b'>  > );
    TRIVIUM_CHECK( ! lt::text_type<  h<double>  > );

    TRIVIUM_CHECK( lt::text_type<  lt::text<>  > );
    TRIVIUM_CHECK( lt::text_type<  lt::text<'a'>  >);
    TRIVIUM_CHECK( lt::text_type<  lt::text<'a','b','c'>  > );
}



// ================================================================================================
//
//  Literal operator ""_t for texts
//
// ------------------------------------------------------------------------------------------------


TEST_CASE("text")
{
    using lt::operator ""_t;


    auto  text_1   = ""_t;
    auto  text_2   = "abc"_t;


    lt::selftest::check_expression_equality<  decltype(text_1),  lt::text<>  >();
    lt::selftest::check_expression_equality<  decltype(text_2),  lt::text< 'a', 'b', 'c' > >();
}



// ================================================================================================
//
//  A class template of the form  template< char... > class X together with a
//  user defined literal that uses text<>::literal
//
// ------------------------------------------------------------------------------------------------


template<  char...  >
struct another_text {};



template<  lt::text<>::literal  some_text  >
constexpr auto operator""_another_text()
{
    return lt::text<>::as_type<  another_text,  some_text  >();
}



TEST_CASE("user constructed literals" )
{
    auto from_user_constructed_literal  =  "hello"_another_text;

    lt::selftest::check_expression_equality<  decltype(from_user_constructed_literal)
                                           ,  another_text< 'h','e', 'l','l','o'  >   >();

    // check that the literal operator ""_another_text is constexpr.

    static constexpr auto is_constexpr = "hello"_another_text;
    static_cast<void>(is_constexpr);
}
