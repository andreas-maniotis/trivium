/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "../fundamental_types/s_expr.hpp"
#include "../fundamental_types/value.hpp"



namespace lt
{
    template<>
    struct lisp::with<>
    {
    private:

        template<  typename,  typename...  >
        struct link_;



        template<  typename...  >
        struct combine_code_;



        template<  typename P  >
        struct combine_code_< P >
        :   type_hull< P > { };



        template<  typename P,  typename Q  >
        struct combine_code_<  P,  Q  >
        :   type_hull<  s_expr<  P,  Q  >  >
        { };



        template<  typename P,  typename Q,  typename... Qs  >
        struct combine_code_<  P,  Q,  Qs...  >
        :   type_hull<  s_expr<  P,  typename combine_code_< Q,  Qs... >::type  >  >
        { };



        template<  typename... Lib_Code  >
        struct link_<  lut< tuple< Lib_Code, Lib_Code >... >  >
        {
        public:

            template<  auto const program,  auto const... navigator  >
            using eval  =
            lisp::eval<  typename combine_code_<  Lib_Code...,  value_type<program>  >::type{}
                      ,  navigator...
                      >;
        };



        template<  typename     Linked
                ,  typename     Code
                ,  typename...  Deps
                ,  typename...  Queue
                >
        struct link_<  Linked
                    ,  tuple<  Code,  tuple< Deps... >  >
                    ,  Queue...
                    >
        :    link_<  Linked
                  ,  tuple< Deps,  value_type< dependencies< Deps{} > >  >...
                  ,  tuple< Code,  tuple<> >
                  ,  Queue...
                  >
        {
            static_assert( sizeof...(Deps) != 0 );
        };



        template<  typename     Linked
                ,  typename     Code
                ,  typename...  Queue
                >
        struct link_<  Linked
                    ,  tuple<  Code,  tuple<>  >
                    ,  Queue...
                    >
        :   link_<  typename Linked::template add_entry<  tuple< Code,  Code >  >,  Queue...  >
        { };



        template<  auto const...  >
        friend struct with;



    public:

        template<  auto const program,  auto const... navigator  >
        using eval = lisp::eval< program, navigator... >;

    };



    template<  auto const... symbols  >
    struct lisp::with
    :   lisp::with<>::
        template link_<  lut<>
                      , tuple<  value_type<symbols>
                             ,  value_type<  dependencies<symbols>  >
                             >...
                      >
    { };
}
