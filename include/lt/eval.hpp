/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "s_expr.hpp"
#include "map.hpp"
#include "type_hull.hpp"



namespace lt
{
    template<  typename...   >
    struct i;


    template<  typename... Def  >
    using imported = i< Def... >;



    template<  typename... Error >
    requires(  sizeof...(Error) <= 1 )
    struct debug_output {};



    struct Y {};


    template<  typename... >
    struct op {};
}



template<>
struct lt::i<>
{
private:




// The Trivium Lisp Interpreter:


    using t_true   =  type_hull< value<true> >;
    using t_false  =  type_hull< value<false> >;




// Evaluator result when evaluation is successful:


    template<  typename T >
    using eval_result = type_hull<T>;



// Evaluator and auxiliary types::



    enum class eval_mode {  lazy,  eager  };



    template<  eval_mode,  typename,  typename   >
    struct eval_;  // The evaluator



    template<  typename Lut,  typename Expr  >
    using eager_eval = typename eval_<  eval_mode::eager,  Lut,  Expr  >::type;



    template<  typename Lut,  typename Expr  >
    using lazy_eval = typename eval_<  eval_mode::lazy,  Lut,  Expr  >::type;



    template<  unsigned,  typename,  typename...  >
    struct call { };



//  Error handling:

    template< typename... Expr >
    struct eval_error
    :   type_hull< eval_error<Expr... >  >
    { };



    template< typename >
    struct no_eval_error_
    :   type_hull< t_true >
    { };



    template<  typename... Expr >
    struct no_eval_error_< eval_error<Expr...> >
    :   type_hull< t_false >
    { };



    template<  typename Expr  >
    using no_eval_error = typename no_eval_error_<Expr>::type;



//  gather:
//
//  Intantiate a class template F with parameters Xs... whenever
//  none of the parameters Xs... is an instance of  eval_error.
//
//  If the instantation is not possible, or if (at least) one ofr the
//  parameters Xs... is an instance of eval_error, then return the
//  type
//          eval_error< class_template<f>, Xs... >.

    template<  template<  typename...  > class F
            ,  typename...                     Xs
            >
    struct gather
    :   eval_error<  class_template<F>,  Xs...  >
    { };



    template<  template<  typename...  > class   F
            ,  typename...                       Xs
            >
    requires
    (
        (  ( no_eval_error<Xs>{} == t_true{} )  && ...  &&  requires(  F< Xs... > ) { nullptr; }  )
    )
    struct gather<  F,  Xs... >
    :   type_hull<  F< Xs... >  >
    { };


// Constrained expression


    template<  template< typename >  class  Constraint
            ,  typename                     Expr
            ,  bool                     =   Constraint<Expr>::value
            >
    struct apply_constraint_
    :   eval_error<  Expr  >
    { };



    template<  template< typename > class  Constraint
            ,  typename                    Expr
            >
    struct apply_constraint_<  Constraint
                            ,  Expr
                            ,  true
                            >
    :   type_hull<  Expr  >
    { };


    template<  template< typename > class Constraint,  typename Expr  >
    using apply_constraint = typename apply_constraint_<  Constraint,  Expr  >::type;



// Helper constraints:

    template<  typename  >
    struct arg_is_boolean
    {
        enum { value = false };
    };



    template<  typename Op,  bool b  >
    struct arg_is_boolean<  op<  Op,  value<b>  >  >
    {
        enum { value = true };
    };



    template<  typename  >
    struct arg_is_value
    {
        enum { value = false };
    };



    template<  typename Op,  auto v  >
    struct arg_is_value<  op<  Op,  value<v>  >  >
    {
        enum { value = true };
    };


// -----------------------------------------------------------------------------
//
// Keywords and Constants
//
// -----------------------------------------------------------------------------


    using Y_combinator = Y;




// elementary functions (except quote):

    using  apply_t         =  value_type<"apply"_text>;
    using  and_t           =  value_type<"and"_text>;
    using  cons_t          =  value_type<"cons"_text>;
    using  count_t         =  value_type<"count"_text>;
    using  def_t           =  value_type<"def"_text>;
    using  drop_first_t    =  value_type<"drop_first"_text>;
    using  eval_success_t  =  value_type<"eval_success"_text>;
    using  eval_t          =  value_type<"eval"_text>;
    using  eq_t            =  value_type<"eq"_text>;
    using  export_t        =  value_type<"export"_text>;
    using  first_t         =  value_type<"first"_text>;
    using  if_t            =  value_type<"if"_text>;
    using  if_possible_t   =  value_type<"if_possible"_text>;
    using  import_t        =  value_type<"import"_text>;
    using  is_atom_t       =  value_type<"is_atom"_text>;
    using  is_integral_t   =  value_type<"is_integral"_text>;
    using  not_t           =  value_type<"not"_text>;
    using  or_t            =  value_type<"or"_text>;
    using  raise_error_t   =  value_type<"raise_error"_text>;
    using  rcons_t         =  value_type<"rcons"_text>;
    using  requires_t      =  value_type<"requires"_text>;
    using  show_error_t    =  value_type<"show_error"_text>;
    using  xor_t           =  value_type<"xor"_text>;



//  ----------------------------------------------------------------------------
//   Binary operations:
//  ----------------------------------------------------------------------------
//
//  a) Comparisons:  ==,  !=,  <,  >,  <=,  >=
//  b) Arithmetics:   +,  -,  *,  /,  %
//
//
//
//  ----------------------------------------------------------------------------





// Final evaluation of binary operation:


    template<  typename Op,  typename X,  typename Y  >
    struct bin_op
    :   eval_error<  Op,  X,  Y  >
    { };


//  ==,  !=,  <=,  >=,  <,  >:

    template<  auto  x
            ,  auto  y
            >
    requires
    requires(  value< bool( x == y ) >  )
    {
        nullptr;
    }
    struct bin_op<  text<'=','='>
                 ,  value< x >
                 ,  value< y >
                 >
    :   eval_result<  value< bool( x == y ) >  >
    { };



    template<  auto  x
            ,  auto  y
            >
    requires
    requires(  value< bool( x != y ) >  )
    {
        nullptr;
    }
    struct bin_op<  text<'!','='>
                 ,  value< x >
                 ,  value< y >
                 >
    :   eval_result<  value< bool( x != y ) >  >
    { };



    template<  auto  x
            ,  auto  y
            >
    requires
    requires(  value< bool( x <= y ) >  )
    {
        nullptr;
    }
    struct bin_op<  text<'<','='>
                 ,  value< x >
                 ,  value< y >
                 >
    :   eval_result<  value< bool( x <= y ) >  >
    { };



    template<  auto  x
            ,  auto  y
            >
    requires
    requires(  value< bool( x >= y ) >  )
    {
        nullptr;
    }
    struct bin_op<  text<'>','='>
                 ,  value< x >
                 ,  value< y >
                 >
    :   eval_result<  value< bool( x >= y ) >  >
    { };



    template<  auto  x
            ,  auto  y
            >
    requires
    requires(  value< bool( x < y ) >  )
    {
        nullptr;
    }
    struct bin_op<  text<'<' >
                 ,  value< x >
                 ,  value< y >
                 >
    :   eval_result<  value< bool( x < y ) >  >
    { };



    template<  auto  x
            ,  auto  y
            >
    requires
    requires(  value< bool( x > y ) >  )
    {
        nullptr;
    }
    struct bin_op<  text<'>' >
                 ,  value< x >
                 ,  value< y >
                 >
    :   eval_result<  value< bool( x > y ) >  >
    { };



//  +,  -,  *,  /,  %:

    template<  auto  x
            ,  auto  y
            >
    requires
    requires(  value<  x + y  >  )
    {
        nullptr;
    }
    struct bin_op<  text<'+'>
                 ,  value< x >
                 ,  value< y >
                 >
    :   eval_result<  value<  x + y  >  >
    { };



    template<  auto  x
            ,  auto  y
            >
    requires
    requires(  value<  x - y  >  )
    {
        nullptr;
    }
    struct bin_op<  text<'-'>
                 ,  value< x >
                 ,  value< y >
                 >
    :   eval_result<  value<  x - y  >  >
    { };



    template<  auto  x
            ,  auto  y
            >
    requires
    requires(  value<  x * y  >  )
    {
        nullptr;
    }
    struct bin_op<  text<'*'>
                 ,  value< x >
                 ,  value< y >
                 >
    :   eval_result<  value<  x * y  >  >
    { };



    template<  auto  x
            ,  auto  y
            >
    requires
    requires(  value<  x / y  >  )
    {
        nullptr;
    }
    struct bin_op<  text<'/'>
                 ,  value< x >
                 ,  value< y >
                 >
    :   eval_result<  value<  x / y  >  >
    { };



    template<  auto  x
            ,  auto  y
            >
    requires
    requires(  value<  x % y  >  )
    {
        nullptr;
    }
    struct bin_op<  text<'%'>
                 ,  value< x >
                 ,  value< y >
                 >
    :   eval_result<  value<  x % y  >  >
    { };



// -----------------------------------------------------------------------------
//
//  Expressions
//
// -----------------------------------------------------------------------------




// -----------------------------------------------------------------------------
//  quoted expressions:
// -----------------------------------------------------------------------------


    template<  typename   Expr
            ,  typename   Lut
            ,  eval_mode  mode
            >
    struct eval_<  mode,  Lut,  quoted<Expr>  >
    :   eval_result< Expr >
    { };



// -----------------------------------------------------------------------------
//  evaluation errors:
// -----------------------------------------------------------------------------


    template<  eval_mode    mode
            ,  typename     Lut
            ,  typename...  p
            >
    struct eval_<  mode
                ,  Lut
                ,  eval_error< p... >
                >
    :   eval_result<  eval_error<p... >  >
    { };



    template<  eval_mode    mode
            ,  typename     Lut
            ,  typename...  p
            ,  typename...  q
            >
    struct eval_<  mode
                ,  Lut
                ,  s<  eval_error<p...>,  q...  >
                >
    :   eval_result<  eval_error< eval_error<p...>, q... >  >
    { };



// -----------------------------------------------------------------------------
//  atoms:
// -----------------------------------------------------------------------------

    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_
    :   eval_result<  typename Lut::template lookup<p,  eval_error<p>  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  auto v
                >
    struct eval_<  mode,  Lut,  value<v>  >
    :   eval_result< value<v>  >
    { };



    template<  eval_mode  mode,  typename Lut  >
    struct eval_<  mode,  Lut,  s<> >
    :   eval_result< s<> >
    { };



    template<  eval_mode   mode
            ,  typename... Pair
            >
    struct eval_<  mode
                ,  map<  Pair... >
                ,  export_t
                >
    :   eval_result<  imported< Pair... >  >
    { };


// -----------------------------------------------------------------------------
//  parametrized unary operator op<  f, xs... >:
// -----------------------------------------------------------------------------


    template<  eval_mode    mode
            ,  typename     Lut
            ,  typename     f
            ,  typename...  xs
            >
    struct eval_<  mode
                ,  Lut
                ,  op<  f,  xs...  >
                >
    :   eval_result<  call<  1,  op<  f,  xs...  >  >  >
    { };



// -----------------------------------------------------------------------------
//  partial call:
// -----------------------------------------------------------------------------


    template<  eval_mode   mode
            ,  typename    Lut
            ,  unsigned    k
            ,  typename... fx
            >
    requires
    (
        k != 0
    )
    struct eval_<  mode,  Lut,  call<  k,  fx... >  >
    :   eval_result<  call<  k,  fx...  >  >
    { };



// -----------------------------------------------------------------------------
//  Operator identifiers:
// -----------------------------------------------------------------------------



// lists:

    template<  eval_mode  mode,  typename Lut,  unsigned n  >
    struct eval_<  mode,  Lut,  list<n>  >
    :   eval_result< call< n,  list<n> >  >
    {
        static_assert(  n != 0 );
    };


// combinators:


    template<  eval_mode                       mode
            ,  typename                        Lut
            ,  unsigned                        n
            ,  template<  typename... > class  F
            ,  typename...                     Prefix
            >
    struct eval_<  mode
                ,  Lut
                ,  combinator<  n,  F,  Prefix...  >
                >
    :   eval_result<  call<  1u,  combinator<  n,  F,  Prefix...  >  >  >
    {
        static_assert(  n != 0u,  "Internal interpreter error!"  );
    };



    template<  eval_mode                       mode
            ,  typename                        Lut
            ,  template<  typename... > class  F
            ,  typename...                     Prefix
            >
    struct eval_<  mode
                ,  Lut
                ,  combinator<  0,  F,  Prefix...  >
                >
    :   gather<  F,  Prefix...  >
    { };



    template<  eval_mode mode
            ,  typename Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  S
                >
    :   eval_result<  call< 1, S >  >
    { };



    template<  eval_mode mode
            ,  typename  x
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  op<  S,  x  >
                >
    :   eval_result<  call<  1,  S,  x  >  >
    { };



    template<  eval_mode mode
            ,  typename  x
            ,  typename  y
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  op<  S,  x,  y  >
                >
    :   eval_result<  call<  1,  S,  x,  y  >  >
    { };




    template<  eval_mode mode
            ,  typename Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  K
                >
    :   eval_result<  call<  1,  K  >  >
    { };


    template<  eval_mode mode
            ,  typename  x
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  op<  K,  x  >
                >
    :   eval_result<  call<  1,  K,  x  >  >
    { };




    template<  eval_mode  mode
            ,  typename   Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  Y_combinator
                >
    :   eval_result<  call<  1,  Y_combinator  >  >
    { };



    template<  eval_mode mode
            ,  typename  f
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  op<  Y_combinator,  f  >
                >
    :   eval_result<  call<  1,  Y_combinator,  f  >  >
    { };




// keywords:


    template<  eval_mode  mode
            ,  typename   Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  apply_t
                >
    :   eval_result<  call<  1,  apply_t  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   f
            >
    struct eval_<  mode
                ,  Lut
                ,  op<  apply_t,  f  >
                >
    :   eval_result<  call<  1,  apply_t,  f  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  and_t
                >
    :   eval_result<  call<  1,  and_t  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  op<  and_t,  p  >
                >
    :   eval_result<  call<  1,  and_t,  p  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  or_t
                >
    :   eval_result<  call<  1,  or_t  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  op<  or_t,  p  >
                >
    :   eval_result<  call<  1,  or_t,  p  >  >
    { };




    template<  eval_mode  mode
            ,  typename   Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  xor_t
                >
    :   eval_result<  call<  1,  xor_t  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  op<  xor_t,  p  >
                >
    :   eval_result<  call<  1,  xor_t,  p  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  cons_t
                >
    :   eval_result<  call<  1,  cons_t  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   x
            >
    struct eval_<  mode
                ,  Lut
                ,  op<  cons_t,  x  >
                >
    :   eval_result<  call<  1,  cons_t,  x  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  count_t
                >
    :   eval_result<  call<  1,  count_t  >  >
    { };




    template<  eval_mode  mode
            ,  typename   Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  def_t
                >
    :   eval_result<  call<  1,  def_t  >  >
    { };



    template<  eval_mode mode
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  drop_first_t
                >
    :   eval_result<  call<  1,  drop_first_t  >  >
    { };



    template<  eval_mode mode
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  eval_success_t
                >
    :   eval_result<  call<  1,  eval_success_t  >  >
    { };



    template<  eval_mode mode
            ,  typename  Lut
            >
    struct eval_< mode,  Lut,  eval_t  >
    :   eval_result<  call<  1,  eval_t  >  >
    { };



    template<  eval_mode mode
            ,  typename  Lut
            >
    struct eval_< mode
                ,  Lut
                ,  eq_t
                >
    :   eval_result<  call<  1,  eq_t  >  >
    { };




    template<  eval_mode mode
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  first_t
                >
    :   eval_result<  call<  1,  first_t  >  >
    { };



    template<  eval_mode mode
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  if_t
                >
    :   eval_result<  call<  1,  if_t  >  >
    { };



    template<  eval_mode mode
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  if_possible_t
                >
    :   eval_result<  call<  1,  if_possible_t  >  >
    { };



    template<  eval_mode mode
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  import_t
                >
    :   eval_result<  call<  1,  import_t  >  >
    { };



    template<  eval_mode mode
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  is_atom_t
                >
    :   eval_result<  call<  1,  is_atom_t  >  >
    { };



    template<  eval_mode mode
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  is_integral_t
                >
    :   eval_result<  call<  1,  is_integral_t  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  not_t
                >
    :   eval_result<  call<  1,  not_t  >  >
    { };



    template<  eval_mode mode
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  raise_error_t
                >
    :   eval_result<  call<  1,  raise_error_t  >  >
    { };



    template<  eval_mode mode
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  rcons_t
                >
    :   eval_result<  call<  1,  rcons_t  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   x
            >
    struct eval_<  mode
                ,  Lut
                ,  op<  rcons_t,  x  >
                >
    :   eval_result<  call<  1,  rcons_t,  x  >  >
    { };




    template<  eval_mode mode
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  requires_t
                >
    :   eval_result<  call<  1,  requires_t  >  >
    { };



    template<  eval_mode mode
            ,  typename  Lut
            >
    struct eval_<  mode
                ,  Lut
                ,  show_error_t
                >
    :   eval_result<  call<  1,  show_error_t  >  >
    { };



// -----------------------------------------------------------------------------
//  <=  >=  <  >  == !=  +  -  *  /  %:
// -----------------------------------------------------------------------------



//  Processing of operator names and first argument


    template<  eval_mode    mode
            ,  typename     Lut
            ,  char...      f
            >
    requires
    (
        text< f... >{} == "<="_text ||
        text< f... >{} == "!="_text ||
        text< f... >{} == "=="_text ||
        text< f... >{} == ">="_text ||
        text< f... >{} == "<"_text  ||
        text< f... >{} == ">"_text  ||
        text< f... >{} == "+"_text  ||
        text< f... >{} == "-"_text  ||
        text< f... >{} == "*"_text  ||
        text< f... >{} == "/"_text  ||
        text< f... >{} == "%"_text
    )
    struct eval_<  mode
                ,  Lut
                ,  text< f... >
                >
    :   eval_result<  call< 1,  text< f... >  >  >
    { };




    template<  eval_mode    mode
            ,  typename     Lut
            ,  char...      f
            ,  typename     p
            >
    requires
    (
        text< f... >{} == "<="_text ||
        text< f... >{} == "!="_text ||
        text< f... >{} == "=="_text ||
        text< f... >{} == ">="_text ||
        text< f... >{} == "<"_text  ||
        text< f... >{} == ">"_text  ||
        text< f... >{} == "+"_text  ||
        text< f... >{} == "-"_text  ||
        text< f... >{} == "*"_text  ||
        text< f... >{} == "/"_text  ||
        text< f... >{} == "%"_text
    )
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  text< f... >,  p   >
                >
    :   eval_result<  call<  1,  text< f... >,  p  >  >
    { };



    template<  eval_mode    mode
            ,  typename     Lut
            ,  char...      f
            ,  typename     p
            >
    requires
    (
        text< f... >{} == "<="_text ||
        text< f... >{} == "!="_text ||
        text< f... >{} == "=="_text ||
        text< f... >{} == ">="_text ||
        text< f... >{} == "<"_text  ||
        text< f... >{} == ">"_text  ||
        text< f... >{} == "+"_text  ||
        text< f... >{} == "-"_text  ||
        text< f... >{} == "*"_text  ||
        text< f... >{} == "/"_text  ||
        text< f... >{} == "%"_text
    )
    struct eval_<  mode
                ,  Lut
                ,  op<  text< f... >,  p  >
                >
    :   eval_result<  call< 1,  text< f... >,  p  >  >
    { };




    template<  eval_mode  mode
            ,  typename   Lut
            ,  char...    f
            ,  typename   p
            ,  typename   q
            >
    requires
    (
        text< f... >{} == "<="_text ||
        text< f... >{} == "!="_text ||
        text< f... >{} == "=="_text ||
        text< f... >{} == ">="_text ||
        text< f... >{} == "<"_text  ||
        text< f... >{} == ">"_text  ||
        text< f... >{} == "+"_text  ||
        text< f... >{} == "-"_text  ||
        text< f... >{} == "*"_text  ||
        text< f... >{} == "/"_text  ||
        text< f... >{} == "%"_text
    )
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  text< f... >,  p,  q   >
                >
    :   bin_op<  text< f... >,  eager_eval<  Lut,  p  >,  eager_eval<  Lut,  q  >  >
    { };


// -----------------------------------------------------------------------------
//
// Nested expressions and Curried Arguments:
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
//  general expressions:
// -----------------------------------------------------------------------------


    template<  eval_mode    mode
            ,  typename     Lut
            ,  typename     f
            ,  typename...  args
            >
   struct eval_<  mode
               ,  Lut
               ,  s<  f,  args... >
               >
    :   eval_<  mode
             ,  Lut
             ,  s<  eager_eval<  Lut,  f  >,  args...  >
             >
    { };



// collection of a function argument:

    template<  eval_mode    mode
            ,  typename     Lut
            ,  unsigned     k
            ,  typename...  p
            ,  typename     q
            ,  typename...  qs
            >
    requires
    (
        k != 0
    )
    struct eval_<  mode
                ,  Lut
                ,  s<  call<  k,  p...  >,  q,  qs...  >
                >
    :   eval_<  mode
             ,  Lut
             ,  s<  call<  k-1,  p...,  q  >,  qs...  >
             >
    { };


// collection of last function argument:


    template<  eval_mode    mode
            ,  typename     Lut
            ,  unsigned     k
            ,  typename...  p
            ,  typename     q
            >
    requires
    (
        k != 0
    )
    struct eval_<  mode
                ,  Lut
                ,  s<  call<  k,  p...  >,  q  >
                >
    :   eval_<  mode
             ,  Lut
             ,  call<  k-1,  p...,  q  >
             >
    { };




// collection of an erratic function argument:

    template<  eval_mode    mode
            ,  typename     Lut
            ,  unsigned     k
            ,  typename...  p
            ,  typename...  q
            ,  typename...  qs
            >
    requires
    (
        k != 0
    )
    struct eval_<  mode
                ,  Lut
                ,  s<  call<  k,  p...  >,  eval_error< q... >,  qs...  >
                >
    :   eval_error<  call<  k-1,  p...,  eval_error<  q...  >  >,  qs...  >
    { };



// nothing to collect any more:

    template<  eval_mode    mode
            ,  typename     Lut
            ,  unsigned     k
            ,  typename...  p
            >
    struct eval_<  mode
                ,  Lut
                ,  s<  call<  k,  p...  >  >
                >
    :   eval_<  mode
             ,  Lut
             ,  call<  k,  p...  >
             >
    { };


// Excess arguments after forming a list are not allowed:


    template<  eval_mode    mode
            ,  typename     Lut
            ,  unsigned     n
            ,  typename...  p
            ,  typename     q
            ,  typename...  qs
            >
    struct eval_<  mode
                ,  Lut
                ,  s<  call< 0, list<n>, p... >,  q,  qs...  >
                >
    :   eval_error<  call< 0, list<n>, p... >,  q,  qs...  >
    {
        static_assert( sizeof...(p) == n,  "Internal interpreter error! " );
        static_assert( n != 0,  "Internal interpreter error!" );
    };



// -----------------------------------------------------------------------------
//
// Evaluations of alls of the form
//
//                   call< 0,  op,  expr... >
//
//
// op is an operator that takes sizeof...(expr) arguments.
//
// -----------------------------------------------------------------------------



//  ----------------------------------------------------------------------------
//  S-combinator:
//  ----------------------------------------------------------------------------


    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call< 0,  S,  p  >
                >
    :   eval_result<  op<  S,  p  >  >
    { };




    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            ,  typename   q
            >
    struct eval_<  mode
                ,  Lut
                ,  call< 0,  S,  p,  q  >
                >
    :   eval_result<  op<  S,  p,  q  >  >
    { };




    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            ,  typename   q
            ,  typename   r
            >
    struct eval_<  mode
                ,  Lut
                ,  call< 0,  S,  p,  q,  r  >
                >
    :   eval_<  mode,  Lut,  s<  p,  r,  s<  q,  r  >  >  >
    { };



//  ----------------------------------------------------------------------------
//  K-combinator:
//  ----------------------------------------------------------------------------



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call< 0,  K,  p  >
                >
    :   eval_result<  op<  K,  p  >  >
    { };




    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            ,  typename   q
            >
    struct eval_<  mode
                ,  Lut
                ,  call< 0,  K,  p,  q  >
                >
    :   eval_<  mode,  Lut,  p  >
    { };



//  ----------------------------------------------------------------------------
//  Fixed-point combinator Y:
//  ----------------------------------------------------------------------------



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   f
            >
    struct eval_<  mode
                ,  Lut
                ,  call< 0,  Y_combinator,  f  >
                >
    :   eval_result<  op<  Y_combinator,  f  >  >
    { };




    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   f
            ,  typename   x
            >
    struct eval_<  mode
                ,  Lut
                ,  call< 0,  Y_combinator,  f,  x  >
                >
    :   eval_<  mode,  Lut,  s<  f,  s< Y_combinator, f >,  x >  >
    { };



//  ----------------------------------------------------------------------------
//  General combinator:
//  ----------------------------------------------------------------------------


    template<  unsigned                         N
            ,  template<  typename...  > class  F
            ,  typename...                      Prefix
            >
    struct combinator_env
    {
        template<  typename X  >
        using add_parameter =  combinator<  N-1,  F,  Prefix...,  X  >;
    };



    template<  template<  typename...  > class  F
            ,  typename...                      Prefix
            >
    struct combinator_env<  1u,  F,  Prefix...  >
    {
        template<  typename X  >
        using add_parameter =  F<  Prefix...,  X  >;
    };



    template<  eval_mode                         mode
            ,  typename                          Lut
            ,  unsigned                          N
            ,  template<  typename...  >  class  F
            ,  typename...                       Prefix
            ,  typename                          p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  combinator<  N,  F,  Prefix...  >,  p  >
                >
    :   gather<  combinator_env<  N,  F,  Prefix... >::template add_parameter
              ,  eager_eval<  Lut,  p  >
              >
    {
        static_assert(  N > 0,  "Internal interpreter error!");
    };



//  ----------------------------------------------------------------------------
//  Definitions:
//  ----------------------------------------------------------------------------


    template<  typename F  >
    struct def_lhs
    :   eval_error<  value_type<"unusable left-hand side in def: "_text>,  F  >
    { };


    template<  char... f  >
    struct def_lhs< text< f... >  >
    :   eval_result<  op<  def_t,  text< f... >  >  >
    { };


    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   f
            >
    struct eval_<  mode
                ,  Lut
                ,  call< 0,  def_t,  f  >
                >
    :   def_lhs<  eager_eval<  Lut,  f  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  char...    f
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  op<  def_t,  text< f... >  >,  p  >
                >
    :   gather<  op
              ,  def_t
              ,  text<f...>
              ,  eager_eval<  Lut
                           ,  s<  typename application<  text< f... >  >::template apply_to< p >
                               ,  s<  Y_combinator
                                   ,  typename application<  text< f... >  >::template apply_to< p >
                                   >
                               >
                           >
              >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  char...    f
            ,  typename   p
            ,  typename   q
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  op<  def_t,  text< f... >,  p  >,  q  >
                >
    :   eval_<  mode
             ,  typename Lut::template add_or_replace<  pair<  text< f... >,  p  >  >
             ,  q
             >
    { };



// -----------------------------------------------------------------------------
//  unevaluated lists:
// -----------------------------------------------------------------------------


    template<  typename...  > struct lazy { };



    template<  typename     Lut
            ,  typename...  p
            >
    struct eval_<  eval_mode::eager
                ,  Lut
                ,  lazy< p... >
                >
    :   gather< s,  eager_eval< Lut, p >...  >
    { };



    template<  typename     Lut
            ,  typename...  p
            >
    struct eval_<  eval_mode::lazy
                ,  Lut
                ,  lazy< p... >
                >
    :   eval_result<  lazy< p... >  >
    { };



// -----------------------------------------------------------------------------
//  list<n>:
// -----------------------------------------------------------------------------


    template<  typename     Lut
            ,  unsigned     n
            ,  typename...  p
            >
    requires
    (
        n != 0  &&  sizeof...(p) == n
    )
    struct eval_<  eval_mode::eager
                ,  Lut
                ,  call< 0,  list<n>,  p...  >
                >
    :   gather< s,  eager_eval< Lut, p >...  >
    {
        static_assert(  sizeof...(p) == n,  "Internal interpreter errror!"  );
    };



    template<  typename     Lut
            ,  unsigned     n
            ,  typename...  p
            >
    requires
    (
        n != 0  &&  sizeof...(p) == n
    )
    struct eval_<  eval_mode::lazy
                ,  Lut
                ,  call< 0,  list<n>,  p...  >
                >
    :   gather<  lazy,  p...  >
    {
        static_assert(  sizeof...(p) == n,  "Internal interpreter errror!"  );
    };



//  ----------------------------------------------------------------------------
//  first:
//  ----------------------------------------------------------------------------


    template<  eval_mode,  typename,  typename X  >
    struct First
    :   eval_error<  first_t,  X  >
    { };



    template<  eval_mode    mode
            ,  typename     Lut
            ,  typename     X
            ,  typename...  Xs
            >
    struct First<  mode,  Lut,  s<  X,  Xs... >  >
    :   eval_result< X  >
    { };



    template<  eval_mode    mode
            ,  typename     Lut
            ,  typename     p
            ,  typename...  ps
            >
    struct First<  mode,  Lut,  lazy<  p,  ps... >  >
    :   eval_< mode,  Lut,  p  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  first_t,  p  >
                >
    :   First<  mode,  Lut,  lazy_eval< Lut, p >  >
    { };



//  ----------------------------------------------------------------------------
//  drop_first:
//  ----------------------------------------------------------------------------


    template<  eval_mode,  typename,  typename  >
    struct Drop_First;



    template<  typename     Lut
            ,  typename     p
            ,  typename...  ps
            >
    struct Drop_First<  eval_mode::lazy
                     ,  Lut
                     ,  lazy<  p,  ps...  >
                     >
    :   eval_result<  lazy< ps... >  >
    { };



    template<  typename     Lut
            ,  typename     p
            ,  typename...  ps
            >
    struct Drop_First<  eval_mode::eager
                     ,  Lut
                     ,  lazy<  p,  ps...  >
                     >
    :  gather<  s,  eager_eval< Lut, ps >...  >
    { };



    template<  eval_mode    mode
            ,  typename     Lut
            ,  typename     p
            ,  typename...  ps
            >
    struct Drop_First<  mode
                     ,  Lut
                     ,  s<  p,  ps...  >
                     >
    :   eval_result<  s<  ps...  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  drop_first_t,  p  >
                >
    :   Drop_First<  mode,  Lut,  lazy_eval<  Lut, p >  >
    { };



// -----------------------------------------------------------------------------
//  cons:
// -----------------------------------------------------------------------------


// lazy evaluation:


    template<  typename P,  typename Q  >
    struct Lazy_Cons_
    :   type_hull<  eval_error<  cons_t,  P,  Q  >  >
    { };


    template<  typename     p
            ,  typename...  q
            >
    struct Lazy_Cons_<  p,  s< q... >  >
    :   type_hull<  s< p, q... >  >
    { };



    template<  typename...  p
            ,  typename...  q
            >
    struct Lazy_Cons_<  lazy<p...>,  s< q... >  >
    :   type_hull<  lazy< lazy< p... >, quoted<q>... >  >
    { };



    template<  typename     p
            ,  typename...  q
            >
    struct Lazy_Cons_<  p,  lazy< q... >  >
    :   type_hull<  lazy<  quoted<p>,  q...  >  >
    { };



    template<  typename...  p
            ,  typename...  q
            >
    struct Lazy_Cons_<  lazy< p... >,  lazy< q... >  >
    :   type_hull<  lazy<  lazy<p...>,  q...  >  >
    { };



    template<  typename P,  typename Q  >
    using Lazy_Cons = typename Lazy_Cons_<  P,  Q  >::type;



    template<  typename   Lut
            ,  typename   p
            ,  typename   q
            >
    struct eval_<  eval_mode::lazy
                ,  Lut
                ,  call< 0,  cons_t,  p,  q  >
                >
    :   gather<  Lazy_Cons,  lazy_eval< Lut, p >,  lazy_eval< Lut, q >  >
    { };



// eager evaluation:


    template<  typename P,  typename Q  >
    struct Cons_
    :   type_hull< eval_error<  cons_t,  P,  Q  >  >
    { };



    template<  typename P,  typename... Q  >
    struct Cons_<  P,  s< Q... >  >
    :   type_hull<  s<  P,  Q...  >  >
    { };


    template<  typename P,  typename Q  >
    using Cons = typename Cons_< P, Q >::type;


    template<  typename   Lut
            ,  typename   p
            ,  typename   q
            >
    struct eval_<  eval_mode::eager
                ,  Lut
                ,  call<  0,  cons_t,  p,  q  >
                >
    :   gather<  Cons,  eager_eval< Lut, p >,  eager_eval< Lut, q >  >
    { };



    template<  typename   Lut
            ,  typename   p
            >
    struct eval_<  eval_mode::eager
                ,  Lut
                ,  call<  0,  cons_t,  p   >
                >
    :   eval_result<  op<  cons_t,  p  >  >
    { };




// -----------------------------------------------------------------------------
//  rcons:
// -----------------------------------------------------------------------------


// lazy evaluation:


    template<  typename P,  typename Q  >
    struct Lazy_RCons_
    :   type_hull<  eval_error<  rcons_t,  P,  Q  >  >
    { };


    template<  typename     p
            ,  typename...  q
            >
    struct Lazy_RCons_<  p,  s< q... >  >
    :   type_hull<  s< q..., p >  >
    { };



    template<  typename...  p
            ,  typename...  q
            >
    struct Lazy_RCons_<  lazy<p...>,  s< q... >  >
    :   type_hull<  lazy<  quoted<q>...,  lazy< p... >  >  >
    { };



    template<  typename     p
            ,  typename...  q
            >
    struct Lazy_RCons_<  p,  lazy< q... >  >
    :   type_hull<  lazy<  q...,  quoted<p>  >  >
    { };



    template<  typename...  p
            ,  typename...  q
            >
    struct Lazy_RCons_<  lazy< p... >,  lazy< q... >  >
    :   type_hull<  lazy<  q...,  lazy<p...>  >  >
    { };



    template<  typename P,  typename Q  >
    using Lazy_RCons = typename Lazy_RCons_<  P,  Q  >::type;



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call< 0,  rcons_t,  p >
                >
    :   eval_<  mode,  Lut,  call<  1,  rcons_t,  p  >  >
    { };



    template<  typename   Lut
            ,  typename   p
            ,  typename   q
            >
    struct eval_<  eval_mode::lazy
                ,  Lut
                ,  call< 0,  rcons_t,  p,  q  >
                >
    :   gather<  Lazy_RCons,  lazy_eval< Lut, p >,  lazy_eval< Lut, q >  >
    { };



// eager evaluation:


    template<  typename P,  typename Q  >
    struct RCons_
    :   type_hull< eval_error<  rcons_t,  P,  Q  >  >
    { };



    template<  typename P,  typename... Q  >
    struct RCons_<  P,  s< Q... >  >
    :   type_hull<  s<  Q...,  P  >  >
    { };


    template<  typename P,  typename Q  >
    using RCons = typename RCons_< P, Q >::type;


    template<  typename   Lut
            ,  typename   p
            ,  typename   q
            >
    struct eval_<  eval_mode::eager
                ,  Lut
                ,  call< 0,  rcons_t,  p,  q  >
                >
    :   gather<  RCons,  eager_eval< Lut, p >,  eager_eval< Lut, q >  >
    { };



// -----------------------------------------------------------------------------
//  count:
// -----------------------------------------------------------------------------


    template<  typename X  >
    struct Count
    :   eval_error<  count_t,  X  >
    { };



    template<  typename... p  >
    struct Count<  lazy< p... >  >
    :   eval_result<  integer< sizeof...(p) >  >
    { };



    template<  typename... p  >
    struct Count<  s< p... >  >
    :   eval_result<  integer< sizeof...(p) >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   xs
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  count_t,  xs  >
                >
    :   Count<  lazy_eval< Lut, xs >  >
    { };



// -----------------------------------------------------------------------------
//  apply:
// -----------------------------------------------------------------------------


    template<  eval_mode mode,  typename Lut >
    struct env_apply
    {
        template<   typename F,  typename Xs  >
        struct  Apply_
        :   eval_error<  F,  Xs  >
        { };


        template<  typename F,  typename... Xs  >
        struct Apply_<  F,  s< Xs... >  >
        :   eval_<  mode,  Lut,  s<  F,  quoted< Xs >...  >  >
        { };


        template<  typename F,  typename Xs  >
        using Apply = typename Apply_<  F,  Xs  >::type;
    };



    template<  eval_mode    mode
            ,  typename     Lut
            ,  typename     f
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  apply_t,  f   >
                >
    :   eval_result<  op<  apply_t,  f  >  >
    { };




    template<  eval_mode    mode
            ,  typename     Lut
            ,  typename     f
            ,  typename     xs
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  apply_t,  f,  xs  >
                >
    :   gather<  env_apply<  mode,  Lut  >::template Apply,  f,  eager_eval< Lut, xs >  >
    { };



//  ----------------------------------------------------------------------------
//  eq:
//  ----------------------------------------------------------------------------


    template<  typename X,  typename Y  >
    struct Eq_
    :   eval_result<  value< false >  >
    { };



    template<  typename X  >
    struct Eq_< X, X >
    :   eval_result<  value< true >  >
    { };



    template<  typename X,  typename Y  >
    using Eq = typename Eq_<  X,  Y  >::type;




    template<  eval_mode mode
            ,  typename  Lut
            ,  typename  p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  eq_t,  p  >
                >
    :   gather<  op,  eq_t,  eager_eval<  Lut,  p  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            ,  typename   q
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  op<  eq_t,  p  >,  q  >
                >
    :   gather<  Eq,  p,  eager_eval<  Lut,  q  >  >
    { };




// -----------------------------------------------------------------------------
//  eval:
// -----------------------------------------------------------------------------


    template<  eval_mode    mode
            ,  typename     Lut
            ,  typename     p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  eval_t,  p  >
                >
    :   eval_<  mode,  Lut,  eager_eval<  Lut,  p  >  >
    { };





// -----------------------------------------------------------------------------
//  eval_success:
// -----------------------------------------------------------------------------


    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  eval_success_t,  p  >
                >
    :   no_eval_error<  eager_eval<  Lut,  p  >  >
    { };



// -----------------------------------------------------------------------------
//  import:
// -----------------------------------------------------------------------------


    template<  typename Erratic_Import  >
    struct Import
    :   eval_error<  value_type<"erratic import argument"_text>
                  ,  Erratic_Import
                  >
    { };



    template<  typename... D  >
    struct Import<  imported< D... >  >
    :   eval_result<  op<  import_t,  typename imported< D... >::map  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  import_t,  p  >
                >
    :   Import<  eager_eval<  Lut,  p  >  >

    { };



    template<  eval_mode     mode
            ,  typename      Lut
            ,  typename...   D
            ,  typename      q
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  op<  import_t,  map< D... >  >,  q  >
                >
    :   eval_<  mode
             ,  typename Lut::template add_or_replace< D... >
             ,  q
             >
    { };



//  ----------------------------------------------------------------------------
//   is_atom:
//  ----------------------------------------------------------------------------


    template<  typename p >
    struct Is_Atom
    :   eval_error<  is_atom_t, p  >
    { };



    template<  auto v  >
    struct Is_Atom<  value<v> >
    :   eval_result< value<true> >
    { };



    template< unsigned n,  typename f  >
    struct Is_Atom<  call< n, f >  >
    :   eval_result< value< true > >
    { };



    template<  typename... Xs  >
    struct Is_Atom<  s< Xs... >  >
    :   eval_result<  value< sizeof...(Xs) == 0 >  >
    { };



    template<  typename... Xs  >
    struct Is_Atom<  lazy< Xs... >  >
    :   eval_result<  value< sizeof...(Xs) == 0 >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call< 0,  is_atom_t,  p  >
                >
    :   Is_Atom<  lazy_eval< Lut, p >  >
    { };



//  ----------------------------------------------------------------------------
//   is_integral:
//  ----------------------------------------------------------------------------

    template<  typename T  >
    using Is_Integral =
    value<  requires( T t,  T* p,  void (*f)(T) )  //T* parameter excludes refernce types
            {
                reinterpret_cast<T>(t); // Exclude class types
                f(0);                   // Exclude enumeration types
                p + t;                  // Exclude everything not yet excluded but integral types
            }
    >;


    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  is_integral_t,  p  >
                >
    :   gather<  Is_Integral,  lazy_eval< Lut, p >  >
    { };






//  ----------------------------------------------------------------------------
//   requires:
//  ----------------------------------------------------------------------------

    template<  eval_mode
            ,  typename
            ,  typename cond
            ,  typename p
            >
    struct Requires
    :   eval_error<  requires_t,  cond,  p  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct Requires<  mode
                   ,  Lut
                   ,  value<true>
                   ,  p
                   >
    :   eval_<  mode,  Lut,  p  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   cond
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  requires_t,  cond,  p  >
                >
    :   Requires<  mode,  Lut,  eager_eval< Lut, cond >,  p  >
    { };



//  ----------------------------------------------------------------------------
//   not:
//  ----------------------------------------------------------------------------


    template<  typename B  >
    struct Not
    :   eval_result<  eval_error<  not_t, B >  >
    { };



    template<  bool b >
    struct Not<  value<b> >
    :   eval_result<  value< !b >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call< 0,  not_t,  p  >
                >
    :   Not<  eager_eval< Lut, p >  >
    { };



//  ----------------------------------------------------------------------------
//   and:
//  ----------------------------------------------------------------------------


    template<  typename B,  typename  C  >
    struct And
    :   eval_result<  eval_error<  and_t,  B,  C  >  >
    { };



    template<  bool a,  bool b  >
    struct And<  value<a>,  value<b>  >
    :   eval_result<  value< a && b >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  and_t,  p  >
                >
    :   eval_result<  op<  and_t,  p  >  >
    { };




    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            ,  typename   q
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  and_t,  p,  q  >
                >
    :   And<  eager_eval< Lut, p >,  eager_eval< Lut, q >  >
    { };



//  ----------------------------------------------------------------------------
//   or:
//  ----------------------------------------------------------------------------


    template<  typename B,  typename  C  >
    struct Or
    :   eval_result<  eval_error<  or_t,  B,  C >  >
    { };



    template<  bool a,  bool b  >
    struct Or<  value<a>,  value<b>  >
    :   eval_result<  value< a || b >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  or_t,  p  >
                >
    :   eval_result<  op<  or_t,  p  >  >
    { };



    template<  eval_mode mode
            ,  typename  Lut
            ,  typename  p
            ,  typename  q
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  or_t,  p,  q  >
                >
    :   Or<  eager_eval< Lut, p >,  eager_eval< Lut, q >  >
    { };



//  ----------------------------------------------------------------------------
//   xor:
//  ----------------------------------------------------------------------------


    template<  typename B,  typename  C  >
    struct Xor
    :   eval_result<  eval_error<  xor_t,  B,  C >  >
    { };



    template<  bool a,  bool b  >
    struct Xor<  value<a>,  value<b>  >
    :   eval_result<  value< ! (a == b)  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  xor_t,  p  >
                >
    :   eval_result<  op<  xor_t,  p  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            ,  typename   q
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  xor_t,  p,  q  >
                >
    :   Xor<  eager_eval< Lut, p >,  eager_eval< Lut, q >  >
    { };




//


// -----------------------------------------------------------------------------
//  if:
// -----------------------------------------------------------------------------



    template<  typename cond  >
    struct If_
    :   eval_error<  if_t,  cond  >
    { };



    template<  bool True  >    // gcc does not like complete specialisations
    requires
    (
        True == true
    )
    struct If_<  value< True >  >
    :   eval_result< K >
    { };



    template<  bool False  >  // gcc does not like complete specialisations
    requires
    (
        False == false
    )
    struct If_<  value< False >  >
    :   eval_result<  s<  K,  s<  S,  K,  K >  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   cond
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  if_t,  cond  >
                >
    :   If_<  eager_eval<  Lut,  cond  >  >
    { };




// -----------------------------------------------------------------------------
//  if_possible:
// -----------------------------------------------------------------------------

    template<  typename p  >
    struct If_Possible
    :   eval_result<  s<  K,  quoted<p>  >  >
    { };



    template<  typename...  p  >
    struct If_Possible<  eval_error< p... >   >
    :   eval_result<  s<  S,  K,  K  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  if_possible_t,  p  >
                >
    :   If_Possible<  typename eval_<  mode,  Lut,  p  >::type  >
    { };



// -----------------------------------------------------------------------------
//  raise_error:
// -----------------------------------------------------------------------------

    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call< 0,  raise_error_t,  p  >
                >
    :   eval_error<  raise_error_t,  eager_eval< Lut, p >  >
    { };



// -----------------------------------------------------------------------------
//  show_error:
// -----------------------------------------------------------------------------

    template<  typename  >
    struct Show_Error
    :   eval_result<  debug_output<>  >
    { };



    template<  typename... p  >
    struct Show_Error<  eval_error< p... >  >
    :   eval_result<  debug_output<  eval_error< p... >  >  >
    { };



    template<  eval_mode  mode
            ,  typename   Lut
            ,  typename   p
            >
    struct eval_<  mode
                ,  Lut
                ,  call<  0,  show_error_t,  p  >
                >
    :   Show_Error<  eager_eval< Lut, p >  >
    { };



// -----------------------------------------------------------------------------
//
//
//
// Preparation of the public interpreter interface:
//
//
//
// -----------------------------------------------------------------------------


    template<  typename  expr
            ,  typename  lut
            >
    struct Eval_
    {
        using type = eager_eval<  lut,  expr  >;

        static_assert(  no_eval_error<type>{} == t_true{}
                     ,
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



// -----------------------------------------------------------------------------
//
//
//
// Helpers for the library import mechanism:
//
//
//
// -----------------------------------------------------------------------------


    template<  typename... >
    friend struct i;



// -----------------------------------------------------------------------------
// gather_definitions:  Generate a map  that contains all definitions
// -----------------------------------------------------------------------------



    template<  typename,  typename...  >
    struct gather_definitions;



    template<  typename...  F
            ,  typename...  P
            >
    struct gather_definitions< s< pair< F, P >... >  >
    :   type_hull<  typename lt::map<>::template add_or_replace< pair< F, P >... >  >
    { };


    template<  typename...  Result
            ,  char...      g
            ,  typename     Q
            ,  typename...  More
            >
    struct gather_definitions<  s< Result... >
                             ,  pair< text<g...>, Q >
                             ,  More...
                             >
    :   gather_definitions<  s<  Result..., pair< text<g...>,  Q >  >,  More... >
    { };



    template<  typename...  Result
            ,  typename...  More
            ,  typename...  Even_More
            >
    struct gather_definitions<  s< Result... >,  imported< More... >,  Even_More... >
    :   gather_definitions<  s< Result... >,  More...,  Even_More... >
    { };



    template<  typename D  >
    struct checked_def_
    {
        static_assert( sizeof(D) != sizeof(D),  "Invalid  library entry!" );
    };



    template<  char...   f
            ,  typename  p
            >
    struct checked_def_<  pair<  text<f...>,  p  >  >
    :   type_hull<  pair<  text<f...>,  p  >  >
    { };



    template<  typename D  >
    using checked_def = typename checked_def_<D>::type;



public:

// -----------------------------------------------------------------------------
//
//
//
// The Public Interpreter Interface:
//
//
//
// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
// The evaluators:
// ----------------------------------------------------------------------------

    template<  literal_or_s prog
            ,  auto         lib   =  map{}
            >
    using eval  =
    typename Eval_<  value_type<  publicly_visible::template parse_< prog >()  >
                  ,  value_type< lib >
                  >::type;



    template< literal_or_s  src
            ,  typename...  X
            >
    using metaprogram  =  eval<  s<  s_expr<src>,  quoted<X>... >{}  >;



// -----------------------------------------------------------------------------
// Library management:
// ----------------------------------------------------------------------------

    using map = lt::map<>;


    template<  typename... Defs  >
    using add = typename imported< Defs... >::map::template apply< imported >;


    template<  typename... Defs  >
    using add_or_replace = add<  Defs... >;

};



namespace lt
{
    template<  lt::literal_or_s  prog
            ,  auto              lib  =  map{}
            >
    using eval  =  typename imported<>::template eval<  prog,  lib  >;


    template<  lt::literal_or_s prog,  typename... X  >
    using metaprogram = typename imported<>::template metaprogram< prog, X... >;
}



template<  typename D0,  typename... Dr  >
struct lt::i<  D0,  Dr...  >
{
    using map = typename  imported<>::template gather_definitions< s<>, D0, Dr... >::type;

private:

    template<  typename  >
    struct add_;


    template<  typename... MoreDefs  >
    struct add_<  lt::map< MoreDefs... >  >
    :   type_hull<  typename map::
                    template add_or_replace<  typename imported<>::
                                              template checked_def<  MoreDefs  >...
                                            >
                 >
    { };



public:

// -----------------------------------------------------------------------------
// The evaluators:
// -----------------------------------------------------------------------------


    template<  literal_or_s prog,  auto lib = map{}  >
    using eval =
    typename imported<>::template eval<  prog
                                      ,  typename add_< value_type<lib> >::type{}
                                      >;



    template<  literal_or_s prog,  typename... X  >
    using metaprogram = eval< s< s_expr<prog>, quoted<X>... >{}  >;



// -----------------------------------------------------------------------------
// Library management:
// -----------------------------------------------------------------------------


    template<  typename...  MoreDefs  >
    using add   =
        typename map::
        template add<  typename imported<>::
                       template checked_def< MoreDefs >...
                    >::
        template apply< imported >;



    template<  typename...  MoreDefs  >
    using add_or_replace   =
        typename map::
        template add_or_replace<  typename imported<>::
                                  template checked_def< MoreDefs >...
                               >::
        template apply< imported >;

};
