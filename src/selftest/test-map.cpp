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


#include "lt/map.hpp"
#include "lt/text.hpp"

#include <type_traits>


template<  auto  >
struct object {};


template< auto x >
using value_type = std::decay_t< decltype(x) >;


template<  typename K,  typename V  >
struct key_value_pair
{
    using key    =  K;
    using value  =  V;
};



using lt::operator""_text;


using key_1_t  =  decltype( "key_1"_text );
using key_2_t  =  decltype( "key_2"_text );
using key_3_t  =  decltype( "key_3"_text );


using value_1 = int*;
using value_2 = int**;
using value_3 = int***;

using entry_1 =  key_value_pair<  key_1_t,  value_1  >;
using entry_2 =  key_value_pair<  key_2_t,  value_2  >;
using entry_3 =  key_value_pair<  key_3_t,  value_3  >;



lt::map< key_value_pair<int, double>,  key_value_pair<int*, void> >* mp;


using t_0 = lt::map<>;


using t_1 = lt::map< entry_1  >;


using t_2 = lt::map<  entry_1,  entry_2  >;


using t_3 = lt::map<  entry_1,  entry_2,  entry_3  >;



// -----------------------------------------------------------------------------
//
// Lookup-tables (maps) have literal types (and can thus be the type
// of non-type template parameters).
//
// -----------------------------------------------------------------------------


TEST_CASE("maps have literal types")
{
    object<  t_0{}  >  o_0;
    object<  t_1{}  >  o_1;
    object<  t_2{}  >  o_2;
    object<  t_3{}  >  o_3;

    static_cast<void>(o_0);
    static_cast<void>(o_1);
    static_cast<void>(o_2);
    static_cast<void>(o_3);
}


// -----------------------------------------------------------------------------
//
// Test whether a key is contained in a map
//
// -----------------------------------------------------------------------------


TEST_CASE("map::contains")
{
    TRIVIUM_CHECK(  ! t_0::contains_key< value_type<"key_1"_text> >() );
    TRIVIUM_CHECK(    t_1::contains_key< value_type<"key_1"_text> >() );
    TRIVIUM_CHECK(    t_2::contains_key< value_type<"key_1"_text> >() );
    TRIVIUM_CHECK(    t_3::contains_key< value_type<"key_1"_text> >() );


    TRIVIUM_CHECK(  ! t_1::contains_key< value_type<"key_2"_text> >() );
    TRIVIUM_CHECK(    t_2::contains_key< value_type<"key_2"_text> >() );
    TRIVIUM_CHECK(    t_3::contains_key< value_type<"key_1"_text> >() );

    TRIVIUM_CHECK(  ! t_1::contains_key< value_type<"key_3"_text> >() );
    TRIVIUM_CHECK(  ! t_2::contains_key< value_type<"key_3"_text> >() );
    TRIVIUM_CHECK(    t_3::contains_key< value_type<"key_3"_text> >() );
}


// -----------------------------------------------------------------------------
//
// Addition of new entries to maps.
//
// Assume that the map has the type
//
//      map :=: map<  type< Key, Value >...  >.
//
//
//  The call map::add_or_replace_types<  key_value_pair< K, V >  >  yields
//  the following result R:
//
//
//  (a)  R  :=:  map<  type< Key, Value >...,  key_value_pair< K, V >
//       whenever the key K is not contained in the parameter pack  Key...
//
//
//  (b)  R :=:  map<  key_value_pair< Key, Value'>  >...  >
//
//              with  Value' :=: Value when K and Key are different
//                    Value' :=: V     when K and Key are identical.
//
//      In short: the unique parameter P in the parameter pack
//      "key_value_pair< Key,  Value >..." the key of which coincides
//      with K is replaced by the key_value_pair P' :=: key_value_pair< K, V >.
//      Alt the other parameters are left unchanged.
//
//
//
//  add_or_replace_types<> does nothing.
//
//
//  add_or_replace_types< E1,  R... >
//  performs map::add_or_replace_types<E1>::add_or_replace_types<R....> recursively.
//
// -----------------------------------------------------------------------------

TEST_CASE("add entries")
{
    using entry_4 =  key_value_pair<  decltype("key_4"_text),  value_1  >;


    using v_0 = t_0::add_or_replace_types< entry_4 >;
    using v_1 = t_1::add_or_replace_types< entry_4 >;
    using v_2 = t_2::add_or_replace_types< entry_4 >;


    using V_0 = lt::map< entry_4 >;
    using V_1 = lt::map<  entry_1,  entry_4  >;
    using V_2  = lt::map<  entry_1,  entry_2,  entry_4  >;


    lt::selftest::check_expression_equality< v_0, V_0 >();
    lt::selftest::check_expression_equality< v_1, V_1 >();
    lt::selftest::check_expression_equality< v_2, V_2 >();


    using entry_1_ = key_value_pair< key_1_t,  int(*)()  >;


    using x_1 = t_1::add_or_replace_types<  entry_1_  >;
    using x_2 = t_2::add_or_replace_types<  entry_1_ >;
    using x_3 = t_3::add_or_replace_types<  entry_1_ >;


    using X_1 = lt::map<  entry_1_ >;
    using X_2 = lt::map<  entry_1_,  entry_2  >;
    using X_3 = lt::map<  entry_1_,  entry_2,  entry_3  >;


    lt::selftest::check_expression_equality< x_1, X_1 >();
    lt::selftest::check_expression_equality< x_2, X_2 >();
    lt::selftest::check_expression_equality< x_3, X_3 >();



    using entry_2_ = key_value_pair<  key_2_t, int (**)() >;

    using y_2 = t_2::add_or_replace_types<  entry_2_ >;

    using Y_2 = lt::map<  entry_1,  entry_2_  >;

    lt::selftest::check_expression_equality< y_2, Y_2 >();



    using entry_3_ = key_value_pair<  key_3_t,  int (***)()  >;

    using z_3 = t_3::add_or_replace_types<  entry_3_  >;

    using Z_3 = lt::map<  entry_1,  entry_2,  entry_3_  >;

    lt::selftest::check_expression_equality< z_3, Z_3 >();
}



// -----------------------------------------------------------------------------
//
// Lookup of symbols:
//
//
// map::lookup< Key >
//
// evaluates to Value whenever  key_value_pair< Key, Value >
// whenever  key_value_pair< key, Value > is contained in map.
// Otherwise compilation error arises.
//
//
// map::lookup< Key, Fallback >
// evaluates to...
//
//  ... Value when key_value_pair< Key, Value > is contained in the map
//
//  ... Fallback  otherwise.
//
//
// ------------------------------------------------------------------------------------------------


struct substitution_failure {};



template<  typename,  typename... >
struct lookup_test
{
    using result = substitution_failure;
};



template<  typename Map,  typename... P  >
requires
requires(  typename Map::template lookup< P...  >  )
{
    nullptr;
}
struct lookup_test<  Map,  P...  >
{
    using result = typename Map::template lookup< P... >;
};


TEST_CASE("lookup without fallback value")
{
    using a_0 = lookup_test<  t_0,  key_1_t  >::result;
    using a_1 = lookup_test<  t_1,  key_1_t  >::result;
    using a_2 = lookup_test<  t_2,  key_1_t  >::result;
    using a_3 = lookup_test<  t_3,  key_1_t  >::result;

    using A_0  =  substitution_failure;
    using A_1  =  value_1;
    using A_2  =  value_1;
    using A_3  =  value_1;

    lt::selftest::check_expression_equality< a_0,  A_0 >();
    lt::selftest::check_expression_equality< a_1,  A_1 >();
    lt::selftest::check_expression_equality< a_2,  A_2 >();
    lt::selftest::check_expression_equality< a_3,  A_3 >();



    using b_0 = lookup_test< t_0,  key_2_t >::result;
    using b_1 = lookup_test< t_1,  key_2_t >::result;
    using b_2 = lookup_test< t_2,  key_2_t >::result;
    using b_3 = lookup_test< t_3,  key_2_t >::result;

    using B_0  =  substitution_failure;
    using B_1  =  substitution_failure;
    using B_2  =  value_2;
    using B_3  =  value_2;

    lt::selftest::check_expression_equality< b_0,  B_0 >();
    lt::selftest::check_expression_equality< b_1,  B_1 >();
    lt::selftest::check_expression_equality< b_2,  B_2 >();
    lt::selftest::check_expression_equality< b_3,  B_3 >();


    using c_0 = lookup_test< t_0,  key_3_t >::result;
    using c_1 = lookup_test< t_1,  key_3_t >::result;
    using c_2 = lookup_test< t_2,  key_3_t >::result;
    using c_3 = lookup_test< t_3,  key_3_t >::result;

    using C_0  =  substitution_failure;
    using C_1  =  substitution_failure;
    using C_2  =  substitution_failure;
    using C_3  = value_3;

    lt::selftest::check_expression_equality< c_0,  C_0 >();
    lt::selftest::check_expression_equality< c_1,  C_1 >();
    lt::selftest::check_expression_equality< c_2,  C_2 >();
    lt::selftest::check_expression_equality< c_3,  C_3 >();
}

struct fallback {};




TEST_CASE("lookup with fallback value")
{
    using a_0 = lookup_test<  t_0,  key_1_t,  fallback  >::result;
    using a_1 = lookup_test<  t_1,  key_1_t,  fallback  >::result;
    using a_2 = lookup_test<  t_2,  key_1_t,  fallback  >::result;
    using a_3 = lookup_test<  t_3,  key_1_t,  fallback  >::result;

    using A_0  =  fallback;
    using A_1  =  value_1;
    using A_2  =  value_1;
    using A_3  =  value_1;

    lt::selftest::check_expression_equality< a_0,  A_0  >();
    lt::selftest::check_expression_equality< a_1,  A_1  >();
    lt::selftest::check_expression_equality< a_2,  A_2  >();
    lt::selftest::check_expression_equality< a_3,  A_3  >();



    using b_0 = lookup_test< t_0,  key_2_t,  fallback  >::result;
    using b_1 = lookup_test< t_1,  key_2_t,  fallback  >::result;
    using b_2 = lookup_test< t_2,  key_2_t,  fallback  >::result;
    using b_3 = lookup_test< t_3,  key_2_t,  fallback  >::result;

    using B_0  =  fallback;
    using B_1  =  fallback;
    using B_2  =  value_2;
    using B_3  =  value_2;

    lt::selftest::check_expression_equality< b_0,  B_0  >();
    lt::selftest::check_expression_equality< b_1,  B_1  >();
    lt::selftest::check_expression_equality< b_2,  B_2  >();
    lt::selftest::check_expression_equality< b_3,  B_3  >();

    using c_0 = lookup_test< t_0,  key_3_t,  fallback  >::result;
    using c_1 = lookup_test< t_1,  key_3_t,  fallback  >::result;
    using c_2 = lookup_test< t_2,  key_3_t,  fallback  >::result;
    using c_3 = lookup_test< t_3,  key_3_t,  fallback  >::result;

    using C_0  =  fallback;
    using C_1  =  fallback;
    using C_2  =  fallback;
    using C_3  =  value_3;

    lt::selftest::check_expression_equality< c_0,  C_0 >();
    lt::selftest::check_expression_equality< c_1,  C_1 >();
    lt::selftest::check_expression_equality< c_2,  C_2 >();
    lt::selftest::check_expression_equality< c_3,  C_3 >();
}
