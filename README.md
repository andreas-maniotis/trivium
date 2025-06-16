---
\documentclass[a4papter]: srcartcl
geometry: margin=2cm
inputenc: utf8x
babel: greek, english
alphabeta:
pagestyle: headings
-header-includes: \babelprovide[import onchar=ids fonts]{greek}
komascript:
standalone: yes
mainfont: LiberationSans
sansfont: DejaVuSans
monofont: NotoMono
...

<!-- pdf-engine: xelatex -->


***2025/06/16:  The Trivium Library has recently undergone a complete redesign and a vast extension in functionality. The interpreter is very well-tested, but at this stage the documentation is lacking. Examples are currently only available in the unit-test section (in lt/src/selftest/ ), but the documentation will be extended in connection with instructive examples twice a month until it reaches its full extent in the combing weeks.***


# TRIVIUM: A LIBRARY FOR SYMBOLIC METAPROGRAMMING IN C++


*Metaprogrammed code in C++ can be as simple, clear, reusable, modular and configurable as code that is written in a functional language like Lisp or Haskell.*


Template metaprogramming (TMP) code tends to be unfriendly to humans. The code is generally neither easy to read nor easy to write.


The Trivium framework gives a solution to this problem by organising TMP indirectly by the means of Trivium Lisp, a symbolic domain specific language (DSL) for metaprogramming.  Metaprograms are not encoded directly in C++, but as symbolic expressions in Trivium-Lisp.

This indirection allows us to hide all the “ugly parts” of TMP in the implementation of the Trivium Lisp interpreter, which is a “universal” C++ template metaprogram to interpret Trivium Lisp expressions as C++ types.


The metaprograms themselves, written in Trivium Lisp, become as clean and as simple as programs written in any reasonable functional language, such as Haskell or Lisp.


The library consists of three components:

1. The DSL for metaprogramming called *Trivium Lisp* together with an interpreter.
2. A symbolic representation of the C++ type system.
3. A C++-representation of the symbolic Trivium Lisp expressions.

The two representations together establish a simple closure mechanism that allows to solve template metaprogramming tasks elegantly in Trivium Lisp and to inject Trivium Lisp code into C++-templates.



## Requirements
C++ 20, no dependencies on any other library. Currently the library compiles with Gcc >= 13.3.1, Clang >= 16.0.6, Msvc 19.42 VS 17.12.


## Download
[https://www.github.com/andreas-maniotis/trivium](https://www.github.com/andreas-maniotis/trivium)

## Structure:
The framework consists of the following header files that are contained in the folder include/lt:

1. text.hpp:                  compile-time strings;
2. type_hull.hpp:             a wrapper for C++ types;
3. s_expr.hpp:                symbolic expressions and lambda expressions;
4. map.hpp:                   a compile-time dictionary;
5. eval.hpp                   an interpreter for Trivium Lisp (dsl for TMP).
6. type_system.hpp:           a symbolic representation of the C++-type system;
7. mpstruct.hpp:              a metaprogrammable struct (still experimental)
8. Headers in lt/lib          metaprogramming libraries written in Trivium Lisp.




## Building Examples and Unit Tests
At the moment only Linux/Unix makefiles are provided.

1. Unit Tests: In the folder src/selftest  type `make -j$nproc`.



## A First Example

Imagine that we have a class template called *my_alloc* and that we want to replace every occurrence of *std::allocator* by *my_alloc* in an arbitrary type *X*. This problem is solved by the following metaprogram:

```
template<  typename T
        ,  template<  typename... > class Old_Tmpl
        ,  template<  typename... > class New_Tmpl
        >   
using replace =
lt::imported< lt::lib::ts >::metaprogram<

    R"( 

    [ type  old_alloc new_alloc ]
    {
        (   def 'subst  [ from to term ]
                        (   if  ( eq term from )

                            to

                            (   if  ( is_atom term )
                                term
                                ( cons  ( subst from to (first term) )
                                        ( subst from to (drop_first term) )
                                )
                            )
                        )
        )
        (lisp_to_cpp (   subst  old_alloc  new_alloc  (cpp_to_lisp type)  )  )
    }


)",  T,  lt::class_template<Old_Tmpl>,  lt::class_template<New_Tmpl>  >;
```


The function `subst`, which takes three variables, defines the substitution algorithm abstractly in Trivium Lisp. The arguments are bound to C++-types with the help of the class template `lt::metaprogram`. This class template takes a Trivium Lisp function in `n` variables, followed by `n` C++-types as its input. The C++-types will be passed in their original order to the Trivium Lisp function. The function `cpp_to_lisp` is a C++-type introspector. It decomposes a C++-type into a symbolic Trivium Lisp expression. This expression can be queried and modified by a symbolic program. The function `lisp_to_cpp` carries out the 'opposite' operation of interpreting symbolic expressions as C++-types. Perhaps surprisingly, the two operations are not inverses of each other, but 'sufficiently close' to being inverses: For any C++ type `T` that is not constructed with the help of types or class templates from the Trivium library we witness
```
	lisp_to_cpp[  cpp_to_lisp [T] ] = T.
```
Furthremore, we witness the identities
```
	lisp_to_cpp o cpp_to_lisp o lsip_to_cpp = lisp_to_cpp
```
and
```
	cpp_to_lisp o lisp_to_cpp o cpp_to_lisp = cpp_to_lisp.
```

(Here we use the notation `f[x]` for `f` applied to `x`, whereas the language Trivium Lisp usese the Lisp notation `(f x)` for `f`  applied to `x` ),



### Limits of the Symbolic Representations

The symbolic representation of the C++ typesystem has limits. It is possible to to decompose every type `T` that is not identical with `std::decay_t<T>` into subtypes. Furthermore, it is also possible to decompose instantiations of template classes of the form 
```
    template<  typename... > class x.
```
But instances of template classes that either contain template template parameters or non-type parameters cannot be broken up into components. 

If introspections for for instantiations of such template classes are needed, then they need to provide some handcrafted code to deal with it. This restriction may change when C++26 is adopted. 

In the above example we would not be able to deal with a class template like  `template< auto > struct foo {}`.  

The type  
```  
replace<  foo< static_cast< std::allocator<int>* >(nullptr) >
       ,  std::allocator
       ,  my_alloc  
       >
``` 
would be identical with the original type
```
    foo< static_cast< std::allocator<int>* >(nullptr) >.
```




### Data and Algorithms in Template Metaprogramming

In contemporary publicly known template metaprogramming code we often encounter two possible problems: Either we witness a complete absence of a separation between algorithms and data, or the interaction between algorithms and data is too cumbersome. 

An example for the first problem can be seen in the following definition of the factorial map:

```
template< int n >
struct factorial { enum { value = n * factorial<n-1>::value }; };

template<>
struct factorial<0> { enum { value = 1 }; };
```

In pseudo-code we would something similar to this:

```
def f [n] {  if (n = 0) return 1  else return n * f(n-1) }
```

The if-condition in the pseudo-code is realised only implicitly through a template specialisation in the C++-code. This is not good of course, because we always want to have something like an if-condition stated explicitly.

Note that we could have chosen to carry out a compile-time computation with values instead of types to improve the readability:

```
constexpr int f(int n) 
{ 
    if constexpr (n == 0)
    { 
    		return 1;  
    	}
    else 
    {
	    return n * f(n-1);
	}
}
```
This approach, taken for instance by Boost Hana, is unproblematic when small inputs are processed by compilers. But for contemporary C++ compilers processing values is far more expensive than processing types. The template class instantiations that arise under the hood, for instance when recursive Trivium Lisp functions  with several variables are processed, can be so large that the value based approach becomes too expensive or even unfeasible because of a massive slowdown and huge memory consumption. Trivium keeps dealing with values to an absolute minimum.



Now we consider the second problem, the cumbersome interaction between data and algorithms. We give a few classes as prerequisites for an example:

```
struct x { ... };

template< typename X>
struct f { ... };

template< typename X>
requires( std::is_integral_v<X> )
struct g { ... };
```

The metaprogram we want to compute is: 
*If x is integral than use `f<x>`, otherwise use `g<x>`*. Imagine that we only have the standard header type_traits at hand. Then we would write something like:

```
template< template<  typename > class   >
struct template_ {};

template< typename X >
using y = 
typename
std::conditional_t<  std::is_integral_v<X>
                   ,  lt::class_template<f>
                    , lt::class_template<g>  >::template apply_to<X>;
```


In the source code above we neither see  `g<X>` nor `f<X>` appearing directly, because the following code would not work:
```
template<  typename X  > 
using y = 
std::conditional_t<  std::is_integral_v<X>, f<X>, g<X>  >;
```

If we attempt to instantiate y<double>, then logically we want to discard f<X> and to pick g<X>, but we end up with a compilation-error because of a failed attempt to instantiate g<X>. The indirect construction with apply_to is harder to read, especially when the metaprogramming code is long.

In Trivium-Lisp the metaprogram-code would look as follows:

First we need a wrapper for std::is_integral that returns a symbolic type for true or false depending the result of std::is_integral_v:

```
template< typename T >
using Is_Integral = lt::value< bool( std::is_integral_v<T> ) >;
```


Then we can compute the desired class template:

```
template<  typename X  >
using result =
lt::metaprogram< "([is_integral f g x] ((if is_integral f g ) x)",
				   Is_Integral
				   lt::combinator< 1, f >
				   lt::combinator< 1, g >" >;
```

We see that no indirection is needed. The symbolic representations of `f<x>` and `g<x>` are present, and the one not chosen is being discarded before an instantiation takes place. 


Of course in this small use case, which is of little complexity, the traditional approach with `std::is_integral_v` fares better than the solution with Trivium, because there is almost no algorithmic complexity in the computation involved.  However the algorithmic component of TMP code is only modestly complex, then Trivium fares far better in terms of readability and  maintainability than direct C++-code.


### The Template Instantiation Rules

The template instantiation rules are 

One of the most serious source of complexity and bugs in template metaprogramming are the template instantiation rules. They are complex, sometimes counter-intuitive, and, occasionally a template specialisation breaks code that appears unrelated to it. Unexpected ambiguities or undesired substitutions may arise, whereas the evaluation scheme of Trivium Lisp is extremely simple and predictable.



## Achievements

0. Simplicity: Metaprogramming has been separated from the peculiarities of the type system of C++. Thus anyone who merely knows what a template is and either knows basic Lisp (without macros) or is willing to learn it can metaprogram with Trivium. Furthermore, the metaprograms are short, clean, with intuitive semantics and without the clutter of a cumbersome syntax that is typical for conventional template metaprograms.


*Advantages that arise from the separation of data and algorithms:*

1. Readable and concise code.
2. Generic code.
2. Reusable code.
3. Maintainable code.

The logic of a metaprogram is concentrated in functional metaprograms. Thus the effort of maintaining good metaprogramming code is identical with the effort needed for maintaining well-written Lisp-code (without macros).

5. No dependence on other libraries: Trivium can be used in embedded environments.

6. Trivium is non-intrusive and cooperative. It does reinvent the wheel. Existing code can easily be integrated without touching it. Usually it takes 1-3 lines of code to write a wrapper on a type that can be made known to the interpreter with a navigator. The interaction with existing solutions is not merely tolerated but actually desired.

7. *Achievements from a research perspective:* When this project started, it was by no means clear whether  symbolic metaprogramming was practically feasible. The project encountered many areas were compilers are not as well-tested and stable as normally expected. Furthermore, symbol tables naturally become very large, especially when recursive Trivium Lisp functions are defined and applied. This means the construction of a high-level metaprogramming language needs a well-balanced compromise between friendly semantics for its user and a sufficiently low syntactic burden for the C++-compiler to keep the compilation times and memory needs of the compiler at a reasonable level. It is was a positive surprise that practical usability has been reached with Trivium Lisp.


Metaprogramming with symbolic expressions is by no means a novel idea, as it has been intensively used by the Lisp community in the 1970s and the 1980s. Hardly any idea presented here is new or particularly sophisticated when viewed in isolation, but the progress and novelty that we claim is founded upon the practical combination of these ideas in the context of C++ and the engineering choices and decisions that made this combination possible.



## Outline of the Documentation

1. Prerequisites from Symbolic Programming
2. Symbolic Expressions
3. Trivium Lisp
4. The Symbolic Representation of the C++ type-system
5. Libraries of Trivium Lisp Functions



# Symbolic Programming
Trivium Lisp is a homoiconic symbolic programming language, which operates on symbolic expressions that represent C++ types. In this section we give a short yet exact meaning to the following notions: *symbolic expressions*, *symbolic programming language*, *homoiconicity*.



A *symbolic expression*  with *symbolic atoms* in a set `X`, also synonymously called an *s-expression*, is a finite tree, whose leaf nodes are labelled with elements from X. Symbolic expressions are usually written as nested lists. For example, when x y z are elements of X, then the expression 
```
    T:  (x (y z (a b) ) x)
```
denotes the following labelled tree:
```
        .
      / | \
T': x   .   x
      / | \
     y  z  .
          / \
         a   b
```

The inductive definition: 

1. Every object of X is a symbolic expression.
2. For every (possibly empty) finite sequence of symbolic expressions `q1, ... , qn` the term `p = (q1 ... qn)` is a symbolic expression, too.


Sometimes we call a symbolic atom an *atomic expression* or merely an *atom*. An expression that is not atomic is generally called a *non-atomic expression*.



A *symbolic programming language* is a programming language whose programs denote transformations on symbolic expressions. It is called a *homoiconic language* when its semantics is a partial map of the form 

```Eval:  Expr  --->  Expr. 	```

Here `Expr` is the set of symbolic expressions over symbolic atoms in a fixed set X, which acts both as a structure in which programs are syntactically formed and and as the structure in which programs are interpreted.


If we evaluate a symbolic expression of the form `px = (p x)` by computing
```
q = Eval[(p x)],
```
then `p` can be understood as a program that is endowed with input value `x` . The output `q`, the program `p`, and the input values `x` have in common that they are all represented as symbolic expressions.

In short, we can say that programs and data are identically represented: They are symbolic expressions.

This property is called *homoiconicity*. A programming langauge that satisfies the property of homoiconicity, such as Trivium Lisp, or any other common Lisp, is called a *homoiconic* programming language.


# Remainder of the Documentation: coming soon.




