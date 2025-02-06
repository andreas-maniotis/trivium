/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "type_hull.hpp"
#include "text.hpp"
#include "map.hpp"

namespace lt
{
// PART 0: Types


// values:

    template<  auto x,  typename T = decltype(x)  >
    struct v
    {
        using type = T;
    };



    template<  auto x,  typename T  >
    struct v<  x,  T const  >
    {
        using type = T;
    };



    template< auto const x  >
    using value = v< x >;



    template<  int const n  >
    using integer  =  value< n >;



    // value_type removes const qualifiers:
    // value_type<x> is T for an object of type "T const"
    template<  auto x  >
    using value_type = typename value<x>::type;



// s: a type for brackets ( ... ) in symbolic expressions



    template<  typename...  >
    struct s { };



// quoted expression:

    template<  typename  >
    struct quoted {};



// marker for lists with n elements:

    template< int n >
    struct list{};



//  literal_or_s


    template<  unsigned,  typename  >
    struct literal_or_s;



    template<  unsigned N
            ,  typename
            >
    struct literal_or_s
    :   public text<>::literal< N >
    {
        consteval literal_or_s( char const (& content)[N] )
        :   lt::text<>::literal<N>(content)
        { };



        consteval literal_or_s( text<>::literal<N> const& other )
        :   lt::text<>::literal<N>(other)
        { }


        using type = text<>::literal< N >;
    };



    template<  typename S_Expr >
    struct literal_or_s< 0,  S_Expr const >
    {
        consteval literal_or_s(  S_Expr const )  { }

        using type = S_Expr;
    };



    template<  typename T  >
    literal_or_s(T const t) -> literal_or_s<  0,  T const  >;



    template<  unsigned N  >
    literal_or_s( char const (&)[N] ) -> literal_or_s< N,  char const (&)[N]  >;



    template<  unsigned N  >
    literal_or_s( text<>::literal<N> const& )   ->  literal_or_s< N,  char const (&)[N]  >;



// nil:

    template< >
    struct s<>
    {
    private:

        struct parser;



        template<  typename T
                ,  typename
                >
        struct subst_
        :   type_hull< T >
        { };



        template<  char...                         id
                ,  typename                        Subst
                >
        struct subst_<  text< id... >,  Subst  >
        :   type_hull<  typename Subst::template lookup<  text< id... >,  text< id...  >  >  >
        { };



        template<  typename...                      Xs
                ,  typename                         Subst
                >
        requires(  sizeof...(Xs) != 0  )
        struct subst_<  s< Xs... >,  Subst  >
        :   type_hull<  s<  typename subst_<  Xs,  Subst  >::type...  >  >
        { };



        template<  literal_or_s  src
                ,  map           subst
                ,  typename         =  decltype(src)
                ,  typename      s_ = s<>
                >
        struct expr_;



        template<  literal_or_s  src
                ,  map           subst
                ,  typename      Src
                ,  typename      s_
                >
        requires
        requires(  decltype(src.content)  )
        {
            nullptr;
        }
        struct expr_<  src,  subst,  Src,  s_  >
        :   s_::parser::template run< static_cast< text<>::literal< sizeof(src.content) > const&>(src),  subst  >
        { };



        template<  literal_or_s  src
                ,  map           subst
                ,  typename      T
                ,  typename      s_
                >
        struct expr_<  src,  subst,  literal_or_s<  0,   T const  >, s_  >
        :   subst_<  value_type< T{} >,  decltype(subst)  >
        { };



    public:

        template<  literal_or_s  src
                ,  map           subst
                >
        using expr =  typename expr_< src, subst >::type;
    };



    using nil = s<>;



    template<  literal_or_s const  src
            ,  map const           subst = map{}
            >
    using s_expr  =  typename s<>::template expr< src, subst>;




// PART I:  Symbolic Components


// -----------------------------------------------------------------------------
// class_template:
// -----------------------------------------------------------------------------


    template<  template< typename... >  class  >
    struct ct {};



    template<  template< typename... >  class F  >
    using class_template  =  ct< F >;



// -----------------------------------------------------------------------------
// combinator:
// -----------------------------------------------------------------------------

    template<  unsigned n,  template< typename... >  class  >
    requires( n != 0 )
    struct co {};



    template<  unsigned n,  template< typename... > class F  >
    using combinator  =  co< n, F >;



// -----------------------------------------------------------------------------
// Assign a type to a symbol:
// -----------------------------------------------------------------------------



    template<  lt::text<>::literal s,  typename  T  >
    inline constexpr auto assign  =
    []
    {
        constexpr struct
        {
            using  key    =  decltype(operator""_text<s>());
            using  value  =  T;

            static_assert(  text_type<key> );

            // make arbitrary use of typename value to suppress compiler warnings.
            static_assert(  type_hull<value>{} == type_hull<T>{} );

        } x;

        return x;
    }
    ();



// PART II:  Lambda-Calculus


    struct S {};  //  S represents the S-combinator  S = lambda xyz . xz(yz).
    struct K {};  //  K represents the K-combinator  K = lambda xy.x.



    template<  typename...  >
    struct application;



    template<>
    struct application<>
    {
    private:

        template<  typename...  >
        friend struct application;


        template<  typename X  >
        using with_result  =  type_hull<X>;


// Make opposite expression:

        template<  typename... no_elements >
        struct r_expr
        {
            static_assert( sizeof...(no_elements) == 0 );


            using reverse = r_expr<>;


            template<  typename X  >
            using rcons = r_expr< X >;
        };



        template<  typename H,  typename... T  >
        struct r_expr< H, T... >
        {
            using reverse =   typename r_expr< T... >::reverse::template rcons< H >;


            template<  typename X  >
            using rcons  = r_expr<  H,  T...,  X  >;
        };



        template<  typename X  >
        struct opposite_expr
        :   with_result<  X  >
        { };



        template< int Zero >
        requires( Zero == 0 )
        struct opposite_expr<  s< list<Zero>  >  >
        :   with_result<  s<>  >
        { };



        template<  typename     X
                ,  typename...  Xs
                >
        struct opposite_expr<  s<  X,  Xs... >  >
        :   with_result<  typename r_expr<  typename opposite_expr< X  >::type
                                         ,  typename opposite_expr< Xs >::type...  >::
                          reverse
                       >
        {
            static_assert(sizeof...(Xs) != 0
                         , "Forbidden nullary function call. No user-defined function f "
                           "may be called without arguments via the expression (f)." );
        };


//  Apply variable to opposite expression


        template<  typename,  typename   >
        struct ap_;



        template<  char... x  >
        struct ap_<  text< x... >,  text< x... >  >
        :   with_result<  s< S, K, K >  >
        { };



        template<  char...   x
                ,  typename  Y
                >
        struct ap_< text<x...>,  Y  >
        :   with_result<  s< K, Y  >  >
        { };



        template<  typename,  typename  >
        struct combine_;



        template<  char...   x
                ,  typename  Q
                ,  typename  P
                >
        struct ap_<  text<x...>,  r_expr<  Q,  P  >  >
        :   combine_<  typename  ap_< text<x...>, P >::type
                    ,  typename  ap_< text<x...>, Q >::type
                    >
        { };



        template<  char...      x
                ,  typename     Q
                ,  typename     P
                ,  typename...  Ps
                >
        struct ap_<  text<x...>,  r_expr<  Q,  P,  Ps...  >  >
        :   combine_<  typename  ap_<  text<x...>,  r_expr<  P,  Ps... >  >::type
                    ,  typename  ap_<  text<x...>,  Q  >::type
                    >
        { };



        template<  typename P,  typename Q  >
        struct combine_
        :   with_result<  s< S,  P,  Q  >  >
        { };




        template<  typename P,  typename Q  >
        struct combine_<  s< K, P >,  s< K, Q >  >
        :   with_result<  s<  K,  s< P, Q >  >  >
        { };



        template<  typename     P
                ,  typename...  Ps
                ,  typename     Q
                >
        struct combine_<  s< K, s< P,  Ps... > >,  s< K, Q >  >
        :   with_result<  s<  K,  s< P, Ps...,  Q >  >  >
        { };



        template<  typename     P  >
        struct combine_<  s<  K,  P  >,  s< S, K, K >  >
        :   with_result< P >
        { };
    };



    template<  char...  X  >
    struct application<  text< X... >  >
    {
        template<  typename Expr  >
        using apply_to  =
        typename application<>::
        template ap_<  text< X... >

                    ,  typename application<>::
                       template opposite_expr<  Expr  >::
                       type

                    >::type;
    };



    template<  typename      X
            ,  typename      Y
            ,  typename...   Ys
            >
    requires
    (
        text_type<X>  &&
        ( text_type<Y>  &&  ...  &&  text_type<Ys> )
    )
    struct application<  X,  Y,  Ys...  >
    {
        template<  typename  Expr  >
        using apply_to  =
        typename application<  X  >::
        template apply_to<  typename application<  Y,  Ys...  >::
                            template apply_to<  Expr  >
                         >;
    };


}



struct lt::s<>::parser
{
    template<  typename...  >
    struct token_sequence {};



    struct lexer
    {
        static consteval bool is_whitespace( char x  )
        {
            return  x == ' '   ||
                    x == '\t'  ||
                    x == '\v'  ||
                    x == '\f'  ||
                    x == '\r'  ||
                    x == '\n';
        }



        static consteval bool is_bracket(  char x  )
        {
            return  x  ==  '('  ||  x  ==  ')'  ||
                    x  ==  '['  ||  x  ==  ']'  ||
                    x  ==  '{'  ||  x  ==  '}' ;
        }



        struct token
        {
            unsigned    begin_;
            unsigned    end_;

            consteval token( unsigned begin,  unsigned end  )
            :   begin_(begin),  end_(end)        { }

        };




        template<  lt::text<>::literal source  >
        static consteval token  step( unsigned pos )
        {
            constexpr char const (&src)[ sizeof(source.content)] = source.content;


            unsigned pos_ = pos;


            while(  src[pos_] != 0  &&  (is_whitespace(src[pos_])  ||  src[pos_] == ';' )  )
            {
                if ( src[pos_] == ';' )
                {
                    while(  src[pos_] != '\0'  &&  src[pos_] != '\n'  )
                    {
                        ++pos_;
                    }
                }


                while ( src[pos_] != 0 &&  is_whitespace(src[pos_])  )
                {
                    ++pos_;
                }
            }



            switch ( src[pos_] )
            {
            case '\0':
                return token{ pos_,  pos_ };

            case '\'':
            case '@':
            case '(':
            case ')':
            case '[':
            case ']':
            case '{':
            case '}':
                return token{ pos_,  pos_+1 };

            default:
                ;
            }


            unsigned pos_end_ = pos_ + 1;

            while(  src[pos_end_] != '\0'  &&
                    src[pos_end_] != ';'   &&
                    ! is_whitespace(src[pos_end_]) &&
                    ! is_bracket(src[pos_end_])  )
            {
                ++pos_end_;
            }

            return token{ pos_,  pos_end_ };
        }



        // specializations below

        template<  typename T  >
        struct sublexer_
        :   type_hull< T >
        { };



        template<  typename T  >
        using sublexer  =  typename sublexer_<T>::type;




        template<  typename X,  typename Map  >
        using subst  =  typename Map::template lookup< X, X >;




        template<  lt::text<>::literal src,  map mp,  unsigned n,  token const... seq  >
        struct run
        :   run<  src,  mp,  step<src>(n).end_,  seq...,  step<src>(n)  >
        { };



        template<  lt::text<>::literal src,  map mp,  unsigned n,  token const... seq  >
        requires
        (
            step<src>(n).begin_ == step<src>(n).end_
        )
        struct run<  src,  mp,  n,  seq...  >
        :   type_hull<  token_sequence<  subst<  sublexer< typename text<>::
                                                           template subtext< src, seq.begin_,  seq.end_ > >
                                              ,  decltype(mp)
                                              >...
                                      >
                     >
        {};



// sublexer for numbers
//
// Grammar:

        // num := hex_num | dec_num | oct_num | bin_num
        // hex_num :=  sign 0[x|X]   hex_suffix
        // dec_num :=  sign [1-9]    dec_suffix
        // oct_num :=  sign 0        oct_suffix
        // bin_num :=  sign 0[b|B]   bin_suffix


        // sign   := + | - | ""
        // hex_suffix := [0-9,a-f,A-F]+
        // dec_suffix := [0-9]*
        // oct_suffix := [0-7]*
        // bin_suffix := [0,1]+



        template< char d  >
        static consteval int digit_()
        {
            switch( d )
            {
            case '0':  case '1':  case '2':  case '3':  case '4':
            case '5':  case '6':  case '7':  case '8':  case '9':

                return d - '0';

            case 'a':  case 'A':
                return 10;

            case 'b':  case 'B':
                return 11;

            case 'c':  case 'C':
                return 12;

            case 'd':  case 'D':
                return 13;

            case 'e':  case 'E':
                return 14;

            case 'f':  case 'F':
                return 15;
            }

            return -1;
        }



        template<  int base, char d  >
        requires
        (
            ( base == 2 || base == 8 || base == 10 || base == 16 )
            &&
            digit_< d >()  < base
        )
        static consteval int digit()
        {
            return digit_<d>();
        }


        // numbers from base and digit sequence:


        template<  int base,  int n,  int...  >
        struct construct_number;



        template< int base,  int n  >
        struct construct_number<  base,  n  >
        :   type_hull< integer<n> >
        { };



        template< int base, int n,  int h,  int... t  >
        struct construct_number<  base,  n,  h,  t...  >
        :   construct_number<  base,  n * base + h,  t... >
        { };



        template<  typename  >
        struct negative;



        template<  int n  >
        struct negative<  integer<n>  >
        :   type_hull< integer<-n>  >
        { };



//  The sublexer:

        template<  char digit,  char...  digits  >
        struct sublexer_<  text< '+', digit, digits...  >  >
        :   sublexer_<  text< digit, digits... >  >
        { };



        template<  char digit,  char... digits  >
        struct sublexer_<  text< '-',  digit,  digits... >  >
        :   negative<  typename sublexer_<  text< digit, digits... >  >::type  >
        { };



        template< char zero  >
        requires( zero == '0' )
        struct sublexer_< text<zero>  >
        :   type_hull<  integer<0>  >
        { };



        template<  char h,  char... t  >
        requires(  h !='x'  &&  h != 'X'  &&  h != 'b'  &&  h != 'B'  )
        struct sublexer_<  text< '0', h, t... >  >
        :   construct_number<  8,  digit<8, h>(),  digit<8, t>()...  >
        { };



        template<  char... num  >
        struct sublexer_<  text< '0', 'x', num... >  >
        :   construct_number<  16,  digit<16, num>()...  >
        { };



        template<  char... num  >
        struct sublexer_<  text< '0', 'X', num... >  >
        :   construct_number<  16,  digit<16, num>()...  >
        { };



        template<  char... num  >
        struct sublexer_<  text< '0', 'B', num... >  >
        :   construct_number<  2,  digit<2, num>()...  >
        { };



        template<  char... num  >
        struct sublexer_<  text< '0', 'b', num... >  >
        :   construct_number<  2,  digit<2, num>()...  >
        { };



        template<  char h,  char... t >
        requires( '1' <=  h  && h <= '9' )
        struct sublexer_<  text< h, t... >  >
        :   construct_number<  10,  digit<10, h>(),  digit<10, t>()...  >
        { };

    };



// PARSER:

//  parser:

    template<  typename,  typename  >
    struct parser_;



//  data types:


    template<  char... xs  >
    using token = text< xs... >;


    template<  typename... Xs  >
    using stack  =  token_sequence<  Xs...  >;



    template<  typename... Xs  >
    using input  =  token_sequence<  Xs...  >;



    template<  typename Input  >
    struct with_remaining_input
    {
        using remaining_input =  Input;
    };



// parser begin without  a bracket:

    template<  typename     X
            ,  typename...  Xs
            >
    struct parser_<  stack<>,  input<  X,  Xs...  >  >
    :   type_hull<  X  >
    ,   with_remaining_input<  input<  Xs...  >  >
    { };



// quotation through ':

    template<  typename     X
            ,  typename...  Xs
            >
    struct parser_<  stack<>,  input<  token<'\''>,  X,  Xs...  >  >
    :   type_hull<   quoted<  typename parser_<  stack<>
                                              ,  input <X,  Xs... >
                                              >::type
                           >
                 >
    ,   with_remaining_input< typename parser_<  stack<>,  input< X,  Xs... >  >::remaining_input  >
    { };



    template<  typename     Y
            ,  typename...  Ys
            ,  typename     X
            ,  typename...  Xs
            >
    struct parser_<  stack<  Y
                          ,  Ys...
                          >
                  ,  input<  token<'\''>,  X,  Xs...  >
                  >
    :   parser_<  stack<  Y
                       ,  Ys...
                       ,  typename parser_<  stack<>
                                          ,  input<  token<'\''>,  X,  Xs...  >
                                          >::type
                       >
               ,  typename parser_<  stack<>,  input<  token<'\''>,  X,  Xs...  >  >::remaining_input
               >
    { };


// shortened navigator call:  @expr shall be interpreted as (` expr)

    template<  typename     X
            ,  typename...  Xs
            >
    struct parser_<  stack<>,  input<  token<'@'>,  X,  Xs...  >  >
    :   type_hull<   s<  token<'`'>
                      ,  typename parser_<  stack<>
                                         ,  input <X,  Xs... >
                                         >::type
                           >
                 >
    ,   with_remaining_input< typename parser_<  stack<>,  input< X,  Xs... >  >::remaining_input  >
    { };



    template<  typename     Y
            ,  typename...  Ys
            ,  typename     X
            ,  typename...  Xs
            >
    struct parser_<  stack<  Y
                          ,  Ys...
                          >
                  ,  input<  token<'@'>,  X,  Xs...  >
                  >
    :   parser_<  stack<  Y
                       ,  Ys...
                       ,  typename parser_<  stack<>
                                          ,  input<  token<'@'>,  X,  Xs...  >
                                          >::type
                       >
               ,  typename parser_<  stack<>,  input<  token<'@'>,  X,  Xs...  >  >::remaining_input
               >
    { };



//  parser begin with an opening bracket, ie '(',  '[' or '{':

    template<  char opening_bracket
            ,  typename...  Xs
            >
    requires
    (
        opening_bracket == '(' ||
        opening_bracket == '[' ||
        opening_bracket == '{'
    )
    struct parser_<  stack<>,  input<  token< opening_bracket >,  Xs...  >  >
    :   parser_<  stack<  token< opening_bracket >  >,  input<  Xs...  >  >
    { };



    template<  char         opening_bracket
            ,  typename...  Xs
            >
    requires
    (
        opening_bracket == '(' ||
        opening_bracket == '[' ||
        opening_bracket == '{'
    )
    struct parser_<  stack<>,  input<  token<'\''>,  token< opening_bracket >,  Xs...  >  >
    :   type_hull<  quoted<
                            typename parser_< stack< token< opening_bracket > >, input< Xs... > >::type
                          >
                 >
    ,   with_remaining_input<  typename parser_< stack< token< opening_bracket > >, input <Xs... > >::remaining_input  >
    { };



// parser encounters new opening bracket:

    template<  char         opening_bracket
            ,  typename...  Xs
            ,  typename...  Ys
            >
    requires
    (
        opening_bracket == '(' ||
        opening_bracket == '[' ||
        opening_bracket == '{'
    )
    struct parser_<  stack<  Ys...  >
                  ,  input<  token< opening_bracket >,  Xs...  >
                  >
    :  parser_<  stack<  Ys...
                      ,  typename parser_<  stack<>,  input<  token< opening_bracket >,  Xs...  >  >::type
                      >
              ,  typename parser_<  stack<>,  input<  token< opening_bracket >,  Xs...  >  >::remaining_input
              >
    { };




// parser encounters a new subterm within a pair of brackets:

    template<  typename     X
            ,  typename...  Xs
            ,  typename...  Ys
            >
    struct parser_<  stack<  Ys...  >,  input<  X,  Xs...  >  >
    :   parser_<  stack<  Ys...,  X  >,  input<  Xs...  >  >
    { };



// parser encounters closing round bracket:

    template<  typename...  Xs
            ,  typename...  Ys
            >
    struct parser_<  stack<  token<'('>,  Ys...  >
                  ,  input<  token<')'>,  Xs...  >
                  >
    :  parser_<  stack<>
              ,  input<  s<  Ys...  >,  Xs...  >
              >
    { };



    template<  typename...  Xs
            ,  typename...  Ys
            >
    struct parser_<  stack<  token<'('>,  token<'l','i','s','t'>,  Ys...  >
                  ,  input<  token<')'>,  Xs...  >
                  >
    :  parser_<  stack<>
              ,  input<  s<  list< sizeof...(Ys) >
                           ,  Ys...
                           >
                      ,  Xs...
                      >
              >
    { };



    template<  typename...  Xs
            ,  typename     Y
            >
    struct parser_<  stack<  token<'('>,  token<'q','u','o','t','e'>,  Y  >
                  ,  input<  token<')'>,  Xs...  >
                  >
    :  parser_<  stack<>
              ,  input<  quoted< Y >,  Xs...  >
              >
    { };




//  parser encounters closing square bracket:

    template<  typename...  Xs
            ,  typename...  Ys
            >
    struct parser_<  stack<  token<'['>,  Ys...  >
                  ,  input<  token<']'>,  Xs...  >
                  >
    :   type_hull<  typename application<  Ys...  >::
                    template apply_to<  typename parser_<  stack<>
                                                        ,  input<  Xs...  >
                                                        >::type
                                     >
                 >
    ,   with_remaining_input<  typename parser_<  stack<>
                                               ,  input<  Xs...  >
                                               >::remaining_input
                            >
    { };



// parser encounters a closing curly bracket:

    template<  typename...  zero_args >
    struct right_assoc_expression
    :   type_hull< s<> >
    {
        static_assert( sizeof...(zero_args) == 0 );
         // gcc dislikes full specializations appearing inline
    };



    template<  typename X  >
    struct right_assoc_expression<  X  >
    :   type_hull<  s< X >  >
    { };



    template<  typename X,  typename Y  >
    struct right_assoc_expression<  X,  Y  >
    :   type_hull< s< X, Y >  >
    { };



    template<  typename X,  typename Y,  typename... Z  >
    struct right_assoc_expression<  X,  Y,  Z... >
    :   type_hull<  s< X,  typename right_assoc_expression< Y, Z... >::type  >  >
    { };



    // { x1 ... xn } ->  x1 ( x2 ( x3 ( ... (x_{n-1} x_n) ... ) ))
    template<  typename...  Xs
            ,  typename...  Ys
            >
    struct parser_<  stack<  token<'{'>,  Ys...  >
                  ,  input<  token<'}'>,  Xs...  >
                  >
    :  parser_<  stack<>
              ,  input<  typename right_assoc_expression<  Ys...  >::type,  Xs...  >
              >
    { };



// parse a string literal:


public:


    template<  text<>::literal listing,  map mp  >
    requires
    requires(  typename parser_<  stack<>
                               ,  typename lexer::template run< listing,  mp,  0 >::type
                               >::remaining_input  r

            ,  token_sequence<> s_
            )
    {
        r = s_; // assignment only exists if types are equal.
    }
    using run  =
    parser_<  stack<>
                    ,  typename lexer::template run<  listing,  mp,  0  >::type
                    >;

};
