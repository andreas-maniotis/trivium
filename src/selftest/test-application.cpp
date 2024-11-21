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



#include "lt/fundamental_types/application.hpp"
#include "lt/fundamental_types/s_expr.hpp"



TEST_CASE("Combinator I = SKK")
{
    using namespace lt;

    using x_  =  lt::text<'x'>;


    //  lambda x x  ~  S K K

    using lambda_x_x  =  application<  x_  >::apply_to<  x_  >;
    using s_k_k       =  s_expr< S, K, K >;


    lt::selftest::check_expression_equality<  lambda_x_x,  s_k_k >();
}



TEST_CASE("Combinator S = lambda xyz.xz(yz)")
{
    using namespace lt;

    using x_  =  lt::text<'x'>;
    using y_  =  lt::text<'y'>;
    using z_  =  lt::text<'z'>;



    //  lambda x y z . xz (yz)    ~     S

    using xz_yz  =  s_expr<  x_,  z_,  s_expr<  y_,  z_  >  >;


   using lambda_xyz__xz_yz  =  application<  x_,  y_,  z_>::
                               apply_to< xz_yz >;


    using lambda_x__lambda_y__lambda_z__xz_yz  =

    application<  x_ > ::apply_to<
        application<  y_  >::apply_to<
            application<  z_  >::apply_to<
                xz_yz
            >
        >
    >;

    lt::selftest::check_expression_equality<  lambda_xyz__xz_yz,  lt::S  >();
    lt::selftest::check_expression_equality<  lambda_x__lambda_y__lambda_z__xz_yz,  lt::S  >();
}



TEST_CASE("Combinator B = lxyz.x(yz)")
{
   using namespace lt;

    using x_  =  lt::text<'x'>;
    using y_  =  lt::text<'y'>;
    using z_  =  lt::text<'z'>;



    using x_yz  = s_expr<  x_,  s_expr<  y_,  z_  >  >;


    //  lambda  x y z . x (y z)   ~  B    with   B :=:  S (KS) K

    using B_computed  =  application<  x_,  y_,  z_  >::apply_to<  x_yz  >;
    using B_expected  =  s_expr<  S,  s_expr< K, S >,  K  >;


    lt::selftest::check_expression_equality<  B_expected,  B_computed  >();
}
