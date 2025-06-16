/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "lt/s_expr.hpp"
#include "lt/map.hpp"



namespace lt
{
    template<  typename Key,  typename Value  >
    struct entry_t
    {
        template<  typename...  Entries  >
        requires map_parameters< Entries... >
        friend struct record;


        Value val_;


        template<  template<  typename, typename... > class  Reference_Seq
                ,  typename...                               Reference
                >
        constexpr entry_t(  decltype(nullptr),  Reference_Seq<  Key,  Reference... > const& rs  )
        :   val_(  static_cast< Reference const& >(rs).get()... )
        { }



        template<  template<  typename...  > class  Reference_Seq_Tbl
                ,  typename...                      Reference_Seq
                >
        constexpr entry_t(  Reference_Seq_Tbl<  Reference_Seq... > const&  tbl  )
        :   entry_t(  nullptr, tbl.select_with_key( type_hull<Key>{} )  )
        { }


        constexpr entry_t() = default;


    public:

        using key    =  Key;
        using value  =  Value;


        constexpr entry_t(entry_t const&)  =  default;
        constexpr entry_t(entry_t&&)       =  default;


        constexpr entry_t& operator=(entry_t const&)  =  default;
        constexpr entry_t& operator=(entry_t&&)       =  default;


        constexpr Value const& operator[](  type_hull<Key>  ) const &  noexcept
        {
            return val_;
        }


        constexpr Value& operator[](  type_hull<Key>  ) &  noexcept
        {
            return val_;
        }


        constexpr Value&& operator[](  type_hull<Key>  ) &&  noexcept
        {
            return static_cast<Value&&>(val_);
        }

    };



    template<  text<>::literal id,  typename T  >
    using entry = entry_t<  value_type<operator""_text<id>()>,  T  >;



    template<  typename Key,  typename Value  >
    struct fixed_entry_t
    :   entry_t<  Key,  Value  >
    {
    private:

        template<  typename... Entries  >
        requires map_parameters< Entries... >
        friend struct record;


        template<  typename  Ignore  >
        constexpr fixed_entry_t( Ignore ) noexcept { }

        constexpr fixed_entry_t() = default;


    public:

        using key    =  Key;
        using value  =  Value;

        constexpr fixed_entry_t(fixed_entry_t const&)  =  default;


        constexpr Value operator[](type_hull<Key>) const
        {
            return Value{};
        }


        constexpr bool operator==(fixed_entry_t const&) const noexcept { return true; }
    };



    template< auto f >
    struct lazy
    {
        constexpr auto operator()() const
        {
            return f();
        }
    };



    template<  typename Key,  auto f  >
    struct fixed_entry_t<  Key,  lazy<f>  >
    {
    private:

        template<  typename... Entries  >
        requires map_parameters< Entries... >
        friend struct record;


        template<  typename  Ignore  >
        constexpr fixed_entry_t( Ignore ) noexcept { }

        constexpr fixed_entry_t() = default;

    public:

        using key    =  Key;
        using value  =  decltype(f());


        constexpr fixed_entry_t(fixed_entry_t const&)  =  default;


        constexpr value operator[](type_hull<Key>) const
        {
            return f();
        }


        constexpr bool operator==(fixed_entry_t const&) const noexcept { return true; }
    };



    template<  text<>::literal id,  typename T  >
    using fixed_entry = fixed_entry_t<  value_type<operator""_text<id>()>,  T  >;



    inline constexpr struct
    {
    private:

        template<  unsigned... >
        struct index_sequence {};


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
            using seq = index_sequence<t...>;
        };


        template<  typename... X  >
        using index_sequence_for  =  typename __make_integer_seq<  sequence_,  unsigned,  sizeof...(X)  >::seq;


#elif __has_builtin(__integer_pack)


        template<  typename... X  >
        using index_sequence_for  =  index_sequence<  __integer_pack(sizeof...(X))...  >;


#else


        template<  typename  >
        struct make_chain_;


        template<  unsigned k,  unsigned... K  >
        struct make_chain_<  index_sequence< k,  K...  >  >
        :   make_chain_<  index_sequence<  k-1,  k,  K...  >  >
        { };


        template<  unsigned... K  >
        struct make_chain_<  index_sequence< 0, K... >  >
        {
            using result = index_sequence< 0, K... >;
        };


        template<  typename... X  >
        using index_sequence_for  =  typename make_chain_< index_sequence< sizeof...(X)-1>  >::result;  // sequence<0, ... , N-1 >

#endif

        template< unsigned n,  typename Ref  >
        requires
        (
            type_hull<Ref>{} == type_hull<Ref&&>{}
        )
        struct numbered_reference
        {
            Ref ref_;


            constexpr numbered_reference(Ref ref)
            :   ref_(static_cast<Ref>(ref))
            { }


            constexpr numbered_reference(numbered_reference const& other)
            :   ref_{ static_cast<Ref&&>(other.ref_)}
            { }


            constexpr Ref&& get() const { return static_cast<Ref>(ref_); }
        };



        template<  typename Key,  typename...  Numbered_Reference >
        struct reference_seq
        :   Numbered_Reference...
        {
            static constexpr type_hull<Key> key() { return {}; }


            template<  typename... Ref  >
            constexpr reference_seq(  decltype(nullptr),  Ref&&... ref  )
            :   Numbered_Reference{ static_cast<Ref&&>(ref) }...
            { }


            reference_seq(reference_seq const&) = default;


            constexpr reference_seq const&  select_with_key( type_hull<Key> )  const
            {
                return *this;
            }
        };



        template<  typename     K
                ,  typename...  R
                ,  auto...      idx
                >
        static auto reference_seq_(  type_hull<K>,  index_sequence< idx... >,  R&&... r  )
        ->  reference_seq<  K,  numbered_reference< idx, R&& >... >
        {
            return reference_seq<K,  numbered_reference< idx, R&& >... >{ nullptr, static_cast<R&&>(r)... };
        }


    public:

        template<  typename Key,  typename... Ref  >
        constexpr auto operator()( type_hull<Key>,  Ref&&... ref ) const
        {
            return
            [ &ref... ]() mutable
            {
                return
                reference_seq_(  type_hull<Key>{}
                              ,  index_sequence_for< Ref... >{}
                              ,  static_cast<Ref&&>(ref)...
                              );
            };
        }

    } argument;



    template<  text<>::literal id  >
    inline constexpr auto arg =
    []( auto&&... ref )
    {
        return argument( type_hull<value_type<operator ""_text<id>()> >{}
                       ,  static_cast<decltype(ref)&&>(ref)... );
    };




    template<  typename... Entries >
    requires map_parameters< Entries... >
    struct record;



    template<>
    struct record<>
    {
    private:


        template<  typename... Entries >
        requires map_parameters< Entries... >
        friend struct record;



        template<  typename... Reference_Seq  >
        struct reference_seq_tbl
        :   Reference_Seq...
        {
            using Reference_Seq::select_with_key...;

            constexpr reference_seq_tbl(Reference_Seq const&... rs )
            :   Reference_Seq( rs )...
            { }
        };



        template< typename T >
        struct remove_ref_
        :   type_hull< T >
        { };



        template<  typename T  >
        struct remove_ref_<  T&  >
        :   type_hull< T >
        { };



        template<  typename T  >
        struct remove_ref_< T&& >
        :   type_hull< T >
        { };


        template<  typename ,  typename U >
        struct const_qualify_second_like_first
        :   type_hull<U>
        { };


        template<  typename T,  typename U  >
        struct const_qualify_second_like_first<  T const, U  >
        :   type_hull< U const >
        { };


        template<  typename,  typename >
        struct ref_qualify_second_like_first;


        template<  typename T,  typename U  >
        struct ref_qualify_second_like_first< T&, U >
        :   type_hull< U& >
        { };


        template<  typename T,  typename U  >
        struct ref_qualify_second_like_first<  T&&, U  >
        :   type_hull< U&& >
        { };


        template<  typename T,  typename U  >
        auto&& forward_like_( U&& u )
        {
            using ret_t =
            typename
            ref_qualify_second_like_first<

                T&&,

                typename
                const_qualify_second_like_first <  typename remove_ref_<T>::type
                                                ,  typename remove_ref_<U>::type
                                                >::type
            >::type;

            return static_cast< ret_t&& >(u);
        }


        template<  typename  >
        struct eval_;


        template<  char... var  >
        struct eval_<  text<var...>  >
        {
            template<  typename Record  >
            static auto apply_to(  Record&& r )
            {
                return r[ type_hull< text<var...> >{} ];
            }
        };



        template<  char... var  >
        struct eval_<  s< value_type<"&"_text>,  text<var...>  >  >
        {
            template<  typename Record  >
            static decltype(auto) apply_to(  Record&& r )
            {
                return r[ type_hull< text<var...> >{} ];
            }
        };



        template<  char... var  >
        struct eval_<  s< value_type<"&&"_text>,  text<var...>  >  >
        {
            template<  typename Record  >
            static decltype(auto) apply_to(  Record&& r )
            {
                return r.forward()[ type_hull< text<var...> >{} ];
            }
        };



        template<  char... var  >
        struct eval_<  s< value_type<"const&"_text>,  text<var...>  >  >
        {
            template<  typename Record  >
            static decltype(auto) apply_to(  Record&& r )
            {
                return r.constant()[ type_hull< text<var...> >{} ];
            }
        };



        template<  typename...  F
                ,  typename...  X
                >
        struct eval_<  s<  s< F... >,  X... >  >
        {
            template<  typename Record  >
            static decltype(auto) apply_to(  Record&& r  )
            {
                return eval_< s< F...> >::template apply_to( static_cast<Record&&>(r) )( eval_<X>::template apply_to( static_cast< Record&& >(r))... );
            }
        };



        template<  char...      f
                ,  typename...  X
                >
        struct eval_< s<  text<f...>,  X... >  >
        {
            template<  typename Record  >
            static decltype(auto) apply_to(  Record&& r  )
            {
                return r[ type_hull<text<f...>>{} ](  eval_<X>::template apply_to( r.forward() )... );
            }
        };


    public:


        constexpr record()               =  default;
        constexpr record(record const&)  =  default;


        decltype(auto) invoke(auto&& f) const  noexcept( noexcept(f() ))
        {
            return f();
        }


        constexpr record&  operator<<=( record const& ) noexcept { return *this; }




        constexpr record const& constant() noexcept
        {
            return *this;
        }


        constexpr record&& move() noexcept
        {
            return static_cast<record&&>(*this);
        }


        constexpr record const& forward() const&  noexcept
        {
            return *this;
        }


        constexpr record& forward() &  noexcept
        {
            return *this;
        }


        constexpr record&& forward() &&  noexcept
        {
            return static_cast<record&&>(*this);
        }


        template<  typename U  >
        constexpr auto&& forward_like() const&  noexcept
        {
            return forward_like_<U>(*this);
        }


        template<  typename U  >
        constexpr auto&& forward_like() &  noexcept
        {
            return forward_like_<U>(*this);
        }



        template<  typename U  >
        constexpr auto&& forward_like() &&  noexcept
        {
            return forward_like_<U>(static_cast<record&&>(*this));
        }


    };



    template<  lt::text<>::literal key  >
    inline constexpr auto index = type_hull<  value_type< operator""_text<key>() >  >{};



    template< lt::text<>::literal key  >
    constexpr auto operator ""_index()  noexcept
    {
        return type_hull<  value_type< operator""_text<key>() >  >{};
    };


    template<  typename...                        Key
            ,  typename...                        Value
            ,  template<  typename... > class...  Entry_t
            >
    requires
        map_parameters<  Entry_t< Key, Value >...  >
    struct record<  Entry_t< Key, Value >...  >
    :   Entry_t< Key, Value >...
    {
    private:

        template<  typename... Ref_Seq  >
        constexpr  record( record<>::template reference_seq_tbl< Ref_Seq... > const& tbl )
        :   Entry_t< Key, Value >( tbl )...
        { }


    public:

        constexpr record()
        :   Entry_t<Key, Value>()...
        { }


        constexpr record( record const& ) = default;
        constexpr record( record&& )      = default;


        template<  typename... Constructor  >
        requires
        (
            sizeof...(Constructor) != 0
        )
        constexpr record(  Constructor... c )
        :   record(  record<>::template reference_seq_tbl< decltype(c())... >( c()... ) )
        { }


        using Entry_t<Key, Value>::operator[]...;



        constexpr record const& constant() const noexcept
        {
            return *this;
        }


        constexpr record&& move() noexcept
        {
            return static_cast<record&&>(*this);
        }


        constexpr record const& forward() const& noexcept
        {
            return *this;
        }


        constexpr record& forward() & noexcept
        {
            return *this;
        }



        constexpr record&& forward() && noexcept
        {
            return static_cast<record&&>(*this);
        }



        template<  typename U  >
        constexpr auto&& forward_like() const& noexcept
        {
            return record<>::forward_like_<U>(*this);
        }


        template<  typename U  >
        constexpr auto&& forward_like() &
        {
            return record<>::forward_like_<U>(*this);
        }


        template<  typename U  >
        constexpr auto&& forward_like() &&
        {
            return record<>::forward_like_<U>(move());
        }



        template<  typename Program  >
        constexpr decltype(auto) eval( Program ) const &
        {
            return record<>::template eval_<Program>::template apply_to(*this);
        }



        template<  typename Program  >
        constexpr decltype(auto) eval( Program ) &
        {
            return record<>::template eval_<Program>::template apply_to(*this);
        }



        template<  typename Program  >
        constexpr decltype(auto) eval( Program ) &&
        {
            return record<>::template eval_<Program>::template apply_to(this->move());
        }





        constexpr decltype(auto) invoke(auto&& f) const&
        {
            return f( constant()[ type_hull<Key>() ]... );
        }


        constexpr decltype(auto) invoke(auto&& f) &
        {
            return f( (*this)[ type_hull<Key>() ]... );
        }


        constexpr decltype(auto) invoke(auto&& f) &&
        {
            return f( move()[ type_hull<Key>() ]... );
        }






        template<  template< typename... > class  E
                ,  typename...                    K
                ,  typename...                    V
                >
        constexpr record& operator<<=( record< E< K, V >... > const&  other )
        {
            if ( this != &other ) [[likely]]
            {
                ( ((*this)[ type_hull<K>{} ] = other[ type_hull<K>{} ]), ... );
            }
            return *this;
        }



        template<  template< typename... > class  E
                ,  typename...                    K
                ,  typename...                    V
                >
        constexpr record& operator<<=( record< E< K, V >... >&&  other )
        {
            if ( this != &other ) [[likely]]
            {
                ( ((*this)[ type_hull<K>{} ] = other.move()[ type_hull<K>{} ]), ... );
            }
            return *this;
        }
    };
}
