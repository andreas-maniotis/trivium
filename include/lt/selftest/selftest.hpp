/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Andreas Milton Maniotis.//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Email: andreas.maniotis@gmail.com
/////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef  TRIVIUM_CHECK_IS_DYNAMIC

    // CHECK is the name of a macro from doctest  to be included below
    #define TRIVIUM_CHECK  CHECK

#else

    #define TRIVIUM_CHECK  static_assert

#endif



#include "doctest.h"



#if __has_include(<boost/core/demangle.hpp>) || __has_include("boost/core/demangle.hpp")

    #if __has_include(<boost/core/demangle.hpp>)

        #include <boost/core/demangle.hpp>

    #else

        #include "boost/core/demangle.hpp"

    #endif

    #include<regex>
    #include<sstream>
    #include<iterator>
    #include<vector>


    namespace lt::selftest
    {

        // The mangled name is transformed through boost::demangle into a demangled name.
        // Then the demangled name is further simplified by replacing any substring arising
        // from a type  lt::text< xs... >  with the substring "\"xs...\"".
        //
        // Thus the type
        //
        //      lt::text<> will appear on the screen as ""  (written as the string "\"\""),
        //
        // and the type
        //
        //      lt::text<'A', 'B', 'C' > will appear on the screen as "ABC".
        //
        // "ABC" is a more readable abbreviation for the original boost::demangle output
        //
        //      lt::text<(char)65, (char)66, (char)67>.
        //
        auto  demangle(  char const* mangled_name,  std::string const suppress_re=""  )
        {
           auto const create_text_string =
            [] (std::string  input)
            {
                // The string " lt::text<  (char)n1, (char)n2, ... >"  is transformed into
                // "n1   n2  n3 ... ",  with n1, n2... integers in the ascii-range 0..255.
                // Then an istream_iterator reas the values in this string and creates an
                // output string from it.

                for (auto& x : input)
                {
                    if ( x < '0' || x > '9' ) x = ' ';
                }

                std::istringstream is(input);

                std::vector<int> v{ std::istream_iterator<int>(is), std::istream_iterator<int>() };

                std::string output("\"");

                for (auto const el : v )
                {
                    output.push_back( static_cast<char const>(el) );
                }

                output.push_back('\"');
                return output;
            };

            std::string result{ boost::core::demangle(mangled_name) };


            std::regex const text_regex("(lt::t<>|lt::t<(\\(char\\)[[:digit:]]*(, |>))+)");

            std::smatch  match;

            while(  std::regex_search(result,  match,  text_regex  )  )
            {
                result = std::string(match.prefix()) + create_text_string( match[0] ) + std::string(match.suffix());
            }
            // result has now the form "n1  n2  n3  n4 ..."  with n1, n2... being numbers representing ascii-codes of
            // letters.



            // suppress  a regular string appearing in result,  for instance a namespace-prefix like "lt::".
            if ( suppress_re.size() != 0 )
            {
                result = std::regex_replace( result,  std::regex{suppress_re.c_str()}, "");
            }


           return result;
        }
    }


#else

    namespace lt::selftest
    {
        char const* demangle(  char const* mangled_name,  char const* = nullptr  )
        {
            return mangled_name;
        }
    }


#endif


#include "configuration_space.hpp"

#include<type_traits>



namespace lt::selftest
{
    template<  typename T  >
    constexpr auto print_type =
    [](  char const* txt = "",  char const* suppress_re=""  )
    {
        std::cout  <<  std::endl << std::endl
                   <<  txt
                   <<  demangle(typeid(T).name(),  suppress_re)
                   <<  std::endl << std::endl;
    };



    template<  typename expr, typename Expr  >
    void check_expression_equality(  char const* msg = "Undesired condition: ",  char const* suppress=""  )
    {
        if  ( ! std::is_same_v<  expr,  Expr  > )
        {
            std::cout << "______________________" << std::endl;
            std::cout << msg << "Expressions are not equal!" << std::endl << std::endl;
            print_type<expr>("",  suppress);
            print_type<Expr>("",  suppress);
            std::cout << "______________________" << std::endl  << std::endl;
        }

        TRIVIUM_CHECK(  std::is_same_v<  expr,  Expr  >  );
    }



    template<  typename expr, typename Expr  >
    void check_expression_inequality(  char const* msg = "Undesired condition: ",  char const* suppress=""  )
    {
        if( std::is_same_v<  expr,  Expr  > )
        {
            std::cout << "______________________" << std::endl;
            std::cout << msg <<  "Expressions are equal!" << std::endl << std::endl;
            print_type<expr>("",  suppress);
            print_type<Expr>("",  suppress);
            std::cout << "______________________" << std::endl  << std::endl;
        }

        TRIVIUM_CHECK( ! std::is_same_v<  expr,  Expr  >  );
    }
}
