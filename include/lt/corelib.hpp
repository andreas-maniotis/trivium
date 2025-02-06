/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "interpreter.hpp"



namespace lt
{
    template< lt::text<>::literal >
    inline constexpr auto lib = s_expr<"()">{};



    template<>
    inline constexpr auto lib<"++"> =  eval< "( def '++ [x y](  if  ( eq x () ) y (  cons (first x) (++ (drop_first x) y)  ) ) )" >{};








    inline constexpr auto corelib =
    with<  eval< "( def '++ [x y](  if  ( eq x () ) y (  cons (first x) (++ (drop_first x) y)  ) ) )" >{}


        ,  eval< "( def  'drop_last  [x](  if  (eq  (  drop_first x)  ())  ()  (cons  (first x)  (  drop_last (drop_first x)) )))" >{}


        ,  eval< "( def  'last  [x](  if  (eq  (drop_first x)  ())  x  (last  (drop_first  x))))">{}

/*
        ,  typename
           with<   eval< "( def  'drop_last  [x](  if  (eq  (  drop_first x)  ())  ()  (cons  (first x)  (  drop_last (drop_first x)) )))" >{}
                ,  eval< "( def  'last  [x](  if  (eq  (drop_first x)  ())  x  (last  (drop_first  x))))">{}
                >::
           template eval<  "(  def  'reverse  [x](  if  (eq  x  ())  ()  (cons  (last x)  (reverse  (drop_last x)))))" >{}


        ,  eval<  "(def 'length [x](  if  ( eq x () ) 0 ( + 1  (length (drop_first x))  ) ) )"  >{}


        ,  eval<  "( def 'map [f][xs] (if (eq xs ())  ()  (cons (f (first xs)) (map f (drop_first xs)  ) )   ) )"  >{}


        ,  eval<  "(def 'for_all  [xs][pred] (if (eq xs ())  true (and (pred (first xs))  (for_all (drop_first xs) pred  ) ) ) )"  >{}


        ,  eval<  "(def 'for_one  [xs][pred] (if (eq xs ()) false (or (pred (first xs))  (for_one (drop_first xs) pred  ) ) ) )"  >{}


        ,  eval<  "(def 'fold::left [f][z][xs](  if  (eq xs ())  z  ( fold::left f (f z (first xs)) (drop_first xs)  )))"  >{}


        ,  eval<  "( def  'fold::left_1  [f][xs]  (  if ( eq (drop_first xs) () )  (first xs)  (  f (first xs) ( fold::left_1 f (drop_first xs) ))))"  >{}


        ,  eval<  "( def 'fold::right  [f][z][xs]  ( if  (eq xs ())  z  (f  (first xs) ( fold::right z (drop_first xs) ))    )  )"  >{}


        ,  eval<  "(  def  'fold::right  [f][xs]  ( if (eq (drop_first xs) () )  (first xs)  (f  (first xs )  (fold::right_1 f (drop_first xs)))))"  >{}

*/
        >{};
}
