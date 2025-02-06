/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once



#include "text.hpp"
#include "symbolic_expressions.hpp"



namespace lt
{
    struct ts
    {
    private:


        template<  typename...  >
        struct construct;



        template<  typename...  >
        struct deconstruct;



//  irreducible types

        template<  typename T  >
        struct construct< T >
        :   type_hull< T >
        { };


        template<  typename T  >
        struct deconstruct< T >
        :   type_hull< T >
        { };


//  class templates:

        template<  template<  typename... >  class F  >
        struct construct<  class_template< F >  >
        :   type_hull<  class_template< F >  >
        { };



        template<  template<  typename... > class  F
                ,  typename...                     Xs
                >
        struct construct<  value_type<"class_template"_text>
                        ,  class_template< F >
                        ,   Xs...
                        >
        :   type_hull<  F<  Xs...  >  >
        { };



        template<  template<  typename... >   class F
                ,  typename...                Ys
                >
        struct deconstruct<  F<  Ys...  >  >
        :   type_hull<  s<  value_type<"class_template"_text>
                              ,  class_template< F >
                              ,  typename deconstruct< Ys >::type...
                              >
                     >
        { };



//  references:

        template<  typename  T  >
        struct construct<  value_type<"&"_text>,   T    >
        :   type_hull< T& >
        { };



        template<  typename T  >
        struct deconstruct<  T&  >
        :   type_hull<  s<  value_type<"&"_text>,  typename deconstruct<T>::type  >  >
        { };


//  rvalue-references:

        template<  typename  T  >
        struct construct<  value_type<"&&"_text>,   T    >
        :   type_hull< T&& >
        { };



        template<  typename T  >
        struct deconstruct<  T&&  >
        :   type_hull<  s<  value_type<"&&"_text>,  typename deconstruct<T>::type  >  >
        { };


//  const:

        template<  typename  T  >
        struct construct<  value_type<"const"_text>,   T    >
        :   type_hull<  T const  >
        { };



        template<  typename T  >
        struct deconstruct<  T const  >
        :   type_hull<  s<  value_type<"const"_text>,  typename deconstruct<T>::type  >  >
        { };


//  volatile:

        template<  typename  T  >
        struct construct<  value_type<"volatile"_text>,   T    >
        :   type_hull< T volatile >
        { };



        template<  typename T  >
        struct deconstruct<  T volatile  >
        :   type_hull<  s<  value_type<"volatile"_text>,  typename deconstruct<T>::type  >  >
        { };


//  const volatile:

        template<  typename  T  >
        struct construct<  value_type<"const_volatile"_text>,   T    >
        :   type_hull<  T const volatile  >
        { };



        template<  typename T  >
        struct deconstruct<  T const volatile >
        :   type_hull<  s<  value_type<"const_volatile"_text>,  typename deconstruct<T>::type  >  >
        { };


//  array:

        template<  typename  T  >
        struct construct<  value_type<"array"_text>,   T    >
        :   type_hull<  T []  >
        { };



        template<  typename T  >
        struct deconstruct<  T [] >
        :   type_hull<  s<  value_type<"array"_text>,  typename deconstruct<T>::type  >  >
        { };


        template<  typename  T,  int n  >
        struct construct<  value_type<"array"_text>,   T,  integer<n>   >
        :   type_hull<  T [n]  >
        { };



        template<  typename T,  auto n  >
        struct deconstruct<  T [n] >
        :   type_hull<  s<  value_type<"array"_text>,  typename deconstruct<T>::type,  integer<n>  >  >
        { };


//  pointers:

        template<  typename  T  >
        struct construct<  value_type<"*"_text>,   T    >
        :   type_hull<  T*  >
        { };



        template<  typename T  >
        struct deconstruct<  T* >
        :   type_hull<  s<  value_type<"*"_text>,  typename deconstruct<T>::type  >  >
        { };



        template<  typename  T  >
        struct construct<  value_type<"*const"_text>,   T    >
        :   type_hull<  T *const  >
        { };



        template<  typename T  >
        struct deconstruct<  T *const >
        :   type_hull<  s<  value_type<"*const"_text>,  typename deconstruct<T>::type  >  >
        { };



        template<  typename  T  >
        struct construct<  value_type<"*volatile"_text>,   T    >
        :   type_hull<  T *volatile  >
        { };



        template<  typename T  >
        struct deconstruct<  T *volatile >
        :   type_hull<  s<  value_type<"*volatile"_text>,  typename deconstruct<T>::type  >  >
        { };



        template<  typename  T  >
        struct construct<  value_type<"*const_volatile"_text>,   T    >
        :   type_hull<  T *const volatile  >
        { };



        template<  typename T  >
        struct deconstruct<  T *const volatile >
        :   type_hull<  s<  value_type<"*const_volatile"_text>,  typename deconstruct<T>::type  >  >
        { };



//  member pointers:

        template<  typename  X
                ,  typename  Y
                >
        struct construct<  value_type<"::*"_text>,   X,  Y    >
        :   type_hull<  Y X::*  >
        { };



        template<  typename  X
                ,  typename  Y
                >
        struct deconstruct<  Y X::* >
        :   type_hull<  s<  value_type<"::*"_text>
                              ,  typename deconstruct<X>::type
                              ,  typename deconstruct<Y>::type
                              >
                     >
        { };



//  functions from symbolic expressions:

        template<  typename     X
                ,  typename...  Xs
                >
        struct construct<  value_type<"signature"_text>,  X,  Xs...  >
        :   type_hull<  s<  value_type<"signature"_text>,  X,  Xs...  >  >
        { };



        template<  typename     X
                ,  typename...  Xs
                >
        struct construct<  value_type<"function"_text>
                        ,  s<  value_type<"signature"_text>,  X,  Xs... >
                        >
        :   type_hull<  X (Xs...)  >
        { };


// With the help of the preprocessor we generate the type definitions for X (Xs..) cv r ne
// with  cv = const / volatile / const volatile / "nothing"  and r = & / && / "nothing".
//
// The case X (xs...), which arises from cv = "nothing" and r="nothing" is not treated by
// the preprocessor, but spelled out above.


#pragma push_macro("TRIVIUM_FCONSTRUCT")
#define  TRIVIUM_FCONSTRUCT( Qual, Qual_ ) \
        template<  typename     X \
                ,  typename...  Xs \
                >\
        struct construct<  value_type<"function"_text>\
                        ,  s<  value_type<"signature"_text>,  X,  Xs... >,  Qual_\
                        >\
        :   type_hull<  X (Xs...)  Qual  >\
        { };



TRIVIUM_FCONSTRUCT(               &,  value_type<"&"_text> )
TRIVIUM_FCONSTRUCT(              &&,  value_type<"&&"_text> )

TRIVIUM_FCONSTRUCT(           const,  value_type<"const"_text> )
TRIVIUM_FCONSTRUCT(        volatile,  value_type<"volatile"_text> )
TRIVIUM_FCONSTRUCT(  const volatile,  value_type<"const_volatile"_text> )

TRIVIUM_FCONSTRUCT(          const&,  value_type<"const&"_text> )
TRIVIUM_FCONSTRUCT(       volatile&,  value_type<"volatile&"_text> )
TRIVIUM_FCONSTRUCT( const volatile&,  value_type<"const_volatile&"_text> )

TRIVIUM_FCONSTRUCT(         const&&,  value_type<"const&&"_text> )
TRIVIUM_FCONSTRUCT(      volatile&&,  value_type<"volatile&&"_text> )
TRIVIUM_FCONSTRUCT(const volatile&&,  value_type<"const_volatile&&"_text> )

#pragma pop_macro("TRIVIUM_FCONSTRUCT")

//  Same procedure as above, but this time for noexcept-functions:



        template<  typename     X
                ,  typename...  Xs
                >
        struct construct<  value_type<"function"_text>
                        ,  s<  value_type<"signature"_text>,  X,  Xs... >
                        ,  value_type<"noexcept"_text>
                        >
        :   type_hull<  X (Xs...) noexcept  >
        { };


#pragma push_macro("TRIVIUM_F_NE_CONSTRUCT")

#define  TRIVIUM_F_NE_CONSTRUCT( Qual, Qual_ ) \
        template<  typename     X \
                ,  typename...  Xs \
                >\
        struct construct<  value_type<"function"_text>\
                        ,  s<  value_type<"signature"_text>,  X,  Xs... > \
                                 ,  Qual_\
                                 ,  value_type<"noexcept"_text> \
                        >\
        :   type_hull<  X (Xs...)  Qual  noexcept >\
        { };


TRIVIUM_F_NE_CONSTRUCT(               &,  value_type<"&"_text> )
TRIVIUM_F_NE_CONSTRUCT(              &&,  value_type<"&&"_text> )

TRIVIUM_F_NE_CONSTRUCT(           const,  value_type<"const"_text> )
TRIVIUM_F_NE_CONSTRUCT(        volatile,  value_type<"volatile"_text> )
TRIVIUM_F_NE_CONSTRUCT(  const volatile,  value_type<"const_volatile"_text> )

TRIVIUM_F_NE_CONSTRUCT(          const&,  value_type<"const&"_text> )
TRIVIUM_F_NE_CONSTRUCT(       volatile&,  value_type<"volatile&"_text> )
TRIVIUM_F_NE_CONSTRUCT( const volatile&,  value_type<"const_volatile&"_text> )

TRIVIUM_F_NE_CONSTRUCT(         const&&,  value_type<"const&&"_text> )
TRIVIUM_F_NE_CONSTRUCT(      volatile&&,  value_type<"volatile&&"_text> )
TRIVIUM_F_NE_CONSTRUCT(const volatile&&,  value_type<"const_volatile&&"_text> )

#pragma pop_macro("TRIVIUM_F_NE_CONSTRUCT")


//  Symbolic expressions from functions:


        template<  bool,  typename  Expr  >
        struct with_noexcept_;



        template<  typename... Expr  >
        struct with_noexcept_< false,  s< Expr... >  >
        :   type_hull<  s< Expr... >  >
        { };



        template<  typename... Expr  >
        struct with_noexcept_< true,  s< Expr... >  >
        :   type_hull<  s< Expr...,  value_type<"noexcept"_text>  >  >
        { };



        template<  typename     R
                ,  typename...  Xs
                >
        struct deconstruct<   R (Xs...) noexcept  >
        :   with_noexcept_<  true
                          ,  s<  value_type<"function"_text>
                                   ,  s<  value_type<"signature"_text>
                                            ,  typename deconstruct<R>::type
                                            ,  typename deconstruct<Xs>::type...
                                            >
                                   >
                          >
        { };

#pragma push_macro("TRIVIUM_F_DECONSTRUCT")

#define TRIVIUM_F_DECONSTRUCT(QUAL, QUAL_)                                          \
       template<  typename     R                                                    \
                ,  typename...  Xs                                                  \
                >                                                                   \
        struct deconstruct<   R (Xs...) QUAL noexcept      >                        \
        :   with_noexcept_< true                                                    \
                          ,  s<  value_type<"function"_text>                      \
                                   ,  s<  value_type<"signature"_text>            \
                                            ,  typename deconstruct<R>::type        \
                                            ,  typename deconstruct<Xs>::type...    \
                                            >                                       \
                                   ,  QUAL_                                         \
                                   >                                                \
                          >                                                         \
        { };


        TRIVIUM_F_DECONSTRUCT(               &,  value_type<"&"_text> )
        TRIVIUM_F_DECONSTRUCT(              &&,  value_type<"&&"_text> )

        TRIVIUM_F_DECONSTRUCT(           const,  value_type<"const"_text> )
        TRIVIUM_F_DECONSTRUCT(        volatile,  value_type<"volatile"_text> )
        TRIVIUM_F_DECONSTRUCT(  const volatile,  value_type<"const_volatile"_text> )

        TRIVIUM_F_DECONSTRUCT(          const&,  value_type<"const&"_text> )
        TRIVIUM_F_DECONSTRUCT(       volatile&,  value_type<"volatile&"_text> )
        TRIVIUM_F_DECONSTRUCT( const volatile&,  value_type<"const_volatile&"_text> )

        TRIVIUM_F_DECONSTRUCT(         const&&,  value_type<"const&&"_text> )
        TRIVIUM_F_DECONSTRUCT(      volatile&&,  value_type<"volatile&&"_text> )
        TRIVIUM_F_DECONSTRUCT(const volatile&&,  value_type<"const_volatile&&"_text> )

#pragma pop_macro("TRIVIUM_F_DECONSTRUCT")


// The same without noexcept:


        template<  typename     R
                ,  typename...  Xs
                >
        struct deconstruct<   R (Xs...)   >
        :   with_noexcept_<  false
                          ,  s<  value_type<"function"_text>
                                   ,  s<  value_type<"signature"_text>
                                            ,  typename deconstruct<R>::type
                                            ,  typename deconstruct<Xs>::type...
                                            >
                                   >
                          >
        { };

#pragma push_macro("TRIVIUM_F_DECONSTRUCT")

#define TRIVIUM_F_DECONSTRUCT(QUAL, QUAL_)                                          \
       template<  typename     R                                                    \
                ,  typename...  Xs                                                  \
                >                                                                   \
        struct deconstruct<   R (Xs...) QUAL               >                        \
        :   with_noexcept_< false                                                   \
                          ,  s<  value_type<"function"_text>                      \
                                   ,  s<  value_type<"signature"_text>            \
                                            ,  typename deconstruct<R>::type        \
                                            ,  typename deconstruct<Xs>::type...    \
                                            >                                       \
                                   ,  QUAL_                                         \
                                   >                                                \
                          >                                                         \
        { };


        TRIVIUM_F_DECONSTRUCT(               &,  value_type<"&"_text> )
        TRIVIUM_F_DECONSTRUCT(              &&,  value_type<"&&"_text> )

        TRIVIUM_F_DECONSTRUCT(           const,  value_type<"const"_text> )
        TRIVIUM_F_DECONSTRUCT(        volatile,  value_type<"volatile"_text> )
        TRIVIUM_F_DECONSTRUCT(  const volatile,  value_type<"const_volatile"_text> )

        TRIVIUM_F_DECONSTRUCT(          const&,  value_type<"const&"_text> )
        TRIVIUM_F_DECONSTRUCT(       volatile&,  value_type<"volatile&"_text> )
        TRIVIUM_F_DECONSTRUCT( const volatile&,  value_type<"const_volatile&"_text> )

        TRIVIUM_F_DECONSTRUCT(         const&&,  value_type<"const&&"_text> )
        TRIVIUM_F_DECONSTRUCT(      volatile&&,  value_type<"volatile&&"_text> )
        TRIVIUM_F_DECONSTRUCT(const volatile&&,  value_type<"const_volatile&&"_text> )

#pragma pop_macro("TRIVIUM_F_DECONSTRUCT")




        template<  typename X  >
        struct cpp_type_from_symbolic_
        :   type_hull< X >
        { };



        template<  typename... FXs  >
        struct cpp_type_from_symbolic_<  s< FXs... >  >
        :   construct<  typename cpp_type_from_symbolic_<FXs>::type...  >
        { };



    public:

        template<  typename CppType  >
        using symbolic  =  typename deconstruct< CppType >::type;



        template<  typename Symbolic_Type  >
        using cpp  =  typename cpp_type_from_symbolic_<  Symbolic_Type  >::type;
    };
}
