/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "text.hpp"
#include "tuple.hpp"
#include "value.hpp"


namespace lt
{
    template<  typename...  >
    struct lut;



    template<>
    struct lut<>
    {
    private:

        template<  typename...  >
        friend struct lut;



        template<  typename Key,  typename Val  >
        struct entry
        {
            static consteval entry< Key, Val >  get_entry_(  Key  );



            static constexpr bool  contains_key_(  Key  )
            {
                return true;
            }



            template<  typename... ignore_fallback >
            requires
            (
                sizeof...(ignore_fallback) <= 1
            )
            struct get_value
            :   type_hull< Val >
            { };
        };



        struct pseudo_entry
        {
            template<  typename Key  >
            static consteval pseudo_entry  get_entry_( Key );



            template<  typename... >
            struct get_value;



            template< typename Fallback >
            struct get_value<  Fallback  >
            :   type_hull<  Fallback  >
            { };



            template<  typename Key  >
            static constexpr bool contains_key_( Key )
            {
                return false;
            }
        };




        template<  typename T,  typename  >
        struct update_entry
        :   type_hull< T >
        { };



        template<  typename Key,  typename Val,  typename Val_new  >
        struct update_entry<  tuple< Key, Val >,  tuple< Key, Val_new >  >
        :   type_hull<  tuple< Key,  Val_new >  >
        { };



        template<  typename...  >
        struct lut_;


        // fix for gcc:
        template<  typename... Entries  >
        struct lazy_lut
        {
            using type = lut<  Entries... >;
        };


        template<  typename... Key,  typename... Value  >
        struct  lut_<  entry< Key, Value >...  >
        :   pseudo_entry
        ,   entry< Key, Value >...
        {
            using pseudo_entry::get_entry_;
            using entry< Key, Value >::get_entry_...;


            using pseudo_entry::contains_key_;
            using entry< Key, Value >::contains_key_...;



            template<  typename >
            struct add_entry_;
        };


        template< typename... >
        struct lookup_;



        template<  typename Ignore,  typename Fallback  >
        struct lookup_<  Ignore,  Fallback  >
        :   type_hull<  Fallback  >
        { };


    public:

        template<  typename Ignore,  typename... Fallback >
        using lookup = typename lookup_<  Ignore,  Fallback...  >::type;



        template<  typename Tuple  >
        using add_entry = lut< Tuple >;



        static constexpr bool contains_key(text_type auto)  noexcept { return false; }
    };



    template<  text_type...  Key
            ,  typename...   Value
            >
    struct lut<  tuple< Key, Value >...  >
    {
    private:

        using lut_  =
        typename  lut<>::template lut_<  typename lut<>::template entry< Key, Value >... >;



        template<  typename  >
        struct add_entry_;



        template<  typename Key_,  typename Val_  >
        requires
        (
           (  (Key_{} == Key{}) || ... )
        )
        struct add_entry_<  tuple< Key_, Val_ >  >
        :   type_hull<  lut<  typename lut<>::template update_entry<  tuple< Key, Value >,  tuple< Key_, Val_ >  >::type...  >  >
        { };



        template<  typename Key_,  typename Val_  >
        requires
        (
            (  ( Key_{} != Key{})  && ...  )
        )
        struct add_entry_<  tuple< Key_, Val_ > >
        :   type_hull<  lut<  tuple< Key, Value >...,  tuple< Key_, Val_ >  >   >
        { };



    public:

        template<  text_type Key_,  typename...  Fallback  >
        requires
        (
            sizeof...(Fallback) <= 1
        )
        using lookup  =  typename decltype(  lut_::get_entry_(  Key_{} )  )::
                         template get_value< Fallback...  >::type;



        static constexpr bool  contains_key(text_type auto key)  noexcept
        {
            return lut_::contains_key_( key );
        }



        template<  typename Tuple  >
        using add_entry  =  typename  add_entry_< Tuple >::type;



        template<  template<   typename...  > class  F
                ,  typename...                       Prefix
                >
        using apply  =  F<  Prefix...,  tuple< Key, Value >...  >;



        struct values
        {
            template<  template< typename... > class  F
                    ,  typename...                    Prefix  >
            using apply  =  F<  Prefix...,  Value... >;
        };
    };



    template<  typename... Entries
            ,  typename Key
            ,  typename  Value
            >
    constexpr auto operator<<(  lut< Entries... >,  tuple<  Key,  Value  >  )  noexcept
    ->  typename lut< Entries... >::template add_entry<  tuple< Key, Value >  >
    {
        return {};
    }

}
