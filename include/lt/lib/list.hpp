/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "../type_system.hpp"
#include "../eval.hpp"


namespace lt::lib
{
    using list = lt::eval<
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


    (  def  'drop

            [n xs]
            (
                if ( eq n 0 )
                xs
                ( drop (- n 1 )  (tail xs ) )
            )
    )



    (  def  'drop_at_end
            [n xs]
            (
                reverse (drop n ( reverse xs ))
            )
    )



    (  def  'count
            [xs]
            (
                if  (eq xs ())
                0
                ( + 1 ( count (tail xs ) ) )
            )
    )



    (  def  'apply_to_all
            [f  xs]
            (
                if  (eq  xs () )
                    ()
                    ( cons (f (first xs)) (apply_to_all (tail xs)) )
            )
    )


    ( def   'integer_range    ;  creates the open integer interval [p q) (assumes p < q )
            [ p  q ]
            (
                def 'f
                    [xs q_]
                    (
                        if ( < q_, p )
                        xs
                        (  f ( cons q_  xs ) (- q_ 1) )
                    )

                    (f  () (- q 1) )
            )
    )


    ( def   'postfix    ; keep last n elements of a list
            [ n  xs ]
            (
                drop  ( - (count xs) n )  xs
            )
    )


    ( def   'prefix   ;  keep first n elements of a list
            [ n xs ]
            (
                reverse (postfix n (reverse xs ))
            )
    )

    ( def   'zip
            [ op  xs  ys ]
            (
                def 'zip_
                    [ xs_ ys_ ]
                    (
                        if ( eq xs_ () )
                        ()
                        (  cons (op  (first xs_)  (first ys_) )  (zip_ (drop_first xs)  (drop_first ys) ) )
                    )

                (reverse ( zip_  xs ys ))

            )
    )


; partition
; select
; sort

    export




} )" >;
}
