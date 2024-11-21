/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "application.hpp"
#include "type_hull.hpp"
#include "text.hpp"



namespace lt
{
    template<  typename...  >
    struct s {};



    template<  typename... Xs  >
    using s_expr  =  s<  Xs...  >;



    template<  typename  >
    struct quoted {};



    template< int n >
    struct list{};



    template< >
    struct s<>
    {
    private:

        class parser;
    };


}


class lt::s<>::parser
{

    static consteval bool is_whitespace( char x  )
    {
        return  x == ' '   ||
                x == '\t'  ||
                x == '\v'  ||
                x == '\f'  ||
                x == '\r'  ||
                x == '\n';
    };



    static consteval bool is_bracket(  char x  )
    {
        return  x  ==  '('  ||  x  ==  ')'  ||
                x  ==  '['  ||  x  ==  ']'  ||
                x  ==  '{'  ||  x  ==  '}' ;
    }



    template<  char...  x  >
    using token = text<  x...  >;



    template<  typename...  Xs >
    struct  sequence  {};



    template<  typename...  Xs  >
    using token_sequence  =   sequence<  Xs...  >;



    template<  typename,  char... >
    struct tokenizer;




    // split_stream conains the most recently scanned token token and an unscanned stream
    //  More_Input...  The template alias add_tokens_and_continue adds token to a token list and runs
    //  the tokenizer that contains the updated token list again with the remaining input More_Input... .

    template<  typename Latest_Token,  char...  More_Input  >
    struct tokenization_step_result
    {
        template< typename...  Tokens  >
        struct update_token_sequence
        {
            using continue_  =
            tokenizer<  token_sequence<  Tokens...,  Latest_Token  >,  More_Input...  >;
        };
    };


    // token<';'> indicates a comment to be ignored.
    //
    template<  char...  More_Input  >
    struct tokenization_step_result<  token<';'>,  More_Input...  >
    {
        template<  typename...  Tokens  >
        struct update_token_sequence
        {
            using continue_  =
            tokenizer<  token_sequence<  Tokens...  >,  More_Input...  >;
        };
    };



    //  get token:

    template<  char...  stream  >
    struct get_token;



    // ignore comments:

    template<  char... empty_stream  >
    struct ignore_remainder_of_line
    :   tokenization_step_result<  token<';'>  >
    {
        static_assert(  sizeof...(empty_stream) == 0u  );
    };


    template<  char head,  char... stream  >
    struct ignore_remainder_of_line<  head,  stream...  >
    :   ignore_remainder_of_line<  stream...  >
    { };



    template<  char... stream  >
    struct ignore_remainder_of_line< '\n',  stream...  >
    :   tokenization_step_result<  token<';'>,  stream...  >
    { };



    // token<';'>:


    template<  char...  stream  >
    struct get_token< ';',  stream...  >
    :   ignore_remainder_of_line<  stream... >
    { };



    // token<'\''>:

    template< char... stream  >
    struct get_token< '\'',  stream...  >
    :   tokenization_step_result<  token< '\'' >,  stream...  >
    { };



    // scan brackets:

    template<  char bracket,  char... stream  >
    requires
    (
        is_bracket( bracket )
    )
    struct get_token<  bracket,  stream...  >
    :   tokenization_step_result<  token< bracket >,  stream...  >
    { };



    // numbers:


    //  The class
    //              scan_num< sign, base, 0, xs...  >
    //
    //  has the base
    //  split_stream<  token< n >,  ys...  >
    //  where xs...  is the pack  ns..., ys... with ns... encodeing the number n.
    //
    //  Implementation comes later.
    //
    template<  int,  int,  int,  char...  >
    struct scan_num;



    // identifier:

    template<  typename,  char...  >
    struct scan_identifier ;



    template<  char     x
            ,  char...  xs
            ,  char...  ys
            >
    requires
    (
        is_whitespace(x) || is_bracket(x)
    )
    struct scan_identifier<  text<  ys...  >,  x,  xs...  >
    :   tokenization_step_result<  text<  ys...  >,  x,  xs...  >
    { };



    template<  char... ys  >
    struct scan_identifier<  text<  ys...  >  >
    :   tokenization_step_result<  text<  ys...  >  >
    { };



    template<  char     x
            ,  char...  xs
            ,  char...  ys
            >
    struct scan_identifier<  text<  ys...  >,  x,  xs...  >
    :   scan_identifier<  text<  ys...,  x  >,  xs...  >
    { };



    template<  char... cs  >
    struct get_token
    :   scan_identifier<  text<>,  cs...  >
    { };



    // hex numbers:

    template<  char...  cs  >
    struct get_token<  '0',  'x',  cs...  >
    :   scan_num<  1,  16,  0,  cs...  >
    { };



    template<  char...  cs  >
    struct get_token<  '+',  '0',  'x',  cs...  >
    :   scan_num<  1,  16,  0,   cs...  >
    { };



    template<  char...  cs  >
    struct get_token<  '-',  '0',  'x',  cs...  >
    :   scan_num<  -1,  16,  0,   cs...  >
    { };



    // octal numbers

    template<  char  c,  char...  cs  >
    struct get_token<  '0',  c,  cs...  >
    :   scan_num<  +1,  +8,  0,  c,  cs...  > { };



    template<  char  c,  char... cs  >
    struct get_token<  '+',  '0',  c,  cs...  >
    :   scan_num<  +1,  8,  0,  c,  cs...  >
    { };



    template<  char  c,  char...  cs  >
    struct get_token<  '-',  '0',  c,  cs...  >
    :   scan_num<  -1,  8,  0,  c,  cs...  >
    { };



    //  binary numbers:

    template<  char...  cs  >
    struct get_token<  '0',  'b',  cs...  >
    :   scan_num<  +1,  2,  0,  cs...  >
    { };



    template<  char...  cs  >
    struct get_token<  '+',  '0',  'b',  cs...  >
    :   scan_num<  +1,  2,  0,  cs...  >
    { };



    template<  char...  cs  >
    struct get_token<  '-',  '0',  'b',  cs...  >
    :   scan_num<  -1,  2,  0,  cs...  >
    { };



    //  decimal numbers:

    template< char  c,  char... cs  >
    requires
    (
        '0' <= c  &&  c <= '9'
    )
    struct get_token<  c,  cs...  >
    :   scan_num<    +1,  10,  0,  c,  cs...  >
    { };



    template< char  c,  char... cs  >
    requires
    (
        '0' <= c  &&   c <= '9'
    )
    struct get_token< '+',  c,  cs...  >
    :   scan_num<  +1,  10,  0,  c,  cs...  >
    { };



    template< char  c,  char... cs  >
    requires
    (
        '0' <= c  &&   c <= '9'
    )
    struct get_token< '-',  c,  cs...  >
    :   scan_num<  -1,  10,  0,  c,  cs...  >
    { };



//  digits:

    template<  char c  >
    static constexpr int digit =
        ( '0' <= c  &&  c <= '9' )  ?  c - '0' :
            (   ( 'a' <= c  &&  c <= 'f' )  ?  10 + (c - 'a') :
                    ('A' <= c  &&  c <= 'F' ) ?  10  + ( c - 'A')  :
                        - 1 );




    template< int,  int,  int,   char...  >
    struct scan_num;



    template<  int sign,  int base,  int result  >
    struct scan_num<  sign
                   ,  base
                   ,  result
                   >
    :   tokenization_step_result<  integer<  result  >  >
    { };



    template<  int      sign
            ,  int      base
            ,  int      result
            ,  char     h
            ,  char...  t
            >
    requires(  digit<  h  >    >=   0   )
    struct scan_num<  sign
                    ,  base
                    ,  result
                    ,  h
                    ,  t...
                    >
    :   scan_num<  sign
                 ,  base
                 ,  result * base + sign * digit<h>
                 ,  t...
                 >
    {
        static_assert( digit<h>  <  base );
    };



    template<  int      sign
            ,  int      base
            ,  int      result
            ,  char     h
            ,  char...  t
            >
    requires(  digit<h>  <  0   )  // not a digit
    struct scan_num<  sign
                    ,  base
                    ,  result
                    ,  h
                    ,  t...
                    >
    :   tokenization_step_result<  integer<  result  >,  h,  t...  >
    { };



// tokenizer:


// tokenizer ready, no more tokens to be scannned:  The resul of the tokenizer
// is the type  tokenizer<  Tokens... >::type

    template<  typename... Tokens  >
    struct tokenizer<  token_sequence<  Tokens...  >  >
    :   type_hull<  token_sequence<  Tokens...  >  >
    { };



// ignore spaces:


    template<  typename...  Tokens
            ,  char         x
            ,  char...      xs
            >
    requires
    (
        is_whitespace( x )
    )
    struct tokenizer<  token_sequence<  Tokens...  >,  x,  xs...  >
    :   tokenizer<  token_sequence<  Tokens...  >,  xs...  >
    { };



//  get next token (throught get_token), then add it to the tokenizer and continue the
//  same proceducre with the updated tokenizer.

    template<  typename...  Tokens
            ,  char         x
            ,  char...      xs
            >
    struct tokenizer<  token_sequence<  Tokens...  >,  x,  xs...  >
    :  get_token<  x,  xs...  >::template update_token_sequence<  Tokens...  >::continue_
    { };


    template<  char...  xs  >
    using tokenizer_result   =  typename tokenizer<  token_sequence<>,  xs...  >::type;


//  parser:

    template<  typename,  typename  >
    struct parser_;



//  data types:

    template<  typename... Xs  >
    using stack  =  sequence<  Xs...  >;



    template<  typename... Xs  >
    using input  =  sequence<  Xs...  >;



    template<  typename Input  >
    struct with_remaining_input
    {
        using remaining_input =  Input;
    };



// parser start without  a bracket:

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


//  parser start with an opening bracket, ie '(',  '[' or '{':

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
              ,  input<  s_expr<  Ys...  >,  Xs...  >
              >
    { };



    template<  typename...  Xs
            ,  typename...  Ys
            >
    struct parser_<  stack<  token<'('>,  token<'l','i','s','t'>,  Ys...  >
                  ,  input<  token<')'>,  Xs...  >
                  >
    :  parser_<  stack<>
              ,  input<  s_expr<  list< sizeof...(Ys) >
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
    :   type_hull< s_expr<> >
    {
        static_assert( sizeof...(zero_args) == 0 );
         // gcc dislikes full specializations appearing inline
    };



    template<  typename X  >
    struct right_assoc_expression<  X  >
    :   type_hull<  s_expr< X >  >
    { };



    template<  typename X,  typename Y  >
    struct right_assoc_expression<  X,  Y  >
    :   type_hull< s_expr< X, Y >  >
    { };



    template<  typename X,  typename Y,  typename... Z  >
    struct right_assoc_expression<  X,  Y,  Z... >
    :   type_hull<  s_expr< X,  typename right_assoc_expression< Y, Z... >::type  >  >
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

    template<  char...  xs  >
    requires
    requires(  typename parser_<  stack<>
                               ,  tokenizer_result<  xs...  >
                               >::
               remaining_input r

            ,  sequence<> s  )
    {
        r = s;
    }
    using parser_result =  typename parser_<  stack<>,  tokenizer_result<  xs...  >  >::type;



public:


    template<  text<>::literal  listing  >
    friend constexpr auto operator ""_s()  noexcept
    {
        return lt::text<>::as_type<  parser_result,  listing  >();
    }
};



template<  lt::text<>::literal  >
constexpr auto lt::operator ""_s()  noexcept;
