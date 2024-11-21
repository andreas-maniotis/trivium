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

#include "lt/lisp/eval.hpp"



// -----------------------------------------------------------------------------
//
// Elementary list operations:
//
// -----------------------------------------------------------------------------



TEST_CASE("Elementary list operations")
{

    using lt::operator""_s;


    using expr_0 =  lt::lisp::eval< "(list 1 2 3 4)"_s  >  ;

    using Expr_0 = lt::s_expr<  lt::integer<1>
                             ,  lt::integer<2>
                             ,  lt::integer<3>
                             ,  lt::integer<4>
                             >;

    lt::selftest::check_expression_equality<  expr_0,  Expr_0  >("expr_0: ");



    using expr_1  =  lt::lisp::eval< "()"_s >;
    using Expr_1  =  lt::s_expr<>;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >("expr_1: ");



    using expr_2  =  lt::lisp::eval<  "( first ( list 1 2 3 )  )"_s >;
    using Expr_2  =  lt::integer<1>;

    lt::selftest::check_expression_equality<  expr_2,  Expr_2  >("expr_2 ");



    using expr_3  =  lt::lisp::eval<  "( first ( list  (list 2 3) 4 ) )"_s >;
    using Expr_3  =  lt::s_expr< lt::integer<2>, lt::integer<3> >;

    lt::selftest::check_expression_equality<  expr_3,  Expr_3  >("expr_3: ");



    using expr_4  =  lt::lisp::eval<  "(drop_first  (list 1 ( list 2 3)))"_s >;
    using Expr_4  =  lt::s_expr<  lt::s_expr<  lt::integer<2>,  lt::integer<3>  >  >;
    lt::selftest::check_expression_equality<  expr_4,  Expr_4  >("expr_4: ");



    using expr_5  =  lt::lisp::eval<  "(cons 1 (drop_first (list 2 3 4)))"_s >;
    using Expr_5  =  lt::s_expr<  lt::integer<1>,  lt::integer<3>,  lt::integer<4>  >;

    lt::selftest::check_expression_equality<  expr_5,  Expr_5  >("expr_5: ");



    using expr_6  =  lt::lisp::eval< "((cons 1) (list 2 3 4))"_s >;
    using Expr_6  =  lt::s_expr<  lt::integer<1>,  lt::integer<2>,  lt::integer<3>,  lt::integer<4>  >;

    lt::selftest::check_expression_equality<  expr_6,  Expr_6  >("expr_6: ");



    using expr_7  =  lt::lisp::eval< "(cons 1 (drop_first (list 1)))"_s >;
    using Expr_7  =  lt::s_expr<  lt::integer<1>  >;

    lt::selftest::check_expression_equality<  expr_7,  Expr_7  >("expr_7: ");



    using expr_8  =  lt::lisp::eval<" (drop_first (list 1))"_s >;
    using Expr_8  =  lt::s_expr<>;

    lt::selftest::check_expression_equality<  expr_8,  Expr_8  >("expr_8: ");



    using expr_9  =  lt::lisp::eval<  "(list (list 1 2 3))"_s  >;
    using Expr_9  =  lt::s_expr<  lt::s_expr<  lt::integer<1>
                                            ,  lt::integer<2>
                                            ,  lt::integer<3>
                                            >
                               >;

    lt::selftest::check_expression_equality<  expr_9,  Expr_9  >("expr_9: ");

}



// -----------------------------------------------------------------------------
//
// Quoted expressions:
//
// -----------------------------------------------------------------------------


TEST_CASE("Quoted expressions")
{
    using lt::operator""_s;


    using expr_1  =  lt::lisp::eval<  "(quote  ( 1 2 3 ) )"_s  >;
    using Expr_1  =  lt::s_expr<  lt::integer<1>,  lt::integer<2>,  lt::integer<3>  >;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >("expr_1: ");



    using expr_2  =  lt::lisp::eval<  "(quote (quote( quote 222)))"_s  >;

    using Expr_2  =  lt::quoted<  lt::quoted<  lt::integer<222>  >  >;

    lt::selftest::check_expression_equality<  expr_2,  Expr_2  >("expr_2: ");



    using expr_3  =  lt::lisp::eval<  "(quote ( first (list 1 2 3) ))"_s  >;
    using Expr_3  =  lt::s_expr<  lt::text< 'f', 'i', 'r', 's', 't' >
                               ,  lt::s_expr<  lt::list<3>
                                            ,  lt::integer<1>
                                            ,  lt::integer<2>
                                            ,  lt::integer<3>
                                            >
                               >;

    lt::selftest::check_expression_equality<  expr_3,  Expr_3  >("expr_3: ");



    using expr_4  =  lt::lisp::eval<  "(eval (quote ( first ( list 1 2 3))))"_s  >;
    using Expr_4  =  lt::integer<1>;

    lt::selftest::check_expression_equality<  expr_4,  Expr_4  >("expr_4: ");



    using expr_5  =  lt::lisp::eval<  "(eval (eval (quote ( first ( list 1 2 3)))))"_s  >;
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
    using lt::operator""_s;



    using expr_1  =  lt::lisp::eval<  "(apply 'first (list (list (quote a) (quote b)) ))"_s  >;
    using Expr_1  =  lt::text<'a'>;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >("expr_1: ");



    using expr_2  =  lt::lisp::eval<  "(apply 'first  (list (list 1 2 3 ) ) )"_s  >;
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

    using lt::operator""_s;

    using expr_1  =  lt::lisp::eval<
    "(( eval '([x]( if ( == 1 x ) (first ( list 'first 'drop_first ) ) 'drop_first) 1)) '(11 22 33))"_s >;

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
    using lt::operator""_s;



    using expr_1  =  lt::lisp::eval<  "([x y] x first 4 (list -1 -2))"_s  >;
    using Expr_1 =  lt::integer<-1>;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >("expr_1: ");



    using expr_2  =  lt::lisp::eval< "([x] x [x] x)"_s  >;
    using Expr_2  =  lt::lisp::eval<  "[x] x"_s  >;


    lt::selftest::check_expression_equality<  expr_2,  Expr_2  >("expr_2: ");





    using expr_4  =  lt::lisp::eval<  "(([x y] x  2) 3)"_s  >;
    using Expr_4  =  decltype(  "2"_s  );


    lt::selftest::check_expression_equality<  expr_4,  Expr_4  >("expr_4: ");



    using expr_5  =
    lt::lisp::eval<  "(  [x y z] (x (y z)) first drop_first (list 1 2 3)) "_s  >;

    using Expr_5  =  decltype(  "2"_s  );


    lt::selftest::check_expression_equality<  expr_5,  Expr_5  >("expr_5: ");



    using expr_6  =  lt::lisp::eval<  "( [x y] x  first 2 ( list -2 -3) )"_s  >;
    using Expr_6  =  decltype( "-2"_s  );

    lt::selftest::check_expression_equality<  expr_6,  Expr_6  >("expr_6: ");



    using expr_7  =
    lt::lisp::eval<
    R"( (  [x y z] ( x (y z))
            first
            first
            (list (list 'a ) (list 'b ) )
         )  )"_s  >;



    using Expr_7   =  lt::text<'a'>;

    lt::selftest::check_expression_equality<  expr_7,  Expr_7  >("expr_7: ");


    // BUG: the following expressions cannot be evaluated:

    using expr_8a  =  lt::lisp::eval< "(  [ x y z w ] w  1  2  3  4 )"_s  >;
    using expr_8b  =  lt::lisp::eval< "( ([ x y z w ] w    1) 2  3  4 )"_s  >;
    using expr_8c  =  lt::lisp::eval< "( ([ x y z w ] w    1  2) 3  4 )"_s  >;
    using expr_8d  =  lt::lisp::eval< "( ([ x y z w ] w    1  2  3)  4 )"_s >;

    using Expr_8   = lt::integer< 4 >;;

    lt::selftest::check_expression_equality<  expr_8a,  Expr_8  >("expr_8a: ");
    lt::selftest::check_expression_equality<  expr_8b,  Expr_8  >("expr_8b: ");
    lt::selftest::check_expression_equality<  expr_8c,  Expr_8  >("expr_8c: ");
    lt::selftest::check_expression_equality<  expr_8d,  Expr_8  >("expr_8d: ");



    using expr_9  =
    lt::lisp::eval< "(if ([x] true  (quote ignored_input) ) 1 0 )"_s >;

    using Expr_9  =  lt::integer<1>;

    lt::selftest::check_expression_equality<  expr_9,  Expr_9  >("expr_9: ");


    using expr_10  =
    lt::lisp::eval< "(if ([x] false  (quote ignored_input) ) 1 0 )"_s>;


    using Expr_10  =  lt::integer<0>;


    lt::selftest::check_expression_equality<  expr_10,  Expr_10  >("expr_10: ");

}



TEST_CASE("arithmetics")
{
    using lt::operator""_s;

    using expr_1  =  lt::lisp::eval< "(+ 3 ( + 4 5) )"_s  >;
    using Expr_1  =  lt::integer<12>;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >();


    using expr_2  =  lt::lisp::eval< "( ( eq 3) true )"_s >;
    using Expr_2  =  decltype("false"_s);

    lt::selftest::check_expression_equality<  expr_2,  Expr_2  >();



    using expr_3  =  lt::lisp::eval< "(- 5 3)"_s  >;
    using Expr_3  =  lt::lisp::eval< "2"_s >;

    lt::selftest::check_expression_equality<  expr_3,  Expr_3  >();


    using expr_4  =  lt::lisp::eval< "(* 5 3)"_s  >;
    using Expr_4  =  lt::lisp::eval< "15"_s >;

    lt::selftest::check_expression_equality<  expr_4,  Expr_4  >();

    using expr_5  =  lt::lisp::eval< "(/ 5 3)"_s  >;
    using Expr_5  =  lt::lisp::eval< "1"_s >;

    lt::selftest::check_expression_equality<  expr_5,  Expr_5  >();

    using expr_6  =  lt::lisp::eval< "(% 5 3)"_s  >;
    using Expr_6  =  lt::lisp::eval< "2"_s >;

    lt::selftest::check_expression_equality<  expr_6,  Expr_6  >();
}



TEST_CASE("Lazy evaluation")
{
    using lt::operator""_s;

    // ignore undefined_string:
    using expr_1 = lt::lisp::eval<"(if true 1 undefined_string)"_s>;
    using Expr_1 = lt::value_type<"1"_s>;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1 >();

    // Compile-time error:  undefined_sring has no value:
    //    using expr_1 = lt::lisp::eval<"(if false 1 undefined_string)"_s>;
}



TEST_CASE("factorial map")
{
    using lt::operator""_s;


    using expr_1  =

    lt::lisp::eval<
    R"(
        ( def  f [n]                             ;  definition of the factorial function
                (   if ( == n  0 )               ;  if n == 0
                    1                            ;  then return 1
                    ( *  n  ( f ( - n 1 ) ) )    ;  else return n * f (n -1 )
                                                 ;  (written in Polish notation).
                )
         (f 6)                                   ;  call f for the input 6
        )                                        ;  returns integer< 6! >
    )"_s >;


    using Expr_1 =  lt::integer<  6*5*4*3*2*1  >;

    lt::selftest::check_expression_equality<  expr_1, Expr_1  >("expr 2: ");

}


TEST_CASE("Nested definitions")
{
    using lt::operator""_s;


    using expr_1 = lt::lisp::eval<
    R"(
        {
            ( def g [x y] ( * x y ) )    ; g(x y ) := x * y

            ( def c 5 )                  ; c       := 5

            ( def h [z] (g c z ) )       ;  h(z)   := g (c, z) = 5 * z


            (h 3)                        ;  return h(3) = 5 * 3
        }
    )"_s >;


    using Expr_1 = lt::integer<15>;

    lt::selftest::check_expression_equality<  expr_1,  Expr_1  >("expr 1: ");
}



TEST_CASE("Stein's gcd")
{
    using lt::operator""_s;


    using expr_1 =

    lt::lisp::eval<

    R"({
            (   def even   [x]( == 0 (% x 2))    )
            (   def odd    [x]( == 1 (% x 2))    )
            (   def abs    [x]( if (> x 0) x (- 0 x) )  )
            (   def gcd [ x y ]
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
    )"_s>;


    using Expr_1  =  lt::integer<6>;
    lt::selftest::check_expression_equality< expr_1,  Expr_1>("Expr 1:");
}


TEST_CASE("dummy")
{
    using lt::operator""_s;


    using expr0 = lt::lisp::eval<"( def    x    [y](* 2 y)    (x { (def x 4) x})  )"_s>;

    lt::selftest::print_type<expr0>("expr_0: ");


    using expr_1  =  lt::lisp::eval<
R"(
(( if false (def f) (def g)  )  [x](   if (eq 0 x)   1    (*  x  ( (if false f g)  (- x 1)  )   ) ) (g 3) )
)"_s>;

    lt::selftest::print_type< expr_1 >();


    using expr_2 = lt::lisp::eval<"( def y 2 ( def x 3 x) )"_s>;

    using expr_3 = lt::value_type<
R"(
(   def f [x] ((def f) 3 f) )
)"_s>;



    lt::selftest::print_type< expr_3 >("expr_3: ");

    using expr_4 =  lt::lisp::eval<"( requires (eq 4 4 ) 4 )"_s>;
    lt::selftest::print_type< expr_4 >("expr_4:");


    using expr_5 = lt::lisp::eval<
R"(
(def f [x] ( if (eq x 0) 1 (*  x (f (- x 1)))) (f { (def f [x] (* 3 x)) (f 2) }))  ;  result = 6!
)"_s>;

    lt::selftest::print_type< expr_5 >("expr 5, expected 6! = 720:  ");
}


/*

TEST_CASE("value lookup")
{
    using lt::operator""_s;
    using enriched_interpreter =
    lt::lisp::interpreter<  lt::value_type<"(def 'x 3)"_s>
                         ,  lt::value_type<"(def 'f [x](* 2 x))"_s>
                         >;

    using expr_1  = enriched_interpreter::eval<"( f x )"_s>;
    using Expr_1  = lt::value<6>;

    lt::selftest::check_expression_equality< expr_1,  Expr_1 >("Expr 1:");
}



#ifndef __clang__   // non-templates arguments of type double are not supported in Clang 16

#include <cmath>


template<  auto const x  >
constexpr auto get_value_(  lt::value<x> ) { return x; };



 // Example of a computation with floatint point values:

TEST_CASE("exp function")
{
    using lt::operator""_s;


    // A formula for the approximation of exp(x):
    constexpr auto  exp_prog =
    R"( (def 'exp [x] (+  1  (* x (+ 1  (* (  / x  2 ) (+ 1 ( / (* 5 x ) 16 )))))) ) )"_s;


    //  Define x as the double value 0.5.
    //  Define exp as the lisp program that computes exp.
    //
    using enriched_interpreter =
    lt::lisp::interpreter<  lt::lisp::bind_value<   "x"_s,  0.5 >
                         ,  lt::value_type<exp_prog>
                         >;

    //  Call (exp x) and store the result in expr_1:
    using expr_1 =  enriched_interpreter::eval<"(exp x)"_s>;


    // Compute std::expr(0.5)
    constexpr double z = std::exp(0.5);


    // Numeric error:
    constexpr double error = (z - get_value_(expr_1{})) / z;


    // Check that hte error between the approximation of exp(0.5) computed by lisp and
    // with respect to std::exp(0.5) is below 3%:
    TRIVIUM_CHECK(  error <= .03  );

}

#endif
*/
