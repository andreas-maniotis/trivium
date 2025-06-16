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


// -----------------------------------------------------------------------------
//
// Elementary list operations:
//
// -----------------------------------------------------------------------------



TEST_CASE("Elementary list operations")
{

    using expr_0 =  lt::eval< "(list 1 2 3 4)"  >  ;


    using Expr_0 = lt::s<  lt::integer<1>
                             ,  lt::integer<2>
                             ,  lt::integer<3>
                             ,  lt::integer<4>
                             >;

    lt::selftest::check_expression_equality<  expr_0,  Expr_0  >("expr_0: ");



    using expr_1  =  lt::eval< "()" >;
    using Expr_1  =  lt::s<>;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >("expr_1: ");


    using expr_2  =  lt::eval<  "( first ( list 1 2 3 )  )" >;
    using Expr_2  =  lt::integer<1>;

    lt::selftest::check_expression_equality<  expr_2,  Expr_2  >("expr_2 ");


    using expr_3  =  lt::eval<  "( first ( list  (list 2 3) 4 ) )" >;
    using Expr_3  =  lt::s< lt::integer<2>, lt::integer<3> >;

    lt::selftest::check_expression_equality<  expr_3,  Expr_3  >("expr_3: ");



    using expr_4  =  lt::eval<  "(drop_first  (list 1 ( list 2 3)))" >;
    using Expr_4  =  lt::s<  lt::s<  lt::integer<2>,  lt::integer<3>  >  >;
    lt::selftest::check_expression_equality<  expr_4,  Expr_4  >("expr_4: ");



    using expr_5  =  lt::eval<  "(cons 1 (drop_first (list 2 3 4)))"  >;
    using Expr_5  =  lt::s<  lt::integer<1>,  lt::integer<3>,  lt::integer<4>  >;

    lt::selftest::check_expression_equality<  expr_5,  Expr_5  >("expr_5: ");



    using expr_6  =  lt::eval< "((cons 1) (list 2 3 4))" >;
    using Expr_6  =  lt::s<  lt::integer<1>,  lt::integer<2>,  lt::integer<3>,  lt::integer<4>  >;

    lt::selftest::check_expression_equality<  expr_6,  Expr_6  >("expr_6: ");



    using expr_7  =  lt::eval< "(cons -1 (drop_first (list 1)))" >;
    using Expr_7  =  lt::s<  lt::integer<-1>  >;

    lt::selftest::check_expression_equality<  expr_7,  Expr_7  >("expr_7: ");



    using expr_5r  =  lt::eval<  "(rcons 1 (drop_first (list 2 3 4)))"  >;
    using Expr_5r  =  lt::s<  lt::integer<3>,  lt::integer<4>,  lt::integer<1>  >;

    lt::selftest::check_expression_equality<  expr_5r,  Expr_5r  >("expr_5r: ");



    using expr_6r  =  lt::eval< "((rcons 1) (list 2 3 4))" >;
    using Expr_6r  =  lt::s<  lt::integer<2>,  lt::integer<3>,  lt::integer<4>,  lt::integer<1>  >;

    lt::selftest::check_expression_equality<  expr_6r,  Expr_6r  >("expr_6r: ");



    using expr_7r  =  lt::eval< "(rcons -1 (drop_first (list 1)))" >;
    using Expr_7r  =  lt::s<  lt::integer<-1>  >;

    lt::selftest::check_expression_equality<  expr_7r,  Expr_7r  >("expr_7r: ");



    using expr_8  =  lt::eval<" (drop_first (list 1))" >;
    using Expr_8  =  lt::s<>;

    lt::selftest::check_expression_equality<  expr_8,  Expr_8  >("expr_8: ");



    using expr_9  =  lt::eval<  "(list (list 1 2 3))"  >;
    using Expr_9  =  lt::s<  lt::s<  lt::integer<1>
                                            ,  lt::integer<2>
                                            ,  lt::integer<3>
                                            >
                               >;

    lt::selftest::check_expression_equality<  expr_9,  Expr_9  >("expr_9: ");


    using expr_10 = lt::eval< "(count '(1 2 3))" >;
    using Expr_10 = lt::value<3>;

    lt::selftest::check_expression_equality<  expr_10,  Expr_10  >("expr_10: ");


    using expr_11 = lt::eval<"(count ())">;
    using Expr_11 = lt::value<0>;


    lt::selftest::check_expression_equality<  expr_11,  Expr_11  >("expr_11: ");


    using expr_12 = lt::eval<"(eval_success (count 3))">;
    using Expr_12 = lt::value<false>;

    lt::selftest::check_expression_equality< expr_12,  Expr_12  >("expr_12: ");


}



// -----------------------------------------------------------------------------
//
// Quoted expressions:
//
// -----------------------------------------------------------------------------


TEST_CASE("Quoted expressions")
{
    using expr_1  =  lt::eval<  "(quote  ( 1 2 3 ) )"  >;
    using Expr_1  =  lt::s<  lt::integer<1>,  lt::integer<2>,  lt::integer<3>  >;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >("expr_1: ");



    using expr_2  =  lt::eval<  "(quote (quote( quote 222)))"  >;

    using Expr_2  =  lt::quoted<  lt::quoted<  lt::integer<222>  >  >;

    lt::selftest::check_expression_equality<  expr_2,  Expr_2  >("expr_2: ");



    using expr_3  =  lt::eval<  "(quote ( first (list 1 2 3) ))"  >;
    using Expr_3  =  lt::s<  lt::text< 'f', 'i', 'r', 's', 't' >
                               ,  lt::s<  lt::list<3>
                                            ,  lt::integer<1>
                                            ,  lt::integer<2>
                                            ,  lt::integer<3>
                                            >
                               >;

    lt::selftest::check_expression_equality<  expr_3,  Expr_3  >("expr_3: ");



    using expr_4  =  lt::eval<  "(eval (quote ( first ( list 1 2 3))))"  >;
    using Expr_4  =  lt::integer<1>;

    lt::selftest::check_expression_equality<  expr_4,  Expr_4  >("expr_4: ");



    using expr_5  =  lt::eval<  "(eval (eval (quote ( first ( list 1 2 3)))))"  >;
    using Expr_5  =  lt::integer<1>;

    lt::selftest::check_expression_equality<  expr_5,  Expr_5  >("expr_5: ");
}



// -----------------------------------------------------------------------------
//
// apply:
//
// -----------------------------------------------------------------------------


TEST_CASE("apply")
{
    using expr_1  =  lt::eval<  "(apply 'first (list (list (quote a) (quote b)) ))"  >;
    using Expr_1  =  lt::text<'a'>;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >("expr_1: ");



    using expr_2  =  lt::eval<  "(apply 'first  (list (list 1 2 3 ) ) )"  >;
    using Expr_2  =  lt::integer<1>;

    lt::selftest::check_expression_equality<  expr_2,  Expr_2  >("expr_2: ");
}



// -----------------------------------------------------------------------------
//
// eval:
//
// -----------------------------------------------------------------------------


TEST_CASE("eval")
{
    using expr_1  =  lt::eval<
    "(( eval '([x]( if ( eq 1 x ) (first ( list 'first 'drop_first ) ) 'drop_first) 1)) '(11 22 33))" >;

    using Expr_1 = lt::integer<11>;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1 >("expr_1: ");

}



// -----------------------------------------------------------------------------
//
// Lambda-Expressions
//
// -----------------------------------------------------------------------------



TEST_CASE("Lambda expressions")
{
    using expr_1  =  lt::eval<  "([x y] x first 4 (list -1 -2))"  >;
    using Expr_1 =  lt::integer<-1>;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >("expr_1: ");



    using expr_2  =  lt::eval< "([x] x [x] x)"  >;
    using Expr_2  =  lt::eval<  "[x] x"  >;


    lt::selftest::check_expression_equality<  expr_2,  Expr_2  >("expr_2: ");





    using expr_4  =  lt::eval<  "(([x y] x  2) 3)"  >;
    using Expr_4  =  lt::s_expr<"2">;


    lt::selftest::check_expression_equality<  expr_4,  Expr_4  >("expr_4: ");



    using expr_5  =
    lt::eval<  "(  [x y z] (x (y z)) first drop_first (list 1 2 3)) "  >;

    using Expr_5  =  lt::s_expr<"2">;


    lt::selftest::check_expression_equality<  expr_5,  Expr_5  >("expr_5: ");



    using expr_6  =  lt::eval<  "( [x y] x  first 2 ( list -2 -3) )"  >;
    using Expr_6  =  lt::s_expr<"-2">;

    lt::selftest::check_expression_equality<  expr_6,  Expr_6  >("expr_6: ");



    using expr_7  =
    lt::eval<
    R"( (  [x y z] ( x (y z))
            first
            first
            (list (list 'a ) (list 'b ) )
         )  )"  >;



    using Expr_7   =  lt::text<'a'>;

    lt::selftest::check_expression_equality<  expr_7,  Expr_7  >("expr_7: ");



    using expr_8a  =  lt::eval< "(  [ x y z w ] w  1  2  3  4 )"  >;
    using expr_8b  =  lt::eval< "( ([ x y z w ] w    1) 2  3  4 )"  >;
    using expr_8c  =  lt::eval< "( ([ x y z w ] w    1  2) 3  4 )"  >;
    using expr_8d  =  lt::eval< "( ([ x y z w ] w    1  2  3)  4 )" >;

    using Expr_8   = lt::integer< 4 >;;

    lt::selftest::check_expression_equality<  expr_8a,  Expr_8  >("expr_8a: ");
    lt::selftest::check_expression_equality<  expr_8b,  Expr_8  >("expr_8b: ");
    lt::selftest::check_expression_equality<  expr_8c,  Expr_8  >("expr_8c: ");
    lt::selftest::check_expression_equality<  expr_8d,  Expr_8  >("expr_8d: ");



    using expr_9  =
    lt::eval< "(if ([x] true  (quote ignored_input) ) 1 0 )" >;

    using Expr_9  =  lt::integer<1>;

    lt::selftest::check_expression_equality<  expr_9,  Expr_9  >("expr_9: ");


    using expr_10  =
    lt::eval< "(if ([x] false  (quote ignored_input) ) 1 0 )" >;


    using Expr_10  =  lt::integer<0>;


    lt::selftest::check_expression_equality<  expr_10,  Expr_10  >("expr_10: ");
}



TEST_CASE("arithmetics")
{
    using expr_1  =  lt::eval< "(+ 3 ( + 4 5) )" >;
    using Expr_1  =  lt::integer<12>;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >();


    using expr_2  =  lt::eval< "( ( eq 3) true )" >;
    using Expr_2  =  lt::value<false>;

    lt::selftest::check_expression_equality<  expr_2,  Expr_2  >();



    using expr_3  =  lt::eval< "(- 5 3)"  >;
    using Expr_3  =  lt::eval< "2" >;

    lt::selftest::check_expression_equality<  expr_3,  Expr_3  >();


    using expr_4  =  lt::eval< "(* 5 3)"  >;
    using Expr_4  =  lt::eval< "15" >;

    lt::selftest::check_expression_equality<  expr_4,  Expr_4  >();

    using expr_5  =  lt::eval< "(/ 5 3)"  >;
    using Expr_5  =  lt::eval< "1" >;

    lt::selftest::check_expression_equality<  expr_5,  Expr_5  >();

    using expr_6  =  lt::eval< "(% 5 3)"  >;
    using Expr_6  =  lt::eval< "2" >;

    lt::selftest::check_expression_equality<  expr_6,  Expr_6  >();
}



TEST_CASE("Lazy evaluation and evaluability test")
{
    // ignore undefined_string:
    using expr_1 = lt::eval<  "(if true 1 undefined_string)"  >;
    using Expr_1 = lt::eval< "1" >;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1 >();


    using expr_2 = lt::eval< "(eval_success 3)" >;
    using Expr_2 = lt::value< true >;

    lt::selftest::check_expression_equality<  expr_2,  Expr_2 >();


    using expr_3 = lt::eval< "(eval_success (humbug blabla (asdf)  ) )" >;
    using Expr_3 = lt::value<false>;

    lt::selftest::check_expression_equality<  expr_3,  Expr_3  >();


    using expr_4 = lt::eval< "(if_possible (list 1 2 3) humbug_not_evaluated )" >;
    using Expr_4 = lt::eval< "( list 1 2 3 )" >;

    lt::selftest::check_expression_equality<  expr_4,  Expr_4  >();


    using expr_5 = lt::eval<"(if_possible humbug (list 1 2 3))">;
    using Expr_5 = lt::eval<"(list 1 2 3)">;

    lt::selftest::check_expression_equality<  expr_5,  Expr_5  >();


    using expr_6 = lt::eval<"(eval_success (if_possible humbug  also_humbug ))">;
    using Expr_6 = lt::value<false>;

    lt::selftest::check_expression_equality<  expr_6,  Expr_6  >();
}



TEST_CASE("factorial map")
{
    using expr_1  =

    lt::eval<
    R"(
        ( def  'f [n]                            ;  definition of the factorial function
                (   if ( == n  0 )               ;  if n == 0
                    1                            ;  then return 1
                    ( *  n  ( f ( - n 1 ) ) )    ;  else return n * f (n -1 )
                                                 ;  (written in Polish notation).
                )
         (f 6)                                   ;  call f for the input 6
        )                                        ;  returns integer< 6! >
    )" >;


    using Expr_1 =  lt::integer<  6*5*4*3*2*1  >;

    lt::selftest::check_expression_equality<  expr_1, Expr_1  >("expr 2: ");
}



TEST_CASE("Nested definitions")
{
    using expr_1 = lt::eval<
    R"(
        {
            ( def 'g [x y] ( * x y ) )    ; g(x y ) := x * y

            ( def 'c 5 )                  ; c       := 5

            ( def 'h [z] (g c z ) )       ;  h(z)   := g (c, z) = 5 * z


            (h 3)                        ;  return h(3) = 5 * 3
        }
    )" >;


    using Expr_1 = lt::integer<15>;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >("expr 1: ");
}



TEST_CASE("Stein's gcd")
{
    using expr_1 =

    lt::eval<

    R"({
            (   def 'even   [x]( == 0 (% x 2))    )
            (   def 'odd    [x]( == 1 (% x 2))    )
            (   def 'abs    [x]( if (> x 0) x (- 0 x) )  )
            (   def 'gcd [ x y ]
                {
                    ( if  (== x y ) x )
                    ( if  (and (even x) (even y) )  (* 2 (gcd   (/ x 2)   (/ y 2) )  ))
                    ( if  (and (even x)  (odd y) )  (gcd   (/ x 2)           y)  )
                    ( if  (and  (odd x) (even y) )  (gcd          x    (/ y 2))  )
                    (  gcd y (abs ( - x y )) )
                }
            )
            (gcd  18 24)
        }
    )" >;


    using Expr_1  =  lt::integer<6>;
    lt::selftest::check_expression_equality< expr_1,  Expr_1>("Expr 1:");
}



TEST_CASE("evaluation errors")
{
    std::cout << std::endl;
    std::cout << "---------------------------------------------------------------------------  " << std::endl;
    std::cout << "The following test produces three messages of the form  (show_error ... ):   " << std::endl;
    std::cout << "---------------------------------------------------------------------------  " << std::endl;
    std::cout << std::endl;

    using expr_1 = lt::eval<"(show_error ((first abc) 2)  )">;
    lt::selftest::print_type<expr_1>("(show_error ((first abc) 2)):");


    using expr_2 = lt::eval<"(show_error (def f [x](abc x) 3 ))">;
    lt::selftest::print_type<expr_2>("(show_error (def f [x](abc x) 3 ))   :");


    using expr_3 = lt::eval<"(show_error ( raise_error 'custom_error_message ))" >;
    lt::selftest::print_type<expr_3>("(show_error ( raise_error 'custom_error_message )): ");



    std::cout << std::endl;
    std::cout << "---------------------------------------------------------------------------  " << std::endl;
    std::cout << "(show_error ... )-messages finished.                                         " << std::endl;
    std::cout << "---------------------------------------------------------------------------  " << std::endl;
    std::cout << std::endl;


    using expr_4 = lt::eval<"(eval_success ( raise_error 'a))" >;
    using Expr_4 = lt::value<false>;

    lt::selftest::check_expression_equality<  expr_4,  Expr_4  >();

}



TEST_CASE("some nested definitions")
{
    using expr0 = lt::eval< "((def   'x  [y](* 2 y))  x { (def 'x 4) x }  )  "   >;
    using Expr0 = lt::value<8>;

    lt::selftest::check_expression_equality<  expr0,  Expr0  >();


    // A recursive function definition, in which the name of the function is given
    // indirectly as the result of expression evaluations. (This feature is very important
    // for writing Trivium Lisp metaprograms (metaprograms of the Trivium Lisp language, not of C++).
    //
    using expr_1  =  lt::eval<
R"(
    (   def     (first '(g f) )  [x]

        (   if  (eq 0 x)
                1
                ( * x  (    (if false f g)  (- x 1)  ) )
        )
        (g 5)  ; expected result 5! = 120
    )
)" >;

    using Expr_1 = lt::value<120>;


    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >();




    using expr_2 = lt::eval<  "( def 'y 2 (list y ( def 'x 3 x) ))"  >;
    using Expr_2 = lt::eval< "  '( 2 3 )  "  >;


    lt::selftest::check_expression_equality< expr_2, Expr_2  >("Expr 2: ");
}




#ifndef __clang__   // non-templates arguments of type double are not supported in Clang 16
#ifndef _MSC_VER    // msvc does not like this test.

#include <cmath>

using t = lt::value<  std::exp(0.5)  >;


template<  typename  >
struct std_exp_;



template< double x  >
struct std_exp_<  lt::value<x>  >
:   lt::type_hull<  lt::value<  std::exp(x) >  >
{ };



template<  typename  X  >
using std_exp  =  typename std_exp_<X>::type;



TEST_CASE("exp approximation")
{
    using  exp_approximation_test_success =
    lt::metaprogram<

     R"( [std::exp x ε ]
         {
             (   def 'abs  [x] ( if ( >=  x  0 ) x  (- 0 x) )    )

             (   def 'exp
                     [x] (+  1  (* x (+ 1  (* (  / x  2 ) (+ 1 ( / (* 5 x ) 16 ))))))  )

             (  > ε  ( / (- ( exp x )  (std::exp x)) (std::exp x) ) )
        }
      )"

    , lt::combinator<  1,  std_exp  >
    , lt::value< 2.3 >
    , lt::value< 0.05 >  // error bound 5%
    >;


    lt::selftest::check_expression_equality<  exp_approximation_test_success
                                           ,  lt::value<true>
                                           >();

}

#endif
#endif
