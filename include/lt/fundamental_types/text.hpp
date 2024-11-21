/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "type_hull.hpp"

namespace lt
{
    template<  char...  Text  >
    struct t
    {
        template<  template<  char...  >  class  F  >
        using interpret_as  =  F<  Text...  >;
    };



    template<>
    struct t<>
    {
        template<  template<  char...  > class F  >
        using interpret_as  =  F< >;



        template<  unsigned  >
        struct literal;



        template< unsigned Zero  >
        requires(  Zero == 0u  )
        struct literal<  Zero  >
        {
        private:

            template<  unsigned >
            friend struct literal;



            template<  unsigned...  >
            struct sequence {};


            friend struct t<>;



            template<  typename  >
            struct make_chain_;


            template<  unsigned k,  unsigned... K  >
            struct make_chain_<  sequence< k,  K...  >  >
            :   make_chain_<  sequence<  k-1,  k,  K...  >  >
            { };


            template<  unsigned... K  >
            struct make_chain_<  sequence< 0, K... >  >
            {
                using result = sequence< 0, K... >;
            };



            template<  template<  char...  >  class F,  auto  const Literal,  unsigned... k  >
            static constexpr  auto split_( sequence<k...> )
            ->  F<  Literal.content[k]...  >
            {
                return {};
            }
        };



        template<  unsigned N  >
        struct literal
        {
            const char content[N];



            template<  auto... k,  template< auto... > class Sequence  >
            constexpr literal(  char const (&Text)[N],  Sequence<k...>  )
            :   content{  Text[k]...  }
            { }



            constexpr literal(  char const (&Text)[N]  )
            :  literal(  Text,  typename literal<0u>::
                                template make_chain_<  typename literal<0u>::
                                                        sequence<N-1>
                                                    >::result{} )
            { }
       };



        template<  unsigned N  >
        literal(  char  const (&)[N]  ) ->  literal< N >;




        template<  template<  char...  >  class F,  auto const  Literal  >
        static constexpr auto as_type()  noexcept
        {
            static_assert(  sizeof(Literal.content) != 0 );


            if constexpr(  sizeof(Literal.content) == 1 )
            {
                return F<>{};
            }
            else
            {
                return
                literal<0>::
                split_< F,  Literal  >(  typename literal<0u>::
                                         template make_chain_<  typename literal<0u>::
                                                                 sequence< sizeof(Literal.content)-2  >
                                                             >::
                                         result{}  ) ;
            }
        }


        template<  char... xs  >
        constexpr bool operator==( t< xs... > )  noexcept
        {
            return sizeof...(xs) == 0;
        }

    };


    template<  t<>::literal Text  >
    constexpr auto operator ""_t()  noexcept
    {
        return t<>::as_type<  t,  Text  >();
    }


    template<  char...  Text  >
    using text  = t< Text... >;


    template<  char... x  >
    constexpr bool operator==(  text< x... >,  text< x... >  )  noexcept
    {
        return true;
    }



    template<  char... x,  char... y  >
    constexpr bool operator==( text< x... >,  text< y... > )  noexcept
    {
        return false;
    }



    template<  typename T  >
    concept text_type  =  requires()
                          {
                              text<>{}.operator==(T{});
                          };

}
