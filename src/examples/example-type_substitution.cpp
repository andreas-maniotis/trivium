/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////


#include <iostream>



#include "lt/symbolic_type_system.hpp"
#include "lt/navigator.hpp"
#include "lt/lisp.hpp"


#include <vector>
#include <type_traits>


using lt::operator""_s;


constexpr auto subst =
R"( (   def subst  [ source target term ]
        (   if  ( eq term source )              ; test whether term and source coincide
                target                          ; if yes, replace term by target
                (   if  ( irreducible term )
                        term
                        ( cons  ( subst source target (first      term) )
                                ( subst source target (drop_first term) )
                        )
                )
        )
    )
)"_s;


template< typename term,  typename source,  typename target  >
using nav =
lt::navigator<  lt::parameter<"term"_s,    lt::ts::symbolic<term>   >
             ,  lt::parameter<"source"_s,  lt::ts::symbolic<source> >
             ,  lt::parameter<"target"_s,  lt::ts::symbolic<target> >
             >;


template<  typename Type,  typename Old_Subtype,  typename New_Subtype  >
using replace =
lt::ts::cpp<
    typename nav<  Type,  Old_Subtype,  New_Subtype  >::
    template with< subst >::
    template eval< "( subst ( ` 'source ) (` 'target) (` 'term ) )"_s >
>;



template<  typename X  >
struct my_allocator : std::allocator<X> { };


struct foo;

using result  =
replace<  std::vector<int> (foo::*)(  std::vector< std::vector<int>  > ) volatile&& noexcept
       ,  lt::class_template< std::allocator >
       ,  lt::class_template< my_allocator >
       >;



using expected  =
std::vector<int, my_allocator<int>> (foo::*)( std::vector< std::vector<int,  my_allocator<int>>,  my_allocator<std::vector<int, my_allocator<int>>>> ) volatile&& noexcept;




int main()
{
    static_assert(  std::is_same_v<  result, expected >  );
}


