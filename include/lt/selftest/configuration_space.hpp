/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <tuple>


namespace lt::selftest
{
    template<  typename...  X  >
    struct pack
    {
        template<  template<  typename...  >  class F  >
        using apply  = F<  X...  >;


        template< std::size_t k  >
        using at =
        typename std::tuple_element< k,  std::tuple< X... >  >::type;
    };



    template<  typename...  X,  typename... Y  >
    constexpr auto operator*(  pack< X... >,  pack< Y... >  )
    ->  pack<  X...,  Y...  >
    {
        return {};
    }



    template<  unsigned,  typename...   >
    struct configuration_space;



    template<  >
    struct configuration_space< 0u >
    {
    private:

        template<  unsigned,  typename...  >
        friend struct configuration_space;


        template<  typename   >
        struct configurations_generator;



        template<  typename... Packs  >
        struct configurations_generator<  pack<  Packs... >  >
        {
            template<  typename X  >
            using push_back =   pack<  decltype(  Packs{} * pack< X >{} )...  >;


            template<  typename... Xs  >
            using add_dimension  =
            configurations_generator< decltype(
                    (   push_back< Xs >{} * ... * pack<>{}    )
                                      ) >;


                using list = std::tuple<  Packs...  >;
        };

    public:

        using list = std::tuple<>;

   };



    template<  typename... X  >
    struct configuration_space<  0,  X...  >
    {
        using list  =  std::tuple<  X...  >;



        template<  unsigned n  >
        using with_dimension =  configuration_space<  n,  X...  >;

    };



    template<  typename...  X  >
    requires(    sizeof...(X) != 0  )
    struct  configuration_space< 1u,  X...  >
    {
    private:

        template<  unsigned,  typename...  >
        friend struct configuration_space;


        using generator  =
            typename configuration_space<0u>::
            template configurations_generator<  pack<  pack<X>...  >  >;


    public:


        using list  =  typename generator::list;


        template<  unsigned n  >
        using with_dimension =  configuration_space<  n,  X...  >;

    };



    template<  unsigned n,  typename...  X  >
    requires(    sizeof...(X) != 0  )
    struct  configuration_space<  n,  X...  >
    {
    private:

        template<  unsigned,  typename...  >
        friend struct configuration_space;


        using generator  =
        typename configuration_space<  n-1,  X...  >::
        generator::
        template add_dimension<  X... >;


    public:

        using list  =  typename generator::list;


        template<  unsigned N  >
        using with_dimension =  configuration_space<  N,  X...  >;

    };
}
