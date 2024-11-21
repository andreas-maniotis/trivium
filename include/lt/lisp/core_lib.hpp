/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "interpreter.hpp"
#include "linker.hpp"


namespace lt::lisp
{
    template< text_type auto const x  >
    inline constexpr auto core  =  s_expr<>{};



    template<>
    constexpr auto core<"++"_t>  =
    "(  def '++ [x y](  if  ( eq x () ) y (  cons (first x) (++ (drop_first x) y)  ) ) )"_s;



    template<>
    constexpr auto core<"drop_last"_t>  =
    "(def  'drop_last  [x](  if  (eq  (  drop_first x)  ())  ()  (cons  (first x)  (  drop_last (drop_first x)) )))"_s;



    template<>
    constexpr auto core<"last"_t>  =
    R"(( def  'last  [x](  if  (eq  (drop_first x)  ())  x  (last  (drop_first  x)))))"_s;



    template<>
    inline constexpr auto dependencies<"reverse"_t>  =
    set_dependencies( core<"last"_t>,  core<"drop_last"_t>);



    template<>
    constexpr auto core<"reverse"_t> =
    "(  def  'reverse  [x](  if  (eq  x  ())  ()  (cons  (last x)  (reverse  (drop_last x)))))"_s;



    template<>
    inline constexpr auto core<"length"_t>  =
    "(def 'length [x](  if  ( eq x () ) 0 ( + 1  (length (drop_first x))  ) ) )"_s;



    template<>
    inline constexpr auto core<"map"_t>  =
    "( def 'map [f][xs] (if (eq xs ())  ()  (cons (f (first xs)) (map f (drop_first xs)  ) )   ) )"_s;



    template<>
    inline constexpr auto core<"for_all"_t>  =
    "(def 'forall  [xs][pred] (if (eq xs ())  true (and (pred (first xs))  (for_all (drop_first xs) pred  )   )  ) )"_s;



    template<>
    inline constexpr auto core<"for_one"_t>  =
    "(def 'for_one  [xs][pred] (if (eq xs ()) false (or (pred (first xs))  (for_one (drop_first xs) pred  )   )  ) )"_s;



    template<>
    inline constexpr auto core<"fold::left"_t>  =
    "(def 'fold::left [f][z][xs](  if  (eq xs ())  z  ( fold::left f (f z (first xs)) (drop_first xs)  )))"_s;



    template<>
    inline constexpr auto core<"fold::left_1"_t>  =
   "( def  'fold::left_1  [f][xs]  (  if ( eq (drop_first xs) () )  (first xs)  (  f (first xs) ( fold::left_1 f (drop_first xs) ))))"_s;



    template<>
    inline constexpr auto core<"fold::right"_t>  =
    "(  def 'fold::right  [f][z][xs]  ( if  (eq xs ())  z  (f  (first xs) ( fold::right z (drop_first xs) ))    )  )"_s;



    template<>
    inline constexpr auto core<"fold::right_1"_t>  =
    "(  def  'fold::right  [f][xs]  ( if (eq (drop_first xs) () )  (first xs)  (f  (first xs )  (fold::right_1 f (drop_first xs)))))"_s;
}
