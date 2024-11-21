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


#include "lt/fundamental_types/lut.hpp"



template<  auto  >
struct object {};



using lt::operator""_t;


using key_1_t  =  decltype( "key_1"_t );
using key_2_t  =  decltype( "key_2"_t );
using key_3_t  =  decltype( "key_3"_t );


using value_1 = int*;
using value_2 = int**;
using value_3 = int***;

using entry_1 =  lt::tuple<  key_1_t,  value_1  >;
using entry_2 =  lt::tuple<  key_2_t,  value_2  >;
using entry_3 =  lt::tuple<  key_3_t,  value_3  >;




using t_0 = lt::lut<>;


using t_1 = lt::lut< entry_1  >;


using t_2 = lt::lut<  entry_1,  entry_2  >;


using t_3 = lt::lut<  entry_1,  entry_2,  entry_3  >;

// ================================================================================================
//
// Lookup-tables (LUTs) have literal types (and can thus be the type
// of non-type template parameters).
//
// ------------------------------------------------------------------------------------------------


TEST_CASE("LUTs have literal types")
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


// ================================================================================================
//
// Test whether a key is contained in a LUT
//
// ------------------------------------------------------------------------------------------------


TEST_CASE("LUT::contains")
{
    TRIVIUM_CHECK(  ! t_0::contains_key("key_1"_t)  );
    TRIVIUM_CHECK(    t_1::contains_key("key_1"_t)  );
    TRIVIUM_CHECK(    t_2::contains_key("key_1"_t)  );
    TRIVIUM_CHECK(    t_3::contains_key("key_1"_t)  );


    TRIVIUM_CHECK(  ! t_1::contains_key("key_2"_t)  );
    TRIVIUM_CHECK(    t_2::contains_key("key_2"_t)  );
    TRIVIUM_CHECK(    t_3::contains_key("key_1"_t)  );

    TRIVIUM_CHECK(  ! t_1::contains_key("key_3"_t)  );
    TRIVIUM_CHECK(  ! t_2::contains_key("key_3"_t)  );
    TRIVIUM_CHECK(    t_3::contains_key("key_3"_t)  );
}


// ================================================================================================
//
// Addition of new entries to LUTs.
//
// Assume that the LUT has the type
//
//      LUT :=: lut<  type< Key, Value >...  >.
//
//
//  The call LUT::add_entry<  tuple< K, V >  >  yields the following result R:
//
//
//  (a)  R  :=:  lut<  type< Key, Value >...,  tuple< K, V >
//       whenever the key K is not contained in the parameter pack  Key...
//
//
//  (b)  R :=:  lut<  tuple< Key, Value'>  >...  >
//
//              with  Value' :=: Value when K and Key are different
//                    Value' :=: V     when K and Key are identical.
//
//      In short: the unique parameter P in the parameter pack  "tuple< Key,  Value >...",
//      the key of which coincides with K is replaced by the tuple P' :=: tuple< K, V >.
//      Alt the other parameters are left unchanged.
//
// ------------------------------------------------------------------------------------------------

TEST_CASE("add entries")
{
    using entry_4 =  lt::tuple<  decltype("key_4"_t),  value_1  >;


    using v_0 = t_0::add_entry< entry_4 >;
    using v_1 = t_1::add_entry< entry_4 >;
    using v_2 = t_2::add_entry< entry_4 >;


    using V_0 = lt::lut< entry_4 >;
    using V_1 = lt::lut<  entry_1,  entry_4  >;
    using V_2  = lt::lut<  entry_1,  entry_2,  entry_4  >;


    lt::selftest::check_expression_equality< v_0, V_0 >();
    lt::selftest::check_expression_equality< v_1, V_1 >();
    lt::selftest::check_expression_equality< v_2, V_2 >();


    using entry_1_ = lt::tuple< key_1_t,  int(*)()  >;


    using x_1 = t_1::add_entry<  entry_1_  >;
    using x_2 = t_2::add_entry<  entry_1_ >;
    using x_3 = t_3::add_entry<  entry_1_ >;


    using X_1 = lt::lut<  entry_1_ >;
    using X_2 = lt::lut<  entry_1_,  entry_2  >;
    using X_3 = lt::lut<  entry_1_,  entry_2,  entry_3  >;


    lt::selftest::check_expression_equality< x_1, X_1 >();
    lt::selftest::check_expression_equality< x_2, X_2 >();
    lt::selftest::check_expression_equality< x_3, X_3 >();



    using entry_2_ = lt::tuple<  key_2_t, int (**)() >;

    using y_2 = t_2::add_entry<  entry_2_ >;

    using Y_2 = lt::lut<  entry_1,  entry_2_  >;

    lt::selftest::check_expression_equality< y_2, Y_2 >();



    using entry_3_ = lt::tuple<  key_3_t,  int (***)()  >;

    using z_3 = t_3::add_entry<  entry_3_  >;

    using Z_3 = lt::lut<  entry_1,  entry_2,  entry_3_  >;

    lt::selftest::check_expression_equality< z_3, Z_3 >();
}



// ================================================================================================
//
// Lookup of symbols.
//
//
// LUT::lookup< Key >             evaluates to  Value whenever  tuple< Key, Value >
//                                whenever  tuple< key, Value > is contained in LUT.
//                                If not, then a compilation error arises
//
// LUT::lookup< Key, Fallback >   evaluates to...
//                                ... Value     when tuple< Key, Value > is contained in the LUT
//                                ... Fallback  otherwise.
//
//
// ------------------------------------------------------------------------------------------------


struct substitution_failure {};



template<  typename,  typename... >
struct lookup_test
{
    using result = substitution_failure;
};



template<  typename LUT,  typename... P  >
requires
requires(  typename LUT::template lookup< P...  >  )
{
    nullptr;
}
struct lookup_test<  LUT,  P...  >
{
    using result = typename LUT::template lookup< P... >;
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
