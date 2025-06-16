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
    using ts =
    lt::metaprogram<
R"( [ @lisp_to_cpp  @cpp_to_lisp  @sizeof  @alignof ]
    {
        ( def 'lisp_to_cpp   @lisp_to_cpp )

        ( def 'cpp_to_lisp   @cpp_to_lisp )

        ( def 'sizeof        @sizeof      )

        ( def 'alignof       @alignof     )

        (   def 'type_matches_pattern [ pattern  type ]

             (  if   ( or  ( eq pattern () )  ( eq type () )  )

                     (  eq pattern () )

                     (  if  ( eq  (first pattern)  (first type ) )
                            ( type_matches_pattern  (drop_first pattern)  (drop_first  type)  )
                            false
                    )
            )
        )

        (   def 'apply_type_pattern [ pattern type ]

            (   if  ( eq patern () )

                    type

                    (  apply_pattern  ( drop_first pattern )  ( cons  (first pattern) type  )  )
            )
        )


        (   def 'drop_type_pattern[ pattern type ]

            (   if ( matches_pattern pattern type  )

                (   def 'drop_ [n t]
                    (   if  (eq n 0)
                            t
                            ( drop_ (- n 1)  (drop_first t) )
                    )

                    (  drop_ (count pattern)  type )
                )

                type
            )
        )

        export
    }
)"  ,  combinator<  1,  ts_rep::template lisp_to_cpp  >
    ,  combinator<  1,  ts_rep::template cpp_to_lisp  >
    ,  combinator<  1,  ts_rep::template sizeof_      >
    ,  combinator<  1,  ts_rep::template alignof_     >
    >;
}
