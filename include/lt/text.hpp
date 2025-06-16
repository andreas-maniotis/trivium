/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


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



            friend struct t<>;



            template<  unsigned...  >
            struct sequence {};



            template<  template<  char...  >  class F,  auto  const Literal,  unsigned... k  >
            static constexpr  auto split_( sequence<k...> )
            ->  F<  Literal.content[k]...  >
            {
                return {};
            }

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable: 4067 )
#endif

//  disable "warning C4067: unexpected tokens following preprocessor directive - expected a newline"
//  in the #if macro below:

#if defined(_MSC_VER) || __has_builtin(__make_integer_seq)

#ifdef _MSC_Ver
#pragma wanring( pop )
#endif

            template< typename T,  T...  t  >
            struct sequence_
            {
                using seq = sequence<t...>;
            };


            template<  unsigned N  >
            using  make_chain  =  typename __make_integer_seq<  sequence_,  unsigned,  N  >::seq;

#elif __has_builtin(__integer_pack)

            template<  unsigned N  >
            using make_chain  =  sequence<  __integer_pack(N)...  >;


#else

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


            template<  unsigned N  >
            using make_chain =  typename make_chain_< sequence<N-1>  >::result;  // sequence<0, ... , N-1 >

#endif


            template< auto Literal,  unsigned,  typename Seq  >
            struct subtext_;



            template<  auto Literal,  unsigned delta,  unsigned... idx  >
            struct subtext_<  Literal,  delta,  sequence< idx... >  >
            {
                using result  =  t<  Literal.content[idx+delta]... >;
            };




            template<  auto Literal,  unsigned begin,  unsigned end  >
            using subtext  =
            typename subtext_<  Literal,  begin,  make_chain< end - begin >  >::result;

        };



        template<  auto      Literal
                ,  unsigned  begin
                ,  unsigned  end
                >
        requires( begin <= end )
        using subtext = literal<0>::template subtext< Literal,  begin,  end  >;



        template<  unsigned N  >
        struct literal
        {
            const char content[N];


        private:

            template<  auto... k,  template< auto... > class Sequence  >
            constexpr literal(  char const (&Text)[N],  Sequence<k...>  )
            :   content{  Text[k]...  }
            { }


        public:

            constexpr literal(  char const (&Text)[N]  )
            :  literal(  Text,  typename literal<0u>::template make_chain<N>{})
            { }



            constexpr literal(  literal const& other )
            :   literal( other.content )
            { }


            template<  char... src  >
            requires(  sizeof...(src) == N-1 )
            constexpr literal(  t< src... >  )
            :   content{ src... }
            { }
       };



        template<  unsigned N  >
        literal(  char  const (&)[N]  )  ->  literal< N >;



        template< char... src  >
        literal(  t< src... >  )   ->  literal<  sizeof...(src) + 1  >;



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
                split_< F,  Literal  >(  typename literal<0u>::template make_chain< sizeof(Literal.content)-1>{}) ;
            }
        }


        template<  char... xs  >
        constexpr bool operator==( t< xs... > )  noexcept
        {
            return sizeof...(xs) == 0;
        }

    };


    template<  t<>::literal Text  >
    constexpr auto operator ""_text()  noexcept
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
