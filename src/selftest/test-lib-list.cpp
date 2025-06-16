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



#include "lt/selftest/selftest.hpp"

#include "lt/eval.hpp"
#include "lt/lib/list.hpp"


// -----------------------------------------------------------------------------
//
// Elementary list operations:
//
// -----------------------------------------------------------------------------


using list_lib =
lt::eval<
R"(
{
    ( def    'last

              [xs]
              (    if    ( eq  ( drop_first xs )  ()  )

                         ( first xs )

                         ( last ( drop_first xs ))
              )
    )


    ( def    'drop_last

              [xs]
              (    if    ( eq ( drop_first xs ) () )

                         ()

                         ( cons ( first xs ) ( drop_last ( drop_first xs ) ) )
              )
    )


    ( def    'reverse

             [xs]
             (    if    ( eq xs () )

                        ()

                        ( cons    ( last xs )    ( reverse ( drop_last xs ) )    )
             )
    )

   export

}
)" >;



TEST_CASE("dummy")
{

    using t1 = lt::lib::list::eval<"(reverse '(1 2 3 4))">;
    lt::selftest::print_type<t1>("reverse: (expect 4 3 2 1): ");


    using t1_b = lt::metaprogram<"[lib::list] ( import lib::list (reverse '( 1 2 3 4) )) "
                                ,  lt::lib::list
                                >;

    lt::selftest::print_type<t1_b>("lib bound from outside! (expecting (4 3 2 1): ");


    // The simplest way to program with types:
    // lt::metaprogram< program, Xs... >
    //
    using s = list_lib::metaprogram<  "[x y z]( reverse (list x y z))",  int,  int*,  int**  >;
    lt::selftest::print_type<s>("reverse 2: ");  // returns lt::s< int**, int*, int >



    using s2 = list_lib::metaprogram<"[xs](apply reverse xs)", lt::s< lt::s< int, int*, int**, int*** > > >;
    lt::selftest::print_type<s2>("reverse s2: "); // returns lt::s< int***, int**, int*, int >


    using s3 = list_lib::eval<"(apply reverse (list '(1 2 3)) )">;
    lt::selftest::print_type<s3>("reverse s3: "); // returns lt::s< lt::value<3>, lt::value<2>, lt::value<1> >


    using s4 = list_lib::eval<"(+ ( + 3 4))">;
    lt::selftest::print_type<s4>("s4  :::::::::::::::: ");
}


