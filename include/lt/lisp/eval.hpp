/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "../fundamental_types/s_expr.hpp"
#include "../fundamental_types/lut.hpp"
#include "../fundamental_types/application.hpp"
#include "../fundamental_types/tuple.hpp"
#include "../fundamental_types/type_hull.hpp"
#include "../fundamental_types/value.hpp"
#include "../fundamental_types/combinator.hpp"



namespace lt
{
    template<  typename...  >
    struct navigator;
}



namespace lt
{
    struct lisp
    {
    private:

        template<  typename X  >
        using with_result  =  type_hull< X >;



        template<  text_type auto const t  >
        using op = value_type<t>;



        enum class eval_mode {  lazy,  eager  };



        template<  eval_mode,  typename  >
        struct eval_;  // The evaluator


        template<  typename Expr  >
        using eager_eval = typename eval_<  eval_mode::eager,  Expr  >::type;


        template<  eval_mode,  typename...  >
        struct call;



        struct Y_combinator {};


// -----------------------------------------------------------------------------
//
// Keywords and Constants
//
// -----------------------------------------------------------------------------


// elementary functions (except quote):

        using  apply        =  value_type<"apply"_t>;
        using  and_         =  value_type<"and"_t>;
        using  cons         =  value_type<"cons"_t>;
        using  def          =  value_type<"def"_t>;
        using  drop_first   =  value_type<"drop_first"_t>;
        using  evaluate     =  value_type<"eval"_t>;
        using  eq           =  value_type<"eq"_t>;
        using  first        =  value_type<"first"_t>;
        using  if_          =  value_type<"if"_t>;
        using  irreducible  =  value_type<"irreducible"_t>;
        using  not_         =  value_type<"not"_t>;
        using  or_          =  value_type<"or"_t>;
        using  requires_    =  value_type<"requires"_t>;
        using  xor_         =  value_type<"xor"_t>;
        using  true_        =  value_type<"true"_t>;
        using  false_       =  value_type<"false"_t>;


// -----------------------------------------------------------------------------
//
// Arities
//
// -----------------------------------------------------------------------------

        static consteval int  arity(...) { return -1; }



        template<  auto N  >
        static consteval int  arity(list<N>)   { return N; }



        template<  auto N
                ,  template< typename... > class F
                >
        static consteval int  arity(combinator< N, F >) { return N; }



        template<  typename... Entries  >
        static consteval int  arity(navigator< Entries... >) { return 1; }


        static consteval int  arity(true_)         { return 0; }
        static consteval int  arity(false_)        { return 0; }
        static consteval int  arity(s_expr<>)      { return 0; }


        static consteval int  arity(evaluate)      { return 1; }
        static consteval int  arity(first)         { return 1; }
        static consteval int  arity(irreducible)   { return 1; }
        static consteval int  arity(not_)          { return 1; }
        static consteval int  arity(drop_first)    { return 1; }

        static consteval int  arity(apply)         { return 2; }
        static consteval int  arity(and_)          { return 2; }
        static consteval int  arity(cons)          { return 2; }
        static consteval int  arity(eq)            { return 2; }
        static consteval int  arity(K)             { return 2; }
        static consteval int  arity(requires_)     { return 2; }
        static consteval int  arity(or_)           { return 2; }
        static consteval int  arity(xor_)          { return 2; }
        static consteval int  arity(Y_combinator)  { return 2; }
        static consteval int  arity(op<"=="_t>)    { return 2; }
        static consteval int  arity(op<"!="_t>)    { return 2; }
        static consteval int  arity(op<"<"_t>)     { return 2; }
        static consteval int  arity(op<"<="_t>)    { return 2; }
        static consteval int  arity(op<">"_t>)     { return 2; }
        static consteval int  arity(op<">="_t>)    { return 2; }
        static consteval int  arity(op<"+"_t>)     { return 2; }
        static consteval int  arity(op<"-"_t>)     { return 2; }
        static consteval int  arity(op<"*"_t>)     { return 2; }
        static consteval int  arity(op<"/"_t>)     { return 2; }
        static consteval int  arity(op<"%"_t>)     { return 2; }


        static consteval int  arity(if_)           { return 3; }
        static consteval int  arity(S)             { return 3; }
        static consteval int  arity(def)           { return 3; }




// -----------------------------------------------------------------------------
//  quoted expressions:
// -----------------------------------------------------------------------------


        template<  typename   Expr
                ,  eval_mode  mode
                >
        struct eval_<  mode,  quoted<Expr>  >
        :   type_hull< Expr >
        { };



// -----------------------------------------------------------------------------
//  Keywords and constants (i e anything with arity >= 0 ):
// -----------------------------------------------------------------------------

        template<  eval_mode  mode,  typename T  >
        requires
        (
            arity(T{}) >= 0
        )
        struct eval_<  mode,  T  >
        :   with_result<  T  >
        { };



// -----------------------------------------------------------------------------
//  Values:
// -----------------------------------------------------------------------------


        template<  eval_mode mode
                ,  auto      val
                >
        struct eval_<  mode,  value< val >  >
        :   with_result<  value< val >  >
        { };



// -----------------------------------------------------------------------------
//  Combinators:
// -----------------------------------------------------------------------------


        template<  eval_mode                     mode
                ,  auto                          N
                ,  template< typename...> class  F
                >
        struct eval_<  mode
                    ,  combinator< N, F >
                    >
        :   with_result<  combinator< N, F >  >
        { };



// -----------------------------------------------------------------------------
//  Constants:
// -----------------------------------------------------------------------------


        template<  eval_mode mode >
        struct eval_<  mode,  true_  >
        :   with_result<  true_ >
        { };



        template<  eval_mode mode >
        struct eval_<  mode,  false_  >
        :   with_result<  false_ >
        { };



        template<  eval_mode mode >
        struct eval_<  mode,  s_expr<>  >
        :   with_result<  s_expr<>  >
        { };



// -----------------------------------------------------------------------------
//  Empty lists:
// -----------------------------------------------------------------------------


        template<  eval_mode mode >
        struct eval_<  mode,  s_expr< list<0>  >  >
        :   with_result<  s_expr<>  >
        { };


// -----------------------------------------------------------------------------
// Use of an undefined symbol: Generate an error message.
// -----------------------------------------------------------------------------

        template<  eval_mode    mode
                ,  char...      undefined_symbol
                >
        requires
        (
            arity(text< undefined_symbol... >{}) < 0
        )
        struct eval_<  mode,  text< undefined_symbol... >  >
        {
           static_assert( arity(text< undefined_symbol... >{}) >= 0,
R"(

! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
!
! METAPROGRAMMING ERROR:
!
! Attempt to use an undefined symbol. Either fix a typing error
! in the metaprogram or provide the definition if it was forgotten.
!
! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

)" );

        };



// -----------------------------------------------------------------------------
//  Expressions:
// -----------------------------------------------------------------------------


// Less operands than needed given to expression, do nothing:

        template<  eval_mode    mode
                ,  typename     Op
                ,  typename     X
                ,  typename...  Xs
                >
        requires
        (
            int{1 + sizeof...(Xs)}  <  arity(Op{})
        )
        struct eval_<  mode
                    ,  s_expr<  Op,  X,  Xs...  >
                    >
        :   with_result<  s_expr<  Op,  X,  Xs...  >  >
        {};


// Call of an undefined function: Create an error message.

        template<  eval_mode    mode
                ,  typename     Op
                ,  typename     X
                ,  typename...  Xs
                >
        requires
        (
            arity(Op{}) < 0
        )
        struct eval_<  mode
                    ,  s_expr<  Op,  X,  Xs...  >
                    >
        {
           static_assert( arity(Op{}) >= 0,
R"(

! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
!
! METAPROGRAMMING ERROR:
!
! Attempt to call an undefined function. Either fix a typing error
! in the metaprogram or provide the definition if it was forgotten.
!
! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

)" );

        };



// Expression ready for evalution:

        template<  eval_mode    mode
                ,  typename     Op
                ,  typename     X
                ,  typename...  Xs
                >
        struct eval_<  mode
                    ,  s_expr<  Op,  X,  Xs...  >
                    >
        :   call<  mode,  Op,  X,  Xs...  >
        {
            static_assert(  1 + sizeof...(Xs) == arity(Op{})   );
        };




// The length of  the expression sequence following an operator
// is longer than the arity of that operator. Begin Currying by
// feeding the operator with its first input expression:

        template<  eval_mode    mode
                ,  typename     Op
                ,  typename     X
                ,  typename     Y
                ,  typename...  Ys
                >
        requires
        (
            arity(Op{}) > 0
            &&
            2 + sizeof...(Ys)  > arity(Op{})
        )
        struct eval_<  mode
                    ,  s_expr<  Op,  X,  Y,  Ys...  >
                    >
        :   eval_<  mode
                 ,  s_expr<  eager_eval< s_expr< Op,  X  >  >
                          ,  Y
                          ,  Ys...
                          >
                 >
        { };



// Continue currying with last remaining input expression:

        template<  eval_mode    mode
                ,  typename     Op
                ,  typename     X
                ,  typename...  Xs
                ,  typename     Y
                >
        requires
        (
            arity(Op{}) > 0
            &&
            2 + sizeof...(Xs)  <=  arity(Op{})
        )
        struct eval_<  mode
                    ,  s_expr<  s_expr<  Op,  X,  Xs...  >,  Y  >
                    >
        :   eval_<  mode
                 ,  s_expr<  Op,  X,  Xs... ,  Y  >
                 >
        { };



// Continue curring with possibly more than one input expressions:

        template<  eval_mode    mode
                ,  typename     Op
                ,  typename     X
                ,  typename...  Xs
                ,  typename     Y
                ,  typename     Z
                ,  typename...  Zs
                >
        requires
        (
            arity(Op{}) > 0
            &&
            2 + sizeof...(Xs)  <=  arity(Op{})
        )
        struct eval_<  mode
                    ,  s_expr<  s_expr<  Op,  X,  Xs...  >,  Y,  Z,  Zs...  >
                    >
        :   eval_<  mode
                 ,  s_expr<  eager_eval<  s_expr<  Op,  X,  Xs...,  Y  >  >
                          ,  Z
                          ,  Zs...
                          >
                 >
        { };



//  General expression that contains subexpressions.  (X is not an operator name):

        template<  eval_mode    mode
                ,  typename     X
                ,  typename...  Xs
                ,  typename     Y
                ,  typename...  Ys
                >
       struct eval_<  mode
                    ,  s_expr<  s_expr<  X,  Xs... >,  Y,  Ys...  >
                    >
        :   eval_<  mode
                ,  s_expr<  X,  Xs...,  Y,  Ys...  >
                >
        { };





//  ----------------------------------------------------------------------------
//  list:
//  ----------------------------------------------------------------------------

        template<  typename... Xs  >
        struct lazy_list {};



        template<  typename... Xs  >
        struct eval_<  eval_mode::eager,  lazy_list<  Xs...  >  >
        :   with_result<  s_expr<   typename eval_<  eval_mode::eager,  Xs  >::type...  >  >
        { };



        template<  typename...  Xs  >
        struct eval_<  eval_mode::lazy,  lazy_list<  Xs...  >  >
        :   with_result<  lazy_list< Xs... >  >
        { };



        template<  auto         N
                ,  typename...  Xs
                >
        requires(  sizeof...(Xs) == N  )  // N==0?
        struct call<  eval_mode::eager
                   ,  list< N >
                   ,  Xs...
                   >
        :   with_result<  s_expr<  typename eval_<  eval_mode::eager,  Xs  >::type...  >  >
        { };



        template<  auto         N
                ,  typename...  Xs
                >
        requires(  sizeof...(Xs) == N  )  // N==0?
        struct call<  eval_mode::lazy
                   ,  list< N >
                   ,  Xs...
                   >
        :   with_result<  lazy_list<  Xs...  >   >
        { };



//  ----------------------------------------------------------------------------
//  combinator:
//  ----------------------------------------------------------------------------


    template<  eval_mode                         mode
            ,  auto                              N
            ,  template<  typename...  >  class  F
            ,  typename...                       Xs
            >
    requires
    (
        sizeof...(Xs) == N
    )
    struct call<  mode,  combinator<  N,  F  >,  Xs...  >
    :   with_result<  F<  eager_eval<Xs>...  >  >
    { };



//  ----------------------------------------------------------------------------
//  first:
//  ----------------------------------------------------------------------------


        template<  eval_mode,  typename  >
        struct First;



        template<  eval_mode    mode
                ,  typename     X
                ,  typename...  Xs
                >
        struct First<  mode
                   ,  lazy_list<  X,  Xs...  >
                   >
        :   eval_<  mode,  X  >
        { };



        template<  eval_mode    mode
                ,  typename     X
                ,  typename...  Xs
                >
        struct First<  mode
                   ,  s_expr<  X,  Xs...  >
                   >
        :   with_result< X  >
        { };



        template<  eval_mode  mode
                ,  typename   X
                >
        struct call<  mode,  first,  X  >
        :   First<  mode,  typename eval_< eval_mode::lazy, X >::type  >
        { };



//  ----------------------------------------------------------------------------
//  drop_first:
//  ----------------------------------------------------------------------------


        template<  eval_mode,  typename  >
        struct Drop_First;



        template<  eval_mode    mode
                ,  typename     X
                ,  typename...  Xs
                >
        struct Drop_First<  mode
                        ,  lazy_list<  X,  Xs...  >
                        >
        :   eval_<  mode,  lazy_list<  Xs...  >  >
        { };



        template<  eval_mode    mode
                ,  typename     X
                ,  typename...  Xs
                >
        struct Drop_First<  mode
                        ,  s_expr<  X,  Xs...  >
                        >
        :   with_result<  s_expr< Xs... >  >
        { };



        template<  eval_mode  mode
                ,  typename   X
                >
        struct call<  mode
                   ,  drop_first
                   ,  X
                   >
        :   Drop_First<  mode,  typename eval_<  eval_mode::lazy,  X  >::type  >
        { };



// -----------------------------------------------------------------------------
//  cons:
// -----------------------------------------------------------------------------

        template<  eval_mode,  typename,  typename  >
        struct Cons;


        template<  typename     X
                ,  typename...  Xs
                >
        struct Cons<  eval_mode::lazy
                   ,  X
                   ,  lazy_list< Xs...  >
                   >
        :   with_result<  lazy_list<  X,  Xs...  >  >
        { };



        template<  typename     X
                ,  typename...  Xs
                >
        struct Cons<  eval_mode::lazy
                   ,  X
                   ,  s_expr<  Xs...  >
                   >
        :   with_result<  lazy_list<  X,  quoted<Xs>...  >  >
        { };



        template<  typename     X
                ,  typename...  Xs
                >
        struct Cons<  eval_mode::eager
                   ,  X
                   ,  s_expr< Xs... >
                   >
        :   with_result<  s_expr<  typename eval_< eval_mode::eager,  X  >::type,  Xs... >  >
        { };



        template<  typename     X
                ,  typename...  Xs
                >
        struct Cons<  eval_mode::eager
                   ,  X
                   ,  lazy_list< Xs... >
                   >
        :   with_result<  s_expr<  typename eval_< eval_mode::eager,  X  >::type
                                ,  typename eval_< eval_mode::eager,  Xs  >::type... 
                                >
                       >
        { };




        template<  eval_mode  mode
                ,  typename   X
                ,  typename   Xs
                >
        struct call<  mode,  cons,  X,  Xs  >
        :   Cons<  mode
                ,  X
                ,  typename eval_<  eval_mode::lazy,  Xs  >::type
                >
        { };




//  ----------------------------------------------------------------------------
//  eq:
//  ----------------------------------------------------------------------------


        template<  typename X,  typename Y  >
        struct Eq
        :   with_result<  false_  >
        { };



        template<  typename X >
        struct Eq<  X,  X  >  :   with_result<  true_  >  { };



        template<  eval_mode mode
                ,  typename  X
                ,  typename  Y
                >
        struct call<  mode,  eq,  X,  Y  >
        :   Eq<  eager_eval< X >,  eager_eval< Y >  >
        { };




//  ----------------------------------------------------------------------------
//   Navigators:
//  ----------------------------------------------------------------------------


        template<  eval_mode    mode
                ,  typename...  Entries
                ,  typename     X
                >
        struct call<  mode,  navigator< Entries... >,  X  >
        :   with_result<  typename navigator< Entries... >::
                          template lookup<  eager_eval< X >{}  >
                       >
        { };



//  ----------------------------------------------------------------------------
//  Conversion between symbolic boolean values "true"_t / "false_t and
//  the boolean values true/false from the C++-typesystem:
//  ----------------------------------------------------------------------------


        static consteval bool bool_rep(true_)  { return true;  }



        static consteval bool bool_rep(false_) { return false; }



        template< bool b >
        static consteval auto bool_rep()
        {
            if constexpr (b)
            {
                return with_result< true_ >{};
            }
            else
            {
                return with_result< false_ >{};
            }
        }

//  ----------------------------------------------------------------------------
//   irreducible:
//  ----------------------------------------------------------------------------

        template<  typename   >
        struct Irreducible
        :   with_result< true_  >
        { };



        template<  typename... Xs  >
        struct Irreducible< s_expr<  Xs...  >  >
        :   decltype(   bool_rep<  sizeof...(Xs) == 0  >()   )
        { };



        template<  typename... Xs  >
        struct Irreducible<  lazy_list< Xs... >  >
        :   decltype(   bool_rep<  sizeof...(Xs) == 0  >()  )
        { };



        template<  eval_mode  mode
                ,  typename   X
                >
        struct call< mode,  irreducible,  X  >
        :   Irreducible<  typename eval_< eval_mode::lazy, X >::type  >
        { };



//  ----------------------------------------------------------------------------
//   requires:
//  ----------------------------------------------------------------------------

        template<  typename,  eval_mode mode,  typename  >
        struct Requires
        {
           static_assert( mode != mode,
R"(

! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
!
! METAPROGRAMMING ERROR:
!
! A requirement needs to evaluate either to "true"_s or to "false"_s.
!
! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
)" );

        };



        template<  eval_mode mode,  typename X  >
        struct Requires<  true_,  mode,  X  >
        :   eval_<  mode,  X  >
        { };



        template<  eval_mode mode,  typename X  >
        struct Requires<  false_,  mode,  X  >
        {
            static_assert(  mode != mode,
R"(

! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
!
! METAPROGRAMMING ERROR:
!
! A requirement has not been met.
!
! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
)" );
        };



        template<  eval_mode  mode
                ,  typename   Cond
                ,  typename   X
                >
        struct call<  mode,  requires_,  Cond,  X  >
        :   Requires<  eager_eval< Cond >,  mode,  X  >
        { };


//  ----------------------------------------------------------------------------
//   not:
//  ----------------------------------------------------------------------------


        template<  typename B  >
        struct Not
        :   decltype(  bool_rep<  ! bool_rep( B{} )  >()  )
        { };



        template<  eval_mode  mode
                ,  typename   X
                >
        struct call< mode,  not_,  X  >
        :   Not<  eager_eval< X >  >
        { };



//  ----------------------------------------------------------------------------
//   and:
//  ----------------------------------------------------------------------------


        template<  typename B,  typename  C  >
        struct And
        :   decltype(  bool_rep<  bool_rep(B{})  &&  bool_rep(C{})  >()  )
        { };



        template<  eval_mode  mode
                ,  typename   X
                ,  typename   Y
                >
        struct call<  mode,  and_,  X,  Y  >
        :   And<  eager_eval< X >,  eager_eval< Y >  >
        { };



//  ----------------------------------------------------------------------------
//   or:
//  ----------------------------------------------------------------------------


        template<  typename B,  typename  C  >
        struct Or
        :   decltype(  bool_rep<  bool_rep(B{})  ||  bool_rep(C{})  >()  )
        { };



        template<  eval_mode mode
                ,  typename  X
                ,  typename  Y
                >
        struct call< mode,  or_,  X,  Y  >
        :   Or<  eager_eval< X >,  eager_eval< Y >  >
        { };



//  ----------------------------------------------------------------------------
//   xor:
//  ----------------------------------------------------------------------------


        template<  typename B,  typename  C  >
        struct Xor
        :   decltype(  bool_rep<  bool_rep(B{})  !=  bool_rep(C{})  >()  )
        { };



        template<  eval_mode mode
                ,  typename  X
                ,  typename  Y
                >
        struct call< mode,  xor_,  X,  Y  >
        :   Xor<  eager_eval< X >,  eager_eval< Y >  >
        { };



//  ----------------------------------------------------------------------------
//   ==, !=, <, >, <=,  >= :  C++-style comparison of values
//  ----------------------------------------------------------------------------
//
//  For CMP a comparator in the set {  ==, !=, <, >, <=,  >=  } the term
//
//      T: ( CMP  value<val_x>  value<val_y>  )
//
//  is determined by the C++-expression   E:  val_x CMP val_y.
//  If E evaluates to true (false), then T evaluates to true_ (false_).
//
//  If E is not well-formed, then the compilation will fail.
//
//
//  Examples (for ==. the other operators are similar):
//
//    ( == 3 4 )       ->  false_
//
//    ( == () () )     ->  Compilation error. s_expr<> is not an instantiation of
//                         value.
//
//    ( == true true)  ->  Compilation error.  true_t is not an instantiation of
//                         value.
//
//    Let x be an expression that evaluates to value<2.9>, whereas y shall
//    evaluate to value<3>.  Then
//
//    ( == x  3)       ->  false.
//
//  ----------------------------------------------------------------------------


        template<  typename,  typename,  typename  >
        struct cmp_op;



        template<  auto x,  auto y  >
        struct cmp_op<  value_type<"=="_t>
                     ,   value< x >
                     ,   value< y >
                     >
        :   decltype(  bool_rep< x == y >()  )
        { };



        template<  auto x,  auto y  >
        struct cmp_op<  value_type<"!="_t>
                     ,   value< x >
                     ,   value< y >
                     >
        :   decltype(  bool_rep< x != y >()  )
        { };



        template<  auto x,  auto y  >
        struct cmp_op<  value_type<"<"_t>
                     ,   value< x >
                     ,   value< y >
                     >
        :   decltype(  bool_rep< (x < y) >()  )
        { };



        template<  auto x,  auto y  >
        struct cmp_op<  value_type<"<="_t>
                     ,   value< x >
                     ,   value< y >
                     >
        :   decltype(  bool_rep< (x <= y) >()  )
        { };



        template<  auto x,  auto y  >
        struct cmp_op<  value_type<">"_t>
                     ,   value< x >
                     ,   value< y >
                     >
        :   decltype(  bool_rep< (x > y) >()  )
        { };



        template<  auto x,  auto y  >
        struct cmp_op<  value_type<">="_t>
                     ,   value< x >
                     ,   value< y >
                     >
        :   decltype(  bool_rep< (x >= y) >()  )
        { };



        template<  char...   cmp
                ,  typename  X
                ,  typename  Y
                ,  eval_mode mode
                >
        requires
        (
            text< cmp... >{}  ==  "=="_t ||
            text< cmp... >{}  ==  "!="_t ||
            text< cmp... >{}  ==  "<"_t  ||
            text< cmp... >{}  ==  "<="_t ||
            text< cmp... >{}  ==  ">"_t  ||
            text< cmp... >{}  ==  ">="_t
        )
        struct call<  mode
                   ,  text< cmp... >
                   ,  X
                   ,  Y
                   >
        :   cmp_op<  text< cmp... >,  eager_eval< X >,  eager_eval< Y > >
        { };


//  ----------------------------------------------------------------------------
//   +, -, *, /, %:  C++-style arithmetic operators
//  ----------------------------------------------------------------------------
//
//  For OP an operator in the set {  +, -, *, /, %   } the term
//
//      T: ( OP  value<val_x>  value<val_y>  )
//
//  evaluates to  T  :=:   value<  val_x OP val_y >
//
//  If the C++-expression "val_X OP val_Y" is not well-formed, then the
//  compilation of the program fails.
//
//  ----------------------------------------------------------------------------

        template<  typename,  typename,  typename  >
        struct Ar_Op;



        template<  auto x
                ,  auto y
                >
        struct Ar_Op<  value_type<"+"_t>
                    ,  value< x >
                    ,  value< y >
                    >
        :   with_result<  value< x + y >  >
        { };



        template<  auto x
                ,  auto y
                >
        struct Ar_Op<  value_type<"-"_t>
                    ,  value< x >
                    ,  value< y >
                    >
        :   with_result<  value< x - y >  >
        { };



       template<  auto x
                ,  auto y
                >
        struct Ar_Op<  value_type<"*"_t>
                    ,  value< x >
                    ,  value< y >
                    >
        :   with_result<  value< x * y >  >
        { };



        template<  auto x
                ,  auto y
                >
        struct Ar_Op<  value_type<"/"_t>
                    ,  value< x >
                    ,  value< y >
                    >
        :   with_result<  value< x / y >  >
        { };



        template<  auto x
                ,  auto y
                >
        struct Ar_Op<  value_type<"%"_t>
                    ,  value< x >
                    ,  value< y >
                    >
        :   with_result<  value< x % y >  >
        { };



        template<  eval_mode mode
                ,  char...   ar_op
                ,  typename  X
                ,  typename  Y
                >
        requires
        (
            text< ar_op... >{} == "+"_t  ||
            text< ar_op... >{} == "-"_t  ||
            text< ar_op... >{} == "*"_t  ||
            text< ar_op... >{} == "/"_t  ||
            text< ar_op... >{} == "%"_t
        )
        struct call<  mode
                   ,  text< ar_op... >
                   ,  X
                   ,  Y
                   >
        : Ar_Op<  text< ar_op... >,  eager_eval< X >,  eager_eval< Y >  >
        { };


// -----------------------------------------------------------------------------
//  if:
// -----------------------------------------------------------------------------


        template<  eval_mode,  typename,  typename,  typename  >
        struct If;



        template<  eval_mode mode
                ,  typename  unevaluated_X
                ,  typename  unevaluated_Y
                >
        struct If<  mode,  true_,  unevaluated_X,  unevaluated_Y  >
        :   eval_<  mode,  unevaluated_X  >
        { };



        template<  eval_mode  mode
                ,  typename   unevaluated_X
                ,  typename   unevaluated_Y
                >
        struct If<  mode,  false_,  unevaluated_X,  unevaluated_Y  >
        :   eval_<  mode,  unevaluated_Y  >
        { };



        template<  eval_mode  mode
                ,  typename   Cond
                ,  typename   X
                ,  typename   Y
                >
        struct call<  mode,  if_,  Cond,  X,  Y  >
        :   If<  mode
              ,  eager_eval< Cond  >
              ,  X
              ,  Y
              >
        { };



// -----------------------------------------------------------------------------
//  eval:
// -----------------------------------------------------------------------------


        template<  eval_mode    mode
                ,  typename     X
                >
        struct call<  mode,  evaluate,  X  >
        :   eval_<  mode,  eager_eval<  X  >  >
        { };



// -----------------------------------------------------------------------------
//  apply:
// -----------------------------------------------------------------------------


        template<  eval_mode,  typename,  typename  >
        struct Apply;


        template<  eval_mode    mode
                ,  typename     F
                ,  typename...  Xs
                >
        struct Apply<  mode,  F,  s_expr<  Xs... >  >
        :   eval_<  mode,  s_expr<  F,  quoted<Xs>... >  >
        { };



        template<  eval_mode    mode
                ,  typename     X
                ,  typename     Y
                >
        struct call<  mode
                   ,  apply
                   ,  X
                   ,  Y
                   >
        :   Apply<  mode
                 ,  eager_eval< X >
                 ,  eager_eval< Y >
                 >
        { };



// -----------------------------------------------------------------------------
//  def:
// -----------------------------------------------------------------------------


//  replace a term U with a term V in an expression:

        template<  typename
                ,  typename
                ,  typename Expr
                >
        struct subst_
        :   with_result<  Expr  >
        { };



        template<  typename U
                ,  typename V
                >
        struct subst_<  U,  V,  U  >
        :   with_result< V >
        { };



        template<  typename                        U
                ,  typename                        V
                ,  typename...                     Xs
                ,  template<  typename... > class  F
                >
        struct subst_< U,  V,  F<  Xs...  >  >
        :   with_result<  F<  typename subst_< U, V, Xs >::type...  >  >
        { };



        template<  typename U
                ,  typename V
                ,  typename X
                >
        struct subst_<  U,  V,  quoted< X >  >
        :   with_result<  quoted<  typename subst_<  U,  V,  X  >::type  >  >
        { };


        template<  typename U,  typename V,  typename Expr  >
        using subst = typename subst_<  U,  V,  Expr  >::type;



//  fix name collisions:

        template<  typename,  typename   >
        struct fix_name_collisions;


        template<  char... F_Name
                ,  typename F_Term
                >
        struct fix_name_collisions<  text< F_Name... >
                                  ,  F_Term
                                  >
        :   with_result< F_Term >
        { };



        template<  char...      F_Name
                ,  typename...  Xs
                >
        struct fix_name_collisions<  text< F_Name... >
                                  ,  s_expr<  Xs... >
                                  >
        :   with_result<  s_expr<  typename fix_name_collisions<  text< F_Name... >,  Xs  >::type...  >  >
        { };



        template<  char...      F_Name
                ,  typename     F_Term
                ,  typename     X
                ,  typename...  Xs
                >
        struct fix_name_collisions<  text< F_Name... >
                                  ,  s_expr< def,  text< F_Name... >,  F_Term,  X,  Xs...  >
                                  >
        :   with_result<  s_expr<  def
                                ,  text<'@', F_Name... >
                                ,  subst<  text< F_Name... >,  text< '@',  F_Name...  >,  F_Term  >
                                ,  subst<  text< F_Name... >,  text< '@',  F_Name...  >,  X  >
                                ,  typename fix_name_collisions<  text< F_Name... >,  Xs  >::type...
                                >
                       >
        { };



        template<  char...      F_Name
                ,  typename     F_Term
                ,  typename     X
                ,  typename...  Xs
                >
        struct fix_name_collisions<  text< F_Name... >
                                  ,  s_expr< s_expr< def,  text< F_Name... >,  F_Term  >,  X,  Xs...  >
                                  >
        :   with_result<  s_expr<  def
                                ,  text<'@', F_Name... >
                                ,  subst<  text< F_Name... >,  text< '@',  F_Name...  >,  F_Term  >
                                ,  subst<  text< F_Name... >,  text< '@',  F_Name...  >,  X  >
                                ,  typename fix_name_collisions<  text< F_Name... >,  Xs  >::type...
                                >
                       >
        { };



        template<  char...      F_Name
                ,  typename     F_Term
                ,  typename     X
                ,  typename...  Xs
                >
        struct fix_name_collisions<  text< F_Name... >
                                  ,  s_expr< s_expr<  s_expr<  def,  text< F_Name... >  >,  F_Term  >,  X,  Xs...  >
                                  >
        :   with_result<  s_expr<  def
                                ,  text<'@', F_Name... >
                                ,  subst<  text< F_Name... >,  text< '@',  F_Name...  >,  F_Term  >
                                ,  subst<  text< F_Name... >,  text< '@',  F_Name...  >,  X  >
                                ,  typename fix_name_collisions<  text< F_Name... >,  Xs  >::type...
                                >
                       >
        { };



// Fixed-point combinator Y:


        template<  eval_mode  mode
                ,  typename   F
                ,  typename   X
                >
        struct  call<  mode
                    ,  Y_combinator
                    ,  F
                    ,  X
                    >
        :   eval_<  mode,  s_expr<  F,  s_expr< Y_combinator, F >,  X  >  >
        { };



        template<  typename F_Name
                ,  typename F_Term
                ,  typename F_  =  typename application< F_Name >::
                                   template apply_to< F_Term >
                >
        struct resolve_recursion
        :   with_result<  s_expr<  F_,  s_expr< Y_combinator,  F_  >  >  >
        { };



// Define a new symbol:


        struct no_name_collisions
        {
            template<  eval_mode,  typename,  typename,  typename  >
            struct Def;


            template<  eval_mode  mode
                    ,  char...    F_Name
                    ,  typename   F_Term
                    ,  typename   Next
                    >
            struct Def<  mode
                      ,  text< F_Name... >
                      ,  F_Term
                      ,  Next
                      >
            :   eval_<  mode,  subst<  text< F_Name... >
                                    ,  typename resolve_recursion<  text< F_Name... >,  F_Term  >::type
                                    ,  Next >
                     >
            { };
        };



        template<  eval_mode,  typename,  typename,  typename  >
        struct Def;



        template<  eval_mode    mode
                ,  char...      F_Name
                ,  typename     F_Term
                ,  typename     Next
                >
        struct Def<  mode
                  ,  text< F_Name... >
                  ,  F_Term
                  ,  Next
                  >
        :   no_name_collisions::Def<  mode
                                       ,  text< F_Name... >
                                       ,  typename fix_name_collisions< text< F_Name... >,  F_Term  >::type
                                       ,  typename fix_name_collisions< text< F_Name... >,  Next  >::type
                                       >
        { };



        template<  eval_mode  mode
                ,  char...    F_Name
                ,  typename   F_Term
                ,  typename   Next
                >
        struct call<  mode
                   ,  def
                   ,  text< F_Name... >
                   ,  F_Term
                   ,  Next
                   >
        :   Def<  mode
               ,  text< F_Name... >
               ,  eager_eval< F_Term >
               ,  Next
               >
        { };



// -----------------------------------------------------------------------------
//  K:  KXY = X
// -----------------------------------------------------------------------------


        template<  eval_mode mode
                ,  typename  X
                ,  typename  Y
                >
        struct call<  mode
                   ,  K
                   ,  X
                   ,  Y
                   >
        :   eval_<  mode,  X  >
        { };



// -----------------------------------------------------------------------------
//  S:  SXYZ = XZ(YZ)
// -----------------------------------------------------------------------------

        template<  eval_mode    mode
                ,  typename     X
                ,  typename     Y
                ,  typename     Z
                >
        struct call<  mode
                   ,  S
                   ,  X
                   ,  Y
                   ,  Z
                   >
        :   eval_<  mode
                 ,  s_expr<  X,  Z,  s_expr<  Y,  Z  >  >
                 >
       { };



// -----------------------------------------------------------------------------
//  combine programs:
// -----------------------------------------------------------------------------


        template<  typename... >
        struct combine_programs;



        template<  typename P  >
        struct combine_programs<  P  >
        :   with_result< P >
        { };



        template<  typename P
                ,  typename Q
                >
        struct combine_programs<  P,  Q  >
        :   with_result<  s_expr< P, Q >  >
        { };



        template<  typename     P
                ,  typename     Q
                ,  typename...  Qs
                >
        struct combine_programs<  P,  Q,  Qs... >
        :   with_result<  s_expr< P,  typename combine_programs< Q, Qs... >::type  >  >
        { };



        template<  typename...  >
        friend struct interpreter;



        template<  typename... >
        struct Eval_;



        template<  typename P  >
        struct Eval_< P >
        :   eval_<  eval_mode::eager,  P  >
        { };



        template<  typename P,  typename N  >
        struct Eval_<  P,  N  >
        :   eval_<  eval_mode::eager,  s_expr< value_type<"def"_t>,  value_type<"`"_s>,  N, P >  >
        { };


// -----------------------------------------------------------------------------
// The interpreter call::
// -----------------------------------------------------------------------------

    public:


        template<  auto const program,  auto const... navigator  >
        requires
        (
            sizeof...(navigator) <= 1
        )
        using eval  = typename Eval_<  value_type< program >,  value_type<navigator>...  >::type;



        template<  auto const  >
        inline static constexpr auto dependencies = tuple<>{};



        inline static consteval auto set_dependencies(  auto const... dep )
        ->  tuple<  value_type< decltype(dep){} >...  >
        {
            return {};
        }



        template<  auto const...  >
        struct with;
    };
}
