/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "symbolic_expressions.hpp"
#include "map.hpp"
#include "type_hull.hpp"
#include "symbolic_type_system.hpp"



namespace lt
{
    template<  auto const symbols  = map{}  >
    struct interpreter;



    template<  literal_or_s...   >
    struct with;



    template<  typename... Error  >
    requires( sizeof...(Error) <= 1  )
    struct error { };



    template<>
    struct error<>
    {
    private:

        template<  auto const >
        friend struct interpreter;

        template<  typename...  > struct evaluation_failure_ {};
    };



    template<  typename...  >
    struct evaluation_failure {};



    template<>
    struct interpreter< map{}  >
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


        template<  typename... Xs  >
        using evaluation_failure_ = typename error<>::evaluation_failure_< Xs... >;


        static consteval bool is_possibly_evaluable_(...) { return true; }



        template<  typename... Xs  >
        static consteval bool is_possibly_evaluable_(type_hull<evaluation_failure_<Xs...>>) { return false; }



        template<  typename T  >
        static inline constexpr bool is_possibly_evaluable = is_possibly_evaluable_( type_hull<T>{} );



        template<  auto op,  template<  typename... > class F,  typename... Xs  >
        struct gather
        :   with_result<  evaluation_failure_<  value_type<op>,  Xs... >  >
        { };



        template<  auto                            op
                ,  template<  typename... > class  F
                ,  typename...                     Xs
                >
        requires
        (
            (   requires( F<Xs...> ) { nullptr; }
                && ... &&
                is_possibly_evaluable<Xs>
            )
        )
        struct gather<  op,  F,  Xs... >
        :   with_result< F< Xs...>  >
        { };




        struct Y_combinator {};



// -----------------------------------------------------------------------------
//
// Keywords and Constants
//
// -----------------------------------------------------------------------------


// elementary functions (except quote):

        using  apply        =  value_type<"apply"_text>;
        using  and_         =  value_type<"and"_text>;
        using  cpp          =  value_type<"C++"_text>;
        using  cons         =  value_type<"cons"_text>;
        using  def          =  value_type<"def"_text>;
        using  drop_first   =  value_type<"drop_first"_text>;
        using  eval_fail    =  value_type<"evaluation_failure"_text>;
        using  evaluate     =  value_type<"eval"_text>;
        using  eq           =  value_type<"eq"_text>;
        using  first        =  value_type<"first"_text>;
        using  if_          =  value_type<"if"_text>;
        using  if_possible  =  value_type<"if_possible"_text>;
        using  irreducible  =  value_type<"irreducible"_text>;
        using  not_         =  value_type<"not"_text>;
        using  or_          =  value_type<"or"_text>;
        using  requires_    =  value_type<"requires"_text>;
        using  show_error   =  value_type<"show_error"_text>;
        using  symbolic     =  value_type<"Symbolic"_text>;
        using  xor_         =  value_type<"xor"_text>;
        using  true_        =  value_type<"true"_text>;
        using  false_       =  value_type<"false"_text>;


// -----------------------------------------------------------------------------
//
// Arities
//
// -----------------------------------------------------------------------------

        static consteval int  arity_(...) { return -1; }



        template<  auto N  >
        static consteval int  arity_( type_hull<list<N>>)   { return N; }



        template<  auto N
                ,  template< typename... > class F
                >
        static consteval int  arity_(type_hull<combinator< N, F >>) { return N; }



        template<  typename... Entries  >
        static consteval int  arity_(type_hull<map< Entries... >>) { return 1; }


        template<  typename... Xs  >
        static consteval int  arity_(type_hull< evaluation_failure_< Xs... >  >) { return 1; }



        static consteval int  arity_(type_hull<true_>)         { return 0; }
        static consteval int  arity_(type_hull<false_>)        { return 0; }
        static consteval int  arity_(type_hull<s<>>)           { return 0; }


        static consteval int  arity_(type_hull<cpp>)            { return 1; }
        static consteval int  arity_(type_hull<drop_first>)     { return 1; }
        static consteval int  arity_(type_hull<eval_fail>)      { return 1; }
        static consteval int  arity_(type_hull<evaluate>)       { return 1; }
        static consteval int  arity_(type_hull<first>)          { return 1; }
        static consteval int  arity_(type_hull<irreducible>)    { return 1; }
        static consteval int  arity_(type_hull<not_>)           { return 1; }
        static consteval int  arity_(type_hull<show_error>)     { return 1; }
        static consteval int  arity_(type_hull<symbolic>)       { return 1; }

        static consteval int  arity_(type_hull<apply>)          { return 2; }
        static consteval int  arity_(type_hull<and_>)           { return 2; }
        static consteval int  arity_(type_hull<cons>)           { return 2; }
        static consteval int  arity_(type_hull<eq>)             { return 2; }
        static consteval int  arity_(type_hull<if_possible>)    { return 2; }
        static consteval int  arity_(type_hull<K>)              { return 2; }
        static consteval int  arity_(type_hull<requires_>)      { return 2; }
        static consteval int  arity_(type_hull<or_>)            { return 2; }
        static consteval int  arity_(type_hull<xor_>)           { return 2; }
        static consteval int  arity_(type_hull<Y_combinator>)   { return 2; }
        static consteval int  arity_(type_hull<op<"=="_text>>)  { return 2; }
        static consteval int  arity_(type_hull<op<"!="_text>>)  { return 2; }
        static consteval int  arity_(type_hull<op<"<"_text>>)   { return 2; }
        static consteval int  arity_(type_hull<op<"<="_text>>)  { return 2; }
        static consteval int  arity_(type_hull<op<">"_text>>)   { return 2; }
        static consteval int  arity_(type_hull<op<">="_text>>)  { return 2; }
        static consteval int  arity_(type_hull<op<"+"_text>>)   { return 2; }
        static consteval int  arity_(type_hull<op<"-"_text>>)   { return 2; }
        static consteval int  arity_(type_hull<op<"*"_text>>)   { return 2; }
        static consteval int  arity_(type_hull<op<"/"_text>>)   { return 2; }
        static consteval int  arity_(type_hull<op<"%"_text>>)   { return 2; }


        static consteval int  arity_(type_hull<if_>)           { return 3; }
        static consteval int  arity_(type_hull<S>)             { return 3; }
        static consteval int  arity_(type_hull<def>)           { return 3; }


        template<  typename T  >
        static inline constexpr int arity = arity_(type_hull<T>{});



// -----------------------------------------------------------------------------
//  evaluation_failure expressions:
// -----------------------------------------------------------------------------


        template<  eval_mode    mode
                ,  typename...  Xs
                ,  typename     Ignore
                >
        struct call<  mode,  evaluation_failure_< Xs... >,  Ignore  >
        :   with_result<  evaluation_failure_< Xs... >  >
        { };


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
            arity<T> >= 0
        )
        struct eval_<  mode,  T  >
        :   with_result<  T  >
        { };



// -----------------------------------------------------------------------------
//  evaluation_failure atoms (i e anything with arity < 0 ):
// -----------------------------------------------------------------------------


        template<  eval_mode  mode,  typename T  >
        requires
        (
            arity<T> < 0
        )
        struct eval_<  mode,  T  >
        :   with_result<  evaluation_failure_<T>  >
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
        :   with_result<  value<true> >
        { };



        template<  eval_mode mode >
        struct eval_<  mode,  false_  >
        :   with_result<  value<false> >
        { };



        template<  eval_mode mode >
        struct eval_<  mode,  s<>  >
        :   with_result<  s<>  >
        { };



// -----------------------------------------------------------------------------
//  Empty lists:
// -----------------------------------------------------------------------------


        template<  eval_mode mode >
        struct eval_<  mode,  s< list<0> >  >
        :   with_result<  s<>  >
        { };



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
            int{1 + sizeof...(Xs)}  <  arity<Op>
        )
        struct eval_<  mode
                    ,  s<  Op,  X,  Xs...  >
                    >
        :   with_result<  s< Op,  X,  Xs...  >  >
        {};


// Call of an undefined function: Create an error message.

        template<  eval_mode    mode
                ,  typename     Op
                ,  typename     X
                ,  typename...  Xs
                >
        requires
        (
            arity<Op> < 0
        )
        struct eval_<  mode
                    ,  s<  Op,  X,  Xs...  >
                    >
        :   with_result< evaluation_failure_<Op,  X,  Xs... >  >
        { };



// Expression ready for evaluation:

        template<  eval_mode    mode
                ,  typename     Op
                ,  typename     X
                ,  typename...  Xs
                >
        struct eval_<  mode
                    ,  s<  Op,  X,  Xs...  >
                    >
        :   call<  mode,  Op,  X,  Xs...  >
        {
            static_assert(  1 + sizeof...(Xs) == arity<Op>   );
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
            arity<Op> > 0
            &&
            2 + sizeof...(Ys)  > arity<Op>
        )
        struct eval_<  mode
                    ,  s<  Op,  X,  Y,  Ys...  >
                    >
        :   eval_<  mode
                 ,  s<  eager_eval< s< Op,  X  >  >
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
            arity<Op> > 0
            &&
            2 + sizeof...(Xs)  <=  arity<Op>
        )
        struct eval_<  mode
                    ,  s<  s<  Op,  X,  Xs...  >,  Y  >
                    >
        :   eval_<  mode
                 ,  s<  Op,  X,  Xs... ,  Y  >
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
            arity<Op> > 0
            &&
            2 + sizeof...(Xs)  <=  arity<Op>
        )
        struct eval_<  mode
                    ,  s<  s<  Op,  X,  Xs...  >,  Y,  Z,  Zs...  >
                    >
        :   eval_<  mode
                 ,  s<  eager_eval<  s<  Op,  X,  Xs...,  Y  >  >
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
                    ,  s<  s<  X,  Xs... >,  Y,  Ys...  >
                    >
        :   eval_<  mode
                ,  s<  X,  Xs...,  Y,  Ys...  >
                >
        { };





//  ----------------------------------------------------------------------------
//  list:
//  ----------------------------------------------------------------------------

        template<  typename... Xs  >
        struct lazy_list {};



        template<  typename... Xs  >
        struct eval_<  eval_mode::eager,  lazy_list<  Xs...  >  >
        :   gather<  "list"_text,  s,   typename eval_<  eval_mode::eager,  Xs  >::type...  >
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
        :   gather<  "list"_text,  s,  typename eval_<  eval_mode::eager,  Xs  >::type...  >
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
//    :   with_result<  F<  eager_eval<Xs>...  >  >
    :   gather< combinator< N, F >{},  F,  eager_eval<Xs>... >
    { };


// -----------------------------------------------------------------------------
//  Symbolic and C++:  (Symbolic type system)
// -----------------------------------------------------------------------------


        template<  eval_mode   mode
                ,  typename    X
                >
        struct call<  mode,  symbolic,  X  >
        :   gather<  "ts::symbolic"_text,  ts::symbolic,  eager_eval<X>  >
        { };



        template<  eval_mode   mode
                ,  typename    X
                >
        struct call<  mode,  cpp,  X  >
        :   gather<  "ts::cpp"_text,  ts::cpp,  eager_eval<X>  >
        { };



//  ----------------------------------------------------------------------------
//  first:
//  ----------------------------------------------------------------------------


        template<  eval_mode,  typename T >
        struct First
        :   with_result<  evaluation_failure_< value_type<"first"_text>,  T  >  >
        { };



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
                   ,  s<  X,  Xs...  >
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


        template<  eval_mode,  typename T  >
        struct Drop_First
        :   with_result<  evaluation_failure_<  value_type<"drop_first"_text>,  T  >  >
        { };



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
                        ,  s<  X,  Xs...  >
                        >
        :   with_result<  s< Xs... >  >
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

        template<  eval_mode,  typename X,  typename  Y>
        struct Cons
        :   with_result<  evaluation_failure_< value_type<"cons"_text>,  X,  Y  >  >
        { };



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
                   ,  s<  Xs...  >
                   >
        :   with_result<  lazy_list<  X,  quoted<Xs>...  >  >
        { };



        template<  typename     X
                ,  typename...  Xs
                >
        struct Cons<  eval_mode::eager
                   ,  X
                   ,  s< Xs... >
                   >
        :   with_result<  s<  typename eval_< eval_mode::eager,  X  >::type,  Xs... >  >
        { };



        template<  typename     X
                ,  typename...  Xs
                >
        struct Cons<  eval_mode::eager
                   ,  X
                   ,  lazy_list< Xs... >
                   >
        :   with_result<  s<  typename eval_< eval_mode::eager,  X  >::type
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
        :   with_result<  value<false>  >
        { };



        template<  typename X >
        struct Eq<  X,  X  >
        :   with_result<  value<true>  >
        { };



        template<  typename X,  typename Y  >
        requires
        (
            ! is_possibly_evaluable<X> ||
            ! is_possibly_evaluable<Y>
        )
        struct Eq< X,  Y  >
        :   with_result<  evaluation_failure_< eq,  X,  Y  >  >
        { };



        template<  typename X  >
        requires
        (
            ! is_possibly_evaluable<X>
        )
        struct Eq< X,  X  >
        :   with_result<  evaluation_failure_<  eq,  X,  X  >  >
        { };



        template<  eval_mode mode
                ,  typename  X
                ,  typename  Y
                >
        struct call<  mode,  eq,  X,  Y  >
        :   Eq<  eager_eval< X >,  eager_eval< Y >  >
        { };




//  ----------------------------------------------------------------------------
//   maps:
//  ----------------------------------------------------------------------------


        template<  eval_mode    mode
                ,  typename...  Entries
                ,  typename     X
                >
        struct call<  mode,  map< Entries... >,  X  >
        :   with_result<  typename map< Entries... >::
                          template lookup<  eager_eval< X >
                                         ,  evaluation_failure_<  map<Entries...>,  X  >
                                         >
                       >
        { };



//  ----------------------------------------------------------------------------
//   irreducible:
//  ----------------------------------------------------------------------------


        template<  typename   >
        struct Irreducible
        :   with_result< value<true>  >
        { };



        template<  typename  X  >
        struct Irreducible<  evaluation_failure_<X>  >
        :   with_result<  evaluation_failure_<  irreducible,  evaluation_failure_<X>  >  >
        { };


        template<  typename... Xs  >
        struct Irreducible< s<  Xs...  >  >
        :   with_result<  value<sizeof...(Xs) == 0 >  >
        { };



        template<  typename... Xs  >
        struct Irreducible<  lazy_list< Xs... >  >
        :   with_result<  value< sizeof...(Xs) == 0 >  >
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

        template<  eval_mode mode,  typename Cond,  typename X >
        struct Requires
        :   with_result<  evaluation_failure_<  requires_,  Cond,  X  >  >
        { };



        template<  eval_mode mode,  typename X  >
        struct Requires<  mode,  value<true>,  X  >
        :   eval_<  mode,  X  >
        { };



        template<  eval_mode  mode
                ,  typename   Cond
                ,  typename   X
                >
        struct call<  mode,  requires_,  Cond,  X  >
        :   Requires<  mode,  eager_eval< Cond >,  X  >
        { };


//  ----------------------------------------------------------------------------
//   not:
//  ----------------------------------------------------------------------------


        template<  typename B  >
        struct Not
        :   with_result<  evaluation_failure_<  not_, B >  >
        { };



        template<  bool b >
        struct Not<  value<b> >
        :   with_result<  value< !b >  >
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
        :   with_result<  evaluation_failure_<  and_, B, C >  >
        { };



        template<  bool a,  bool b  >
        struct And<  value<a>,  value<b>  >
        :   with_result<  value< a && b >  >
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
        :   with_result<  evaluation_failure_<  or_, B, C >  >
        { };





        template<  bool a,  bool b  >
        struct Or<  value<a>,  value<b>  >
        :   with_result<  value< a || b >  >
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
        :   with_result<  evaluation_failure_<  xor_, B, C >  >
        { };




        template<  bool a,  bool b  >
        struct Xor<  value<a>,  value<b>  >
        :   with_result<  value< ! (a == b)  >  >
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
//  If E evaluates to true (false), then T evaluates to value<true> (value<false>).
//
//  If E is not well-formed, then the compilation will fail.
//
////
//  ----------------------------------------------------------------------------


        template<  typename Op,  typename X,  typename  Y>
        struct cmp_op
        :   with_result<  evaluation_failure_<  value_type<"::comparison operation:: "_text>,  Op,  X,  Y  >  >
        { };



        template<  auto x,  auto y  >
        requires
        requires( value< bool{ x == y } > )
        {
            nullptr;
        }
        struct cmp_op<  value_type<"=="_text>
                     ,   value< x >
                     ,   value< y >
                     >
        :   with_result< value< bool{ x == y } > >
        { };



        template<  auto x,  auto y  >
        requires
        requires( value< bool{ x != y } > )
        {
            nullptr;
        }
        struct cmp_op<  value_type<"!="_text>
                     ,   value< x >
                     ,   value< y >
                     >
        :   with_result< value< bool{ x !=y } > >
        { };



        template<  auto x,  auto y  >
        requires
        requires( value< bool{ (x < y) } > )
        {
            nullptr;
        }
        struct cmp_op<  value_type<"<"_text>
                     ,   value< x >
                     ,   value< y >
                     >
        :   with_result< value< ( bool{ x < y } ) > >
        { };



        template<  auto x,  auto y  >
        requires
        requires( value< bool{ (x <= y) } > )
        {
            nullptr;
        }
        struct cmp_op<  value_type<"<="_text>
                     ,   value< x >
                     ,   value< y >
                     >
        :   with_result< value< ( bool{ x <= y } ) > >
        { };



        template<  auto x,  auto y  >
        requires
        requires( value< bool{ (x > y) } > )
        {
            nullptr;
        }
        struct cmp_op<  value_type<">"_text>
                     ,   value< x >
                     ,   value< y >
                     >
        :   with_result< value< ( bool{ x > y } ) > >
        { };



        template<  auto x,  auto y  >
        requires
        requires( value< bool{ (x >= y) } > )
        {
            nullptr;
        }
        struct cmp_op<  value_type<">="_text>
                     ,   value< x >
                     ,   value< y >
                     >
        :   with_result< value< ( bool{ x >= y } ) > >

        { };



        template<  char...   cmp
                ,  typename  X
                ,  typename  Y
                ,  eval_mode mode
                >
        requires
        (
            text< cmp... >{}  ==  "=="_text ||
            text< cmp... >{}  ==  "!="_text ||
            text< cmp... >{}  ==  "<"_text  ||
            text< cmp... >{}  ==  "<="_text ||
            text< cmp... >{}  ==  ">"_text  ||
            text< cmp... >{}  ==  ">="_text
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

        template<  typename Op,  typename X,  typename Y  >
        struct Ar_Op
        :   with_result<  evaluation_failure_<  value_type<"::arithmetic operation::"_text>,  Op,  X,  Y  >  >
        { };


        template<  auto x
                ,  auto y
                >
        requires
        requires(  value< x + y >  )
        {
            nullptr;
        }
        struct Ar_Op<  value_type<"+"_text>
                    ,  value< x >
                    ,  value< y >
                    >
        :   with_result<  value< x + y >  >
        { };



        template<  auto x
                ,  auto y
                >
        requires
        requires(  value< x - y >  )
        {
            nullptr;
        }
        struct Ar_Op<  value_type<"-"_text>
                    ,  value< x >
                    ,  value< y >
                    >
        :   with_result<  value< x - y >  >
        { };



       template<  auto x
                ,  auto y
                >
        requires
        requires(  value< x * y >  )
        {
            nullptr;
        }
        struct Ar_Op<  value_type<"*"_text>
                    ,  value< x >
                    ,  value< y >
                    >
        :   with_result<  value< x * y >  >
        { };



        template<  auto x
                ,  auto y
                >
        requires
        requires(  value< x / y >  )
        {
            nullptr;
        }
        struct Ar_Op<  value_type<"/"_text>
                    ,  value< x >
                    ,  value< y >
                    >
        :   with_result<  value< x / y >  >
        { };



        template<  auto x
                ,  auto y
                >
        requires
        requires(  value< x % y >  )
        {
            nullptr;
        }
        struct Ar_Op<  value_type<"%"_text>
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
            text< ar_op... >{} == "+"_text  ||
            text< ar_op... >{} == "-"_text  ||
            text< ar_op... >{} == "*"_text  ||
            text< ar_op... >{} == "/"_text  ||
            text< ar_op... >{} == "%"_text
        )
        struct call<  mode
                   ,  text< ar_op... >
                   ,  X
                   ,  Y
                   >
        :   Ar_Op<  text< ar_op... >,  eager_eval< X >,  eager_eval< Y >  >
        { };


// -----------------------------------------------------------------------------
//  if:
// -----------------------------------------------------------------------------


        template<  eval_mode,  typename Cond,  typename X,  typename Y >
        struct If
        :   with_result<  evaluation_failure_<  Cond,  X,  Y  >  >
        { };



        template<  eval_mode mode
                ,  typename  unevaluated_X
                ,  typename  unevaluated_Y
                >
        struct If<  mode,  value<true>,  unevaluated_X,  unevaluated_Y  >
        :   eval_<  mode,  unevaluated_X  >
        { };



        template<  eval_mode  mode
                ,  typename   unevaluated_X
                ,  typename   unevaluated_Y
                >
        struct If<  mode,  value<false>,  unevaluated_X,  unevaluated_Y  >
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
//  evaluation_failure:
// -----------------------------------------------------------------------------

        template<  typename  >
        struct Eval_Fail
        :   with_result<  value<false>  >
        { };



        template<  typename... Xs  >
        struct Eval_Fail<  evaluation_failure_< Xs... >  >
        :   with_result<  value<true>  >
        { };



        template<  eval_mode  mode
                ,  typename   X
                >
        struct call<  mode,  eval_fail,  X  >
        :   Eval_Fail<  eager_eval<X>  >
        { };


// -----------------------------------------------------------------------------
//  if_possible:
// -----------------------------------------------------------------------------

        template<  eval_mode
                ,  typename   P
                ,  typename
                >
        struct If_Possible
        :   with_result<  P  >
        { };



        template<  eval_mode    mode
                ,  typename...  Ps
                ,  typename     Q
                >
        struct If_Possible<  mode
                          ,  evaluation_failure_<  Ps...  >
                          ,  Q
                          >
        :   eval_<  mode,  Q  >
        { };



        template<  eval_mode  mode
                ,  typename   P
                ,  typename   Q
                >
        struct call<  mode,  if_possible,  P,  Q  >
        :   If_Possible<  mode
                       ,  eager_eval< P  >
                       ,  Q
                       >
        { };




// -----------------------------------------------------------------------------
//  show_error:
// -----------------------------------------------------------------------------

        template<  typename No_Error  >
        struct Show_Error
        :   with_result<  error<>  >
        { };



        template<  typename T  >
        struct use_outer_evaluation_failure
        :   with_result< T >
        { };



        template<  typename... Xs  >
        struct use_outer_evaluation_failure<  evaluation_failure_< Xs... >  >
        :   with_result<  evaluation_failure<  typename use_outer_evaluation_failure<Xs>::type...  >  >
        { };



        template<  typename... Xs  >
        struct Show_Error<  evaluation_failure_<  Xs... >  >
        :   with_result<  error<  typename
                                  use_outer_evaluation_failure< evaluation_failure_< Xs... >  >::type
                               >
                       >
        { };



        template<  eval_mode  mode
                ,  typename   X
                >
        struct call<  mode,  show_error,  X  >
        :   Show_Error<  eager_eval<X>  >
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


        template<  eval_mode,  typename F,  typename  Xs  >
        struct Apply
        :   with_result<  evaluation_failure_<  apply,  F,  Xs  >  >
        { };



        template<  eval_mode    mode
                ,  typename     F
                ,  typename...  Xs
                >
        struct Apply<  mode,  F,  s<  Xs... >  >
        :   eval_<  mode,  s<  F,  quoted<Xs>... >  >
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
                                  ,  s<  Xs... >
                                  >
        :   with_result<  s<  typename fix_name_collisions<  text< F_Name... >,  Xs  >::type...  >  >
        { };



        template<  char...      F_Name
                ,  typename     F_Term
                ,  typename     X
                ,  typename...  Xs
                >
        struct fix_name_collisions<  text< F_Name... >
                                  ,  s< def,  text< F_Name... >,  F_Term,  X,  Xs...  >
                                  >
        :   with_result<  s<  def
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
                                  ,  s< s< def,  text< F_Name... >,  F_Term  >,  X,  Xs...  >
                                  >
        :   with_result<  s<  def
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
                                  ,  s< s<  s<  def,  text< F_Name... >  >,  F_Term  >,  X,  Xs...  >
                                  >
        :   with_result<  s<  def
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
        :   eval_<  mode,  s<  F,  s< Y_combinator, F >,  X  >  >
        { };



        template<  typename F_Name
                ,  typename F_Term
                ,  typename F_  =  typename application< F_Name >::
                                   template apply_to< F_Term >
                >
        struct resolve_recursion
        :   with_result<  s<  F_,  s< Y_combinator,  F_  >  >  >
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
                ,  typename   F_Name
                ,  typename   F_Term
                ,  typename   Next
                >
        struct Def_
        :   public Def<  mode,  F_Name,  F_Term,  Next  >
        { };



        template<  eval_mode    mode
                ,  typename     F_Name
                ,  typename...  Xs
                ,  typename     Next
                >
        struct Def_<  mode
                   ,  F_Name
                   ,  evaluation_failure_<  Xs...  >
                   ,  Next
                   >
        :   with_result<  evaluation_failure_<  def,  F_Name,  evaluation_failure_< Xs... >,  Next  >  >
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
        :   Def_<  mode
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
                 ,  s<  X,  Z,  s<  Y,  Z  >  >
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
        :   with_result<  s< P, Q >  >
        { };



        template<  typename     P
                ,  typename     Q
                ,  typename...  Qs
                >
        struct combine_programs<  P,  Q,  Qs... >
        :   with_result<  s< P,  typename combine_programs< Q, Qs... >::type  >  >
        { };



        template<  typename,  typename  >
        struct Eval_;




        template<  typename P,  typename...  Entries  >
        struct Eval_<  P,  map< Entries... >  >
        :   eval_<  eval_mode::eager
                 ,  s<  value_type<"def"_text>,  text<'`'>,  map< Entries... >,  P  >
                 >
        {
            static_assert(  is_possibly_evaluable<
                eager_eval<  s<  value_type<"def"_text>,  text<'`'>,  map< Entries... >,  P  >  >
                                                 >,
R"(

! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
!
! METAPROGRAMMING ERROR:
!
! Expression contains at least one ubexpression that causes an
! evaluation failure.
!
! @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
)" );
        };



        struct publicly_visible
        {
            template<  literal_or_s program_literal  >
            requires
            requires()
            {
                program_literal.content;
            }
            static consteval auto parse_()
            {
                return  s_expr< static_cast< typename decltype(program_literal)::type const&>(program_literal) >{};
            }



            template<  literal_or_s program_literal  >
            static consteval auto parse_()
            ->  typename decltype(program_literal)::type
            {
                return  {};
            }

        };


        template<  auto const Map  >
        friend struct interpreter;



// -----------------------------------------------------------------------------
// The interpreter call::
// -----------------------------------------------------------------------------

    public:


        template<  literal_or_s prog  >
        using eval  =  typename Eval_<  value_type<  publicly_visible::template parse_< prog >()  >
                                     ,  value_type< map<>{} >
                                     >::type;
    };



    template<  auto const Map  >
    struct interpreter
    {
        template<  literal_or_s  prog >
        using eval  =

        typename interpreter< map<>{}  >::
        template Eval_<   value_type<  interpreter<  map<>{}  >::
                                       publicly_visible::
                                       template parse_< prog >()  >
                      ,  value_type< Map >
                      >::
        type;
    };



    template<  literal_or_s prog,  auto const symbols = map<>{}  >
    using eval  =  typename interpreter< symbols >::template eval<  prog  >;
};


namespace lt
{
    template<>
    struct with<>
    {
        template<  auto const symbols = map{}  >
        using interpreter = struct interpreter< symbols >;


        template<  literal_or_s prog,  auto const symbols = map{}  >
        using eval = typename lt::interpreter< symbols >::template eval< prog >;


    private:


        template<  literal_or_s...  >
        friend struct with;



        template<  auto const... >
        struct list_;



        template<  typename,  typename...  >
        struct gather;




        template<  auto const...    scanned
                ,  auto...          lib
                ,  typename...      Next
                >
        struct gather<  list_< scanned... >,  with< lib...  >,  Next...  >
        :   gather<  list_< scanned... >,  s_expr<lib>...,  Next...  >
        { };




        template<  auto const...  scanned
                ,  char...        f
                ,  typename       X
                ,  typename...    More
                >
        struct gather<  list_<  scanned...  >
                     ,  s<  value_type<"def"_text>,  text<f...>,  X  >
                     ,  More...
                        >
        :   gather<   list_<  scanned...
                           ,  assign< lt::text<>::literal{  text<f...>{}  }
                                    , lt::eval<  s<  value_type<"def"_text>,  text<f...>,  X,  text<f...>  >{}  >
                                    >
                            >
                  ,  More...
                  >
        { };




        template<  auto const... scanned  >
        struct gather<  list_< scanned... >  >
        {
            template<  map symbols = map{}  >
            struct interpreter
            {
                template<  literal_or_s  prog  >
                using eval =  lt::eval<  s_expr< prog,  map{ scanned... }  >{},  symbols  >;
            };


            template<  literal_or_s  prog,  map symbols = map{}  >
            using eval  =  lt::eval<  s_expr<  prog,  map{ scanned... } >{},  symbols  >;
        };
    };



    template<  literal_or_s     h
            ,  literal_or_s...  t
            >
    struct with<  h,  t...  >
    :   with<>::template gather<  typename with<>::template list_<>,  s_expr<h>,  s_expr<t>...  >
    { };



    template<  literal_or_s... components  >
    inline constexpr auto lib = with<  components...  >{};
}
