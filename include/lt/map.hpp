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


    // the concept map_parameters< Xs... > is satisfied if and only if the following conditions hold:
    //
    // a)  The typename Xs::key  exists for all Xs...  .
    // b)  The typename Xs::value exists for all Xs... .
    // c)  The pack  "typename Xs::key..."  contains no duplicates.
    //
    template<  typename... Xs  >
    concept  map_parameters  =

    requires(  Xs...,  type_hull<typename Xs::key>...,  type_hull<typename Xs::value>... ) { nullptr; }

    &&

    (   sizeof...(Xs) == 0

        ||


        (   sizeof...(Xs)

            ==

            []( auto... keys  )
            {
                return
                (   [](auto k,  auto... keys_)
                    {
                        return (unsigned( k == keys_ ) + ... );
                    }
                    ( keys,  keys... )  + ...
                );
            }(  type_hull<  typename Xs::key >{}...  )
        )
    );



    template<  typename... KV_Pair  >
    requires map_parameters< KV_Pair... >
    struct map;



    template<>
    struct map<>
    {
    private:

        template<  typename... KV_Pair  >
        requires map_parameters< KV_Pair... >
        friend struct map;



        template<  typename  KV_Pair
                ,  typename  Key     = typename KV_Pair::key
                ,  typename  Value   = typename KV_Pair::value
                >
        struct entry
        {
            static consteval entry< KV_Pair >  get_entry_(  type_hull<Key>  );



            static constexpr bool  contains_key_(  type_hull<Key>  )
            {
                return true;
            }



            template<  typename... ignore_fallback >
            requires
            (
                sizeof...(ignore_fallback) <= 1
            )
            struct get_value
            :   type_hull< Value >
            { };
        };



        struct pseudo_entry
        {
            template<  typename Key  >
            static consteval pseudo_entry  get_entry_( type_hull<Key> );



            template<  typename... >
            struct get_value;



            template< typename Fallback >
            struct get_value<  Fallback  >
            :   type_hull<  Fallback  >
            { };



            template<  typename Key  >
            static constexpr bool contains_key_( type_hull<Key> )
            {
                return false;
            }
        };



        template<  typename  KV_1
                ,  typename  KV_2
                >
        struct update_entry
        :   type_hull< KV_1 >
        { };



        template<  typename KV_1
                ,  typename KV_2
                >
        requires
        (
            type_hull<  typename KV_1::key  >{} == type_hull<  typename KV_2::key  >{}
        )
        struct update_entry<  KV_1,  KV_2  >
        :   type_hull< KV_2 >
        { };



        template<  typename...  >
        struct map_;


        // fix for gcc:
        template<  typename... Entries  >
        struct lazy_map
        {
            using type = map<  Entries... >;
        };


        template<  typename...  KV  >
        struct  map_<  entry< KV >...  >
        :   pseudo_entry
        ,   entry< KV >...
        {
            using pseudo_entry::get_entry_;
            using entry< KV >::get_entry_...;


            using pseudo_entry::contains_key_;
            using entry< KV >::contains_key_...;


            template<  typename >
            struct add_entry_;
        };


        template< typename... >
        struct lookup_;



        template<  typename Ignore,  typename Fallback  >
        struct lookup_<  Ignore,  Fallback  >
        :   type_hull<  Fallback  >
        { };



        template<  typename...  >
        struct add_or_replace_t_;



        template<  typename...  no_entries  >
        requires
        (
            sizeof...(no_entries) == 0
        )
        struct add_or_replace_t_<  no_entries... >
        :   type_hull< map<> >
        { };



        template<  typename Entry,  typename... More_Entries  >
        struct add_or_replace_t_<  Entry,  More_Entries...  >
        :   type_hull<  typename map<Entry>::template add_or_replace_types<  More_Entries...  >  >
        { };



    public:


        // change lookup to lookup_t  and add lookup for values;
        template<  typename Ignore,  typename... Fallback >
        using lookup = typename lookup_<  Ignore,  Fallback...  >::type;



        template<  typename... Entries  >
        using add_or_replace_types =  typename add_or_replace_t_< Entries... >::type;



        template<  typename... Entries  >
        using add_types =  map<  Entries...  >;



        template<  typename... Entries  >
        static constexpr auto add_or_replace(  Entries const...  )  noexcept
        ->  add_or_replace_types<  Entries...  >
        {
            return {};
        }



        template<  typename... Entries  >
        static constexpr auto add(  Entries const... ) noexcept
        ->  map< Entries... >
        {
            return {};
        }



        template<  typename K  >
        static constexpr bool contains_key()  noexcept { return false; }



        constexpr map()  noexcept = default;
    };




    template<  typename... KV_Pair  >
    requires  map_parameters< KV_Pair... >
    struct map
    {
    private:

        using map_  =
        typename  map<>::template map_<  typename map<>::template entry< KV_Pair >... >;


        template<  typename...  >
        struct add_or_replace_t_;


        template<  typename... no_entries  >
        requires
        (
            sizeof...(no_entries) == 0
        )
        struct add_or_replace_t_< no_entries... >
        :   type_hull<  map< KV_Pair... >  >
        { };



        template<  typename     New_KV_Pair
                ,  typename...  More
                >
        struct add_or_replace_t_<  New_KV_Pair,  More...  >
        :   type_hull<
                typename
                map<  typename map<>::
                       template update_entry<  KV_Pair,  New_KV_Pair  >::type...
                    >::
                template add_or_replace_types<  More...  >
            >
        { };



        template<  typename     New_KV_Pair
                ,  typename...  More
                >
        requires
        (
            (  (  type_hull<  typename KV_Pair::key  >{}
                  !=
                  type_hull<  typename New_KV_Pair::key  >{}  )  && ...  )
        )
        struct add_or_replace_t_<  New_KV_Pair,  More...  >
        :   type_hull<  typename map<  KV_Pair...,  New_KV_Pair  >::template add_or_replace_types< More...  >  >
        { };



    public:

        constexpr map( KV_Pair... )  noexcept {}



        template< auto = nullptr >
        requires
        (
            sizeof...(KV_Pair) != 0
        )
        constexpr map() { }



        template<  typename Key_,  typename...  Fallback  >
        requires
        (
            sizeof...(Fallback) <= 1
        )
        using lookup  =  typename decltype(  map_::get_entry_(  type_hull<Key_>{} )  )::
                         template get_value< Fallback...  >::type;



        template<  typename K  >
        static constexpr bool  contains_key()  noexcept
        {
            return map_::contains_key_( type_hull<K>{} );
        }



        template<  typename... KV  >
        using add_or_replace_types  =  typename add_or_replace_t_< KV... >::type;



        template<  typename... More_KV_Pairs  >
        using add_types = map< KV_Pair...,  More_KV_Pairs... >;



        template<  typename... KV  >
        static constexpr auto add_or_replace( KV...  )  noexcept
        -> add_or_replace_types<  KV...  >
        {
            return {};
        }



        template<  typename... KV  >
        static constexpr auto add( KV const...  )  noexcept
        -> map< KV_Pair...,  KV...  >
        {
            return {};
        }



        template<  template<   typename...  > class  F
                ,  typename...                       Prefix
                >
        using apply  =  F<  Prefix...,  KV_Pair...  >;

    };


    template<  typename...  KV_Pair  >
    map(  KV_Pair...  )  ->  map< KV_Pair... >;
}
