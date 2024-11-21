/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once



#include "./fundamental_types/text.hpp"
#include "./fundamental_types/s_expr.hpp"
#include "./fundamental_types/tuple.hpp"
#include "./fundamental_types/type_hull.hpp"
#include "./fundamental_types/value.hpp"
#include "./fundamental_types/lut.hpp"
#include "lisp/eval.hpp"


namespace lt
{
    template<  typename...   >
    struct navigator;



    template<  auto const  symbol
            ,  typename    X
            >
    struct parameter
    {
        using key   = value_type<symbol>;
        using value = X;
    };



    template<  typename...  >
    struct navigator;



    template<  auto const... symbol
            ,  typename...   X
            >
    requires
    (
        ( text_type< value_type<symbol>  > && ... )
    )
    struct navigator<  parameter< symbol, X >...  >
    {
        template<  auto const x  >
        using lookup =  typename lut<  tuple< value_type<symbol>,  X >... >::
                        template lookup< value_type<x>  >;



        template<  typename... Entries  >
        using add_entries  =
        typename decltype(
            (   lut< tuple< value_type<symbol>, parameter< symbol, X >  >...  >{}
                <<  ...  <<
                tuple< typename Entries::key,  Entries >{}
            )
        )::values::
        template  apply< navigator >;



        template<  auto const... lib  >
        struct with
        {
            template<  auto const program  >
            using eval  =
                typename lisp::
                template with<  lib...  >::
                template eval<  program,  navigator< parameter< symbol, X >...  >{}  >;
        };

    private:

        // avoids confusion by gcc:
        template<  typename... Parameters  >
        using this_template = navigator< Parameters... >;


    public:

        template< auto const program
                ,  template< typename... > class Navigator = this_template
                >
        requires
        (
            type_hull<class_template<this_template>>{} == type_hull<class_template<Navigator>>{}
        )
        using eval  =  lisp::eval< program,  Navigator<  parameter< symbol, X >...  >{}  >;
    };
}
