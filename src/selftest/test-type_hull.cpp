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


#include "lt/fundamental_types/type_hull.hpp"



template<  unsigned n >
requires(  n==1  || n == 2 )
using type_hull_test_configurations  =

lt::selftest::configuration_space<  n
                                 ,  void
                                 ,  int
                                 ,  int (double) const&
                                 >::list;




// ================================================================================================
//
// Check for type_hull<T>::type  :=:  T
//
// ------------------------------------------------------------------------------------------------

TEST_CASE_TEMPLATE_DEFINE( "type_hull<T>::type :=:  T",  T,  type_hull_type_test  )
{
    using hull_type  = typename lt::type_hull<  T  >::type;

    TRIVIUM_CHECK(  std::is_same_v<  hull_type,  T  >  );
}

TEST_CASE_TEMPLATE_APPLY(  type_hull_type_test,  type_hull_test_configurations<1>  );



// ================================================================================================
//
// Comparison operator  test
//
// ------------------------------------------------------------------------------------------------


TEST_CASE_TEMPLATE_DEFINE( "type hull comparisons",  T,  type_hull_cmp_test  )
{
    using lhs  =  lt::type_hull<  typename T::template at<0>  >;
    using rhs  =  lt::type_hull<  typename T::template at<1>  >;

    constexpr bool expected_for_eq  =  std::is_same_v<  lhs,  rhs  >;
    constexpr bool expected_for_neq  = ! expected_for_eq;

    constexpr bool eq_result  = ( lhs{} == rhs{} );
    constexpr bool neq_result = ( lhs{} != rhs{} );


    TRIVIUM_CHECK(  eq_result  == expected_for_eq   );
    TRIVIUM_CHECK(  neq_result == expected_for_neq  );
}


TEST_CASE_TEMPLATE_APPLY(  type_hull_cmp_test,  type_hull_test_configurations<2>  );
