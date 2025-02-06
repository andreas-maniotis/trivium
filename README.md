# TRIVIUM:  A LIBRARY FOR SYMBOLIC METAPROGRAMMING IN C++


5 February 2025

================================================

**NOTE: THIS TEXT IS INCOMPLETE. It will be significantly expanded and updated at least once a week until it is (hopefully) ready by 24 February 2025.**

================================================


*Metaprogrammed code in C++ can be as simple, clear, reusable, modular and configurable as code that is written in a functional language like Lisp or Haskell._*


Template metaprogramming (TMP) code tends to be unfriendly to humans. The code is generally neither easy to read nor easy to write.


The Trivium framework gives a solution to this problem by organising TMP indirectly by the means of Trivium Lisp, a symbolic domain specific language (DSL) for metaprogramming.  Metaprograms are not encoded directly in C++, but as symbolic expressions in Trivium-Lisp.

This indirection allows us to hide all the “ugly parts” of TMP in the implementation of the Trivium Lisp interpreter, which is a “universal” C++ template metaprogram to interpret Trivium Lisp expressions as C++ types.


The metaprograms themselves, written in Trivium Lisp, become as clean and as simple as programs written in any reasonable functional language, such as Haskell or Lisp.


The framework consists of three components:

1. The DSL *Trivium Lisp* together with an interpreter.
2. A symbolic representation of the C++ type system.
3. A simple closure mechanism that connects Trivium Lisp-metaprograms with the C++-type system.



## Requirements
C++ 20, no dependencies on any other library. Currently the library compiles with Gcc >= 13.3.1, Clang >= 16.0.6, Msvc 19.42 VS 17.12.


## Download
[https://www.github.com/andreas-maniotis/libtrivium](https://www.github.com/andreas-maniotis/trivium)

## Structure:
The framework consists of seven header files that are contained in the folder include/lt:

1. text.hpp:                  compile-time strings;
2. type_hull.hpp:             a wrapper for C++ types;
3. map.hpp:                   a compile-time dictionary;
4. symbolic_expressions.hpp:  symbolic expressions (and lambda expressions);
5. symbolic_type_system.hpp   a symbolic representation of the C++ type system;
6. interpreter.hpp:           an interpreter for Trivium Lisp;
7. corelib.hpp:               a library of Trivium Lisp functions (written in Trivium Lisp).




## Building Examples and Unit Tests
At the moment only Linux/Unix makefiles are provided.

1. Examples:   In the folder src/examples  type `make -j$nproc`.
2. Unit Tests: In the folder src/selftest  type `make -j$nproc`.



## A First Example

Imagine that we have a class template called *my_alloc* and that we want to replace every occurrence of *std::allocator* by *my_alloc* in an arbitrary type *X*. This problem is solved by the following metaprogram:


```
template<  typename Type
        ,  template< typename > class Old_Alloc
        ,  template< typename > class New_Alloc
        ,  auto closure  =
            map{  lt::assign< "from"
                           ,  lt::class_template< Old_Alloc > >
              ,  lt::assign<   "to"
                           ,  lt::class_template< New_Alloc > >
              ,  lt::assign< "term"
                           ,  Type >}

        >
using replace =
lt::eval< R"({  ;  Recursive function for the 
                ;  reursive replacement of subterms,
                ;  defined in Trivium Lisp

                ( def subst  [ from to term ]                                  
                (   if  ( eq term from )
                    target
                    (   if  ( irreducible term ) 
                        term
                        ( cons  ( subst from to (first term) )
                                ( subst from to (drop_first term) )
                        )
                    )
                )

                ;  Apply the function to arguments from the closure:

                (@'C++ ( subst ( @'Symbolic @'term )  ( @'Symbolic @'from )  (@'Symbolic @'to ) ))
             })"
        , closure  // connect type parameters to the metaprogram 
>;
```

If for instance we encounter a class X with a member function type  
```
    T = std::vector<int> (X::*)(double,  std::vector<double>) &&,
```
then `replace< T, std::allocator, my_alloc >` evaluates to the desired type
```
    std::vector<int, my_alloc<int>> (X::*)(
                          double, std::vector<double, my_alloc<double>) &&.
```

In the example we can see the all three layers of Trivium in action:
1. `lt::eval` is the interpreter that evaluates a Trivium Lisp program. It takes two arguments, a metaprogram in Trivium Lisp and a map that acts as a symbol table. 
2. Symbols are mapped into C++-types with a compile-time map. 
3. In this way we build a closure that connects our metaprogram to the C++ types that we want to process. The expression `(@'Symbolic x)` returns a symbolic representation of a C++ type that is bound to a variable `x`, whereas the expression `(@'C++ x)` translates a symbolic representation of a C++ into the actual C++ type that is represented. This is usually the end result of a metaprogrammatic computation.


Consider the expression 

```
(@'C++ ( subst ( @'Symbolic @'term )  ( @'Symbolic @'from )  (@'Symbolic @'to ) )).
```

It is evaluated in three steps:

1. We compute the symbolic representations of the arguments `@'term`, `@'from` and, `@'to`
2. We call subst on these representations
3. We extract the actual C++ type that is symbolically represented by the result of subst



### Limits of the Symbolic Representations

The symbolic representation of the C++ typesystem has limits. It is possible to to decompose every type `T` that is not identical with std::decay_t<T> into subtypes. Furthermore, it is also possible to decompose instantiations of template classes of the form 
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
would be identical with the original 
`foo< static_cast< std::allocator<int>* >(nullptr) >`.



### The Role of the Symbol @

We have ssen that the evaluator for Trivium Lisp takes to arguments, a metaprogram and a map that acts as a symbol table for the enclosure of prog.

For a Trivium Lisp expression `expr` that evaluates to a string (sequence of characters) x, the expression `@expr` evaluates to the value `x` in the symbol table. If we attempt to evaluate `@expr` and `x` has no entry in the symbol table, then a compilation error arises. If `expr` is a name, then we need a program whose only action it is to return the name `expr`. In all lisp dialects we write `'expr`. This process is called *quoting*.



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
using Is_Integral = 
typename std::conditional_t<  std::is_integral_v<T>, s_expr<"true">,  s_expr<"false">  >;
```


Then we can compute the desired class template:


```
template<  typename X  >
using result =  
lt::eval<  

    "(  @'C++ ( list 'class_template ( if (@'is_integral @'x)  @'f  @'g ) @'x  ))"

    ,  map{  lt::assign< "f",  lt::class_template<f> >
          ,  lt::assign< "g",  lt::class_template<g> >
          ,  lt::assign< "x",  X  >
          ,  lt::assign< "is_integral",  lt::combinator<1, Is_Integral>  >
          }
>;
```

We see that no indirection is needed. The symbolic representations of `f<x>` and `g<x>` are present, and the one not chosen is being discarded before an instantiation takes place. The Trivium-Lisp if statement is, like all of its other functions, as lazy as possible.


Of course in this small use case, which is of little complexity, the traditional approach with `std::is_integral_v` fares better than the solution with Trivium, because there is almost no algorithmic complexity in the computation involved. The effort of spelling out the closure and writing a wrapper for std::is_integral outweighs the disadvantage of the clumsy construction with apply_to. However the algorithmic component of TMP code is only modestly complex, then Trivium fares far better.


For example, let us assume that we want to select all the types that are integral from the parameter pack of an instantiated type `struct f< xs... >` and then apply `f` to the resulting sub-parameter pack. For instance `f<int, double, short>` would be transformed into `f<int, short>`. This is done easily with a library metafunction select that is written in Trivium Lisp:


```
template<  typename FX  >
using result = 
lt::with< lt::lib<"++">, lt::lib<"select">   >::
cpp_eval<  "( ++  (take 2 @'fx)  (select @'is_integral ( drop 2 @'fx )) ) "
            ,  lt::assign< "fx",  lt::symbolic<FX>
            ,  lt::assign< "is_integral", lt::combinator<1, Is_Integral>  >
```




Another important point that makes template metaprogramming difficult is the complexity of the substitution scheme for template specialisations. Unexpected ambiguities or undesired substitutions may arise, whereas the evaluation scheme of Trivium Lisp is extremely simple and easily predictable.



## Achievements

0. Simplicity: Metaprogramming has been separated from the peculiarities of the type system of C++. Thus anyone who merely knows what a template is and either knows basic Lisp (without macros) or is willing to learn it can metaprogram with Trivium. Furthermore, the metaprograms are short, clean, with intuitive semantics and without the clutter of a cumbersome syntax that is typical for conventional template metaprograms.


_Advantages that arise from the separation of data and algorithms:_

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
4. The Symbolic Representation of the C++ typesystem
5. Libraries of Trivium Lisp Functions



# Prerequisites From Symbolic Programming.
The Trivium Framework is founded upon the key ideas of symbolic programming. Hence we give a short but precise meaning to the notion of *symbolic expressions* and symbolic computing..



A *symbolic expression* with *symbolic atoms* is a finite tree, whose leaf nodes are labelled with elements from X. Symbolic expressions are usually written as nested lists. For example, when x y z are elements of X, then the expression 
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

1. Every object of X is an s-expression.
2. For every (possibly empty) finite sequence q1 ... qn of s-expressions the term `p = (q1 ... qn)` is an s-expression, too.



A *symbolic programming language* is a programming language, whose programs denote transformations on symbolic expressions. It is called a *homoiconic language* when its semantics is a partial map of the form 

```Eval:  Expr  --->  Expr, 	```

where `Expr` is the set of symbolic expressions over symbolic atoms in a fixed set X.

If we evaluate a symbolic expression of the form `px = (p xs...)` by computing
```
q = Eval[ (p xs... )],
```
then `p` can be understood as a program that is endowed with input values `xs...` . The output `q`, the program `p`, and the input values xs... have in common that they are all represented as symbolic expressions.

In short we can say that programs and data are identically represented: They are symbolic expressions.

This property is called *homoiconicity*. A programming langauge that satisfies the property of homoiconicity, such as Trivium Lisp, or any other common Lisp, is called a *homoiconic* programming language.



# Syntax and Semantics of Trivium Lisp.

This chapter contains a descritpion of the semantics of Trivium Lisp. First we describe its set Expr of symbolic expressions by specifying its symbolic atoms. Then we give rules for computing the semantic map
```
    Eval: Expr ---->  Expr.
```


## Symbolic Atoms

### Keywords
The following ASCII-strings are reserved Trivium Lisp keywords:
```
apply    and    C++    cons    drop_first    eval    eq
first    if    irreducible    not    or    requires
symbolic    xor    true    false
==    !=    <    <=    >    >=    +    -    *    /    %
```
We assume that these strings be not zero-terminated.



### Number Literals
A string is called a number literal if it is a C++ integer literal that contains no integer suffix.

(For instance `0xa` is a number literal in Trivium Lisp, but `1ul` is not).


We assume that number literals are not zero-terminated.


### Identifiers

An identifier is an ASCII-string with the following properties:

```
1. It does not contain any whitespace symbol  (' ', '\t', '\v', '\f', '\r', '\n')

2. It is not a proper prefix of a C++-number literal.

3. It starts with none of the symbols [ ( { @ '
```



### Atomic Expressions

An *atomic expression* in Trivium Lisp is one of the following:

``` 
(a) A C++-type.

(b) The empty expression ().

(c) A keyword, a number literal, or an identifier.

(d) A symbol Y* that is neither an ascii-sequence nor any of the other symbols defined in (a)-(c)
```



## The Semantics of Elementary Trivium Lisp Expressions


### Constants
We let

```
Eval [ x ]  = x
```

when `x` is one of the following expressions:

```
a) a keyword;

b) an identifier;

c) a number literal

d) the empty list nil = ()


e) One of following C++-types and 
   instantiations of the following template 
   that are defined in the framework:


namespace lt
{
	struct S;
	struct K;

	template<  auto N,  template< typename... > class >
	struct combinator {};
	
	
	template<  typename...  > 
	class map
	{
		...;
	};
	
	
	template<  auto  >
	struct list {};
}
```


### Quotations
```
Eval[ (quote x)  ]  = x
```


The command "quote" is very important. Its purpose is to interpret every symbolic expression `x` as the program `return the expression x`.


We use the following abbreviation in Trivium Lisp that is common in all Lisp dialects:
```

			'x :=  (quote x).
```



### Elementary List Commands  (list / cons / first / drop_first)


A letter `p...` followed by three dots shall denote a possibly empty sequence of expressions.


```
Eval[  (list p... ) ]      =  ( Eval(p)... ). 
```
This includes `Eval[ (list) ] = ()` as a special case.


Let x, xs, h, t... be s-expressions:

```
Eval[  (cons x xs ) ]       =  CONS( Eval(x)  Eval(xs) )

with CONS given by

CONS( h (t...) ) = (h t...)
CONS( h w )      =  -undefined- when w is not a list of the form (t...).
```


```
Eval(  (first xs ) )       =  FIRST( eval(xs) )

with FIRST given by

FIRST[ (h t...) ] = h
FIRST[ w ] = -undefined- when w is not a list of the form (h t...).
```


```
Eval(  (drop_first xs ) )       =  DROP_FIRST( Eval(xs) )

with DROP_FIRST given by

DROP_FIRST[ (h t...) ] = (t...)
DROP_FIRST[ w ] = -undefined- when w is not a list of the form (h t...).
```



### eval
The eval command enforces the evaluation of a symbolic expression.
```
    Eval[  eval(x) ] = Eval[ Eval[x] ]
```

We give an example:

```
    Eval[ (list 'head '(1 2 3))] = ( head ( 1 2 3 ) ).
    Eval[ (eval ( list 'head '(1 2 3) ) ) ]  = 1
```


### apply
The operator apply evaluates a function on arguemnts given in a  list.
```
Eval[ (apply f xs ) ] = APPLY( Eval(f),  Eval(xs) )

APPLY( f, (xs...) )  = (f 'xs...))
APPLY( f, w )  = -undefined- when w is not a list of the form (xs...).

```


### Equality Test eq:
```
Eval[ (eq x y) ] = true  whenever Eval[x] and Eval[y] are identical.
Eval[ (eq x y) ] = false whenever Eval[x] and Eval[y] are not identical.

If either Eval[x] or Eval[y] is not defined, then (eq x y) is not defined either.
```


### Boolean Operators

```
(not x)
(and x y)
(or x y)
(xor x y)
```

The evaluation of the expressions above is defined if and only if x and y are expressions that evaluate to either true or false. The return value of Eval is then either true or false in accordance with the standard boolean comparisons.

Note that expressions like `(and 1 1)` or `(and 1 true)` are not admissible, because 1 does not evaluate to the string "true" or  to the string "false".



### Assertion Of A Condition: requires
The requires command  asserts a condition. If the condition is broken, the code will not compile.

```
Eval[(requires cond x)] = Eval[x]
```
if and only if `Eval[cond] = true`, otherwise the evaluation is undefined.


We give two examples:

```
    (requires (eq 3 4) 0 )  is undefined.
    (requires (eq 3 3) 0 )  evaluates to 0.
```


### The Conditional Statement if

Pick three expressions `cond, p, q` Then `Eval[(if cond p q)]` is defined if and only if one the following conditions holds:

```
Either  (a) Eval[cond] evaluates to true  and Eval[p] is defined
or      (b) Eval[cond] evaluates to false and Eval[q] is defined.
```
Then we witness
```
Case a:  Eval[(if cond p q)] = Eval[p].
Case b:  Eval[(if cond p q)] = Eval[q].
```



### The Keywords C++ and Symbolic:

```
Eval[ (C++ x) ]
```

gives the C++ representation of an symbolic exprssion `x` that encodes a C++ type.

```
Eval[ (Symbolic X) ]
```
gives a symbolic representation of a C++ type `X`.


The encodings of C++-types as symbolic expressions and their decodings together with the exact semantics of the keywords `C++` and `Symbolic` are discussed in the chapter on the representation of the C++ type system.




### The Class Template lt::combinator<n, F>

The framework contains the type
```
namesapce lt
{
	template<  int n,  template< typename... > class F  >;
}
```



Assume that p... consists of n expressions.

```
Eval[ ( lt::combinator< n,  F >, p... ) ] =  F<  Eval[p]... >;
```


If one of the result `Eval[p]` is not a C++ type or not defined, then the evaluation above is not defined.



### The Class Template lt::list<n>

The framework contains the class template

```
namespace lt
{
	template< int n > struct list {};
}
```

We give a sequence p... of symbolic expressions.

```
Case 1: p... contains n expressions.

	Eval[ ( lt::list<n> p... ) ]  =  ( Eval[p]... )


Case 2: p... contains fewer than n expressions

	Eval[ (lt::list<n> p... ) ] = ( lt::list<n>, p... )


Case 3: p... contains more than n expressions

	Eval[ (lt::list<n>  p...)  is not defined.
```




### The C++-Types lt::S and lt::K 

```
Eval[ (lt::S x y z) ] =  Eval[ (x z (y z) )]
Eval[ (lt::K x y) ]   =  Eval[ x ]
```

These operators may appear obscure to a reader who is unfamiliar with combinatory logic. They are in fact never directly used by a programmer. They are intermediate auxiliary items to allow for  an internal representations of functions.

We give a simple example. The identity map for can be represented by the expression `( lt::S, lt::K, lt::K)` in the sense that for an arbitrary expression x we witness

```
Eval[ (lt::S, lt::K,  lt::K x ) ]       =
Eval[ (  lt::K  x  (  lt::K  x  )  ) ]  = 
Eval[x].
```



## The Command Y*

The atomic symbol Y* is not meant to be used by a programmer directly. It is a mere auxiliary tool to enable recursive functions. It is, as we shall discuss later, implicitly present in recursive code.

We give the meaning
```
Eval[ (Y* f x) ] = Eval[  f (Y* f) x)  ].
```




### Substitutions and Definitions

Fix two elementary Trivium Lisp expressions p, q and an atomic elementary Trivum Lisp expression x. We write  <q/x>p for the unique expression that arises by replacing every occurrence of x in p by q. We call the operation <q/x> the *substitution* of `x` by `q`.

```
<q/x>p = q                           for p atomic with p = x.
<q/x>p = q                           for p atomic with p != x.
<q/x>p = ( <q/x>r0, ... , <q/x>rn )  for p = (r0, ...,  rn).
```


If `c: Expr --->  { true false }` is a (partially defined) condition on expressions, then  we give the *conditional substitution* `<x/q/c>` of `x` by `q` under the conditon `c` by the following identities:

```
<q/x/c>p undefined for c(p) undefined

<q/x/c>p = p  for c(p) = false

<q/x/c>x = q  for  c(x) = true
<q/x/c>y = y  for y atomic and different from x

<q/x/c>( r0 .... rn )  = (  <q/x/c>r0  ... <q/x/c>rn  )
```



Assume that `f` is an identifier and that `p` and `q` are arbitrary expressions. 

We give a condition  `no_name_clash<f>: S-Expr --->  S-Expr:


If `p` is an expression of the form `p = (def f r...)`, then this will have the meaning that the current meaning of `f` will be replaced. We have a name clash here and thus we let `no_name_clash<f>[ p ] =  false`. In all other cases we let `no_name_clash[p] = true`.


We describe the intended meaning `expr = (def f p q)`  has the following meaning:

1. Associate the expression `p` with the name `f`
2. Replace every appearance of f in q with the expression p, but do not overwrite a local inner definition of `f` if one arises.

A naive specification would read as

```
Eval[ (def f p q ) ] = Eval[  < p / f / name_clash<f> >q ].  
```


Such a specification however would not allow us to write recursive functions, where f appears in p. For this to happen we need an expression y*(p; f) that takes the role of a fixed point operator. We discuss this later. For now it suffices to say that we specify

```
Eval[ (def f p q ) ] = Eval[  < y*(p;f) / f / name_clash<f> >q ].
```

We also note that y*(p;f) will be picked to coincide with p when f does not appear in p.



## Lazy Evaluation and Currying

We have hitherto given the semantics of operations `op` through equations of the form

```
	Eval[ (op p... ) ]  = something.
```
We shall the number of arguments in p... the *arity* of op. For instance the command `first` has the arity 1: When we give it one argument xs, then xs will be evaluated, and then the first element of the resulting list will be returned. 


The operator `if` has the arity 3. A complete if-expression has the form `(if cond alternative_1 alternative_2)`


Only for the the command `list` we cannot define an arity, because the number of elements a list takes is not fixed.



### Rule On Incomplete Expressions


We shall allow to bind less than the number of arguments given by the arity of a command to it. 

Then the evaluation will be chosen to be lazy in the sense that the arguments are kept unevaluated until we bind all the remaining arguments for  to reach the arity of the operator. We see this strategy spelled out in the following rule.


Let `op` be a command of arity `n`.

If  `p...` is a list of expressions with less then n arguments and `x` is an arbitary expression, then we let
```
Eval[ (op p... ) ]  =  ( op p... )
		
and
	
Eval[ ((op p... ) x ) ]  =  Eval[( op p... x ) ].
```
		
		
We add a restriction to the rule above for the operator def:

```
   The def operator shall never be given just one one argument. 
   It takes at least two arguments.
 ```
 
Expressions like `(def f x)` or `(def f x p)` legal, but something like `(def f)` is not.

Furthermore we reiterate that, in accordance with the definition of def, the term `f` cannot be an arbitrary expression, but it must always be an identifier.  For instance the expression `(def (first  (list 'f 'g) 'hello )` is not an evaluable Trivium Lisp program.
 
 
An expression like '(def f x )' evaluates, as expected, to itself
 ```
   		Eval[(def f x)] = (def f x).
 ```
 


*IMPORTANT NOTE*:  The `list` command has no arity.  Hence it is excluded from the rule of incomplete expressions. We give an example:

The expression `((list 1 2 3) 4)` does not evaluate to (1 2 3 4). Instead the evaluation leads to  the evaluation
```
    Eval[ (( list 1 2 3) 4) ] = Eval[ (1 2 3) 4 ] = *undefined*.
```



### Rule on Nested Expressions:

If p... and q... are non-empty lists of expressions, we let
```
	Eval[ (p... (q... )) ]  =  Eval[ Eval[(p...)]  Eval[(q...)] ].
```


### Currying

The rule on incomplete expressions and the rule on nested expressions have an important consequence. Every program except for `list` and symbolic atoms that evaluate to themselves, like `3` or `()`, can be understood as having only one input value. Consider for instance the expression `p = (cons 2 '(3 4)')`.
the command `cons` takes the input `2` and returns the program `(cons 2)`. `(cons 2)` is a program that takes again one input expression, here we give it the list `(3 4)`.  We witness
```
(2 3 4) = (cons 2 '(3 4)) = ( (cons 2) '(3 4)).
```

The procedure of breaking up a function  that takes n arguments into a sequence of n unary functions is called 'currying'. 

**In Trivium Lisp, like in Haskell, every command except `list` is curried.**

This aspect is fundamentally different from the original Lisp and its dialects. With respect to Currying Trivium Lisp follows the same strategy as Haskell.


## Unnamed Functions

At this stage, if we completed the semantics of def by giving a definition for Y*(f;p), using conditional substitutions and lt::S, lt::K,  we could have finished our semantic description of Trivium Lisp.


The language as we have given it now is Turing complete and friendly to the C++-compiler, but it is quite hostile to its human users as we do not have a method to specify functions in a readable manner.

We give some examples of functions:

1. The identity function `x --> x`.  

We let `id := (lt::S  lt::K  lt::K)`. ( We then verify that `Eval[ (id x) ]= Eval[x]` holds.

2. The function B:(x, y, z) ---> x (yz).  

we let 
```
		B := ( lt::S, (lt::K, lt::S), lt::K )
```
and verify the identity  `Eval[ (B x y z) ] = Eval[x (y z)]` for arbitrary expressions `x, y, z`.


As we said, the representations of `id` and `B` are good for a compiler to deal with, but very bad to follow by a human programmer.


We need to add a feature to Trivium Lisp to deal with unnamed functions. For this reason we extend the set Expr of symbolic expressions to a set Expr* of *extended Trivium Lisp expressions*:

An extended Trivium Lisp expression is either

(a) and atomic Trivium Lisp expression;
(b) an expression of the form `(ps...)` for extended Trivium Lisp expressions ps... ;
(c) an expression of the form `[x xs...]p` with  an extended Trivium Lisp `p` expression and identifiers x, xs... .


We shall write `Expr*` for the set of extended Trivium Lisp expressions.


An expression of the form `[x xs...]p` will be given the meaning of the anonymous  function that binds formal parameters  `x xs...` in a function body `p`.


We use the following fact:

For every pair `(x, q)` with `x` and identifier and `q` an expression in Expr
we can pick an expression `fun(x, q)` in Expr that satisfies the property
```
   Eval[ (fun(x,q)  t )} =  Eval[ <t/x> q ].
```
We can pick `f` to be a map that is computable in linear time.



We now give a function
```
    Reduce: Expr* ---> Expr.
```

by the foollowing pattern matching rules ordered by decreasing priority:
```
	Reduce[ p ] = p  for p in Expr

	Reduce[  [x]p  ] = fun(x, p)                 for p in Expr

	Reduce[ ( ps... ) ] = ( Reduce[ps]... )
	
	Reduce[  [x]p ] = Reduce[  [x] Reduce[p] ]  for p not in Expr

	Reduce[  [x1 ... xn x]p  = Reduce[  [x1...xn] Reduce[  [x]p ] ]
	                                            for n >= 1

```


```
Definition:  We write p* = Reduce[p]  and call p* the *reduced representation* of p.
```



Now we can extend the semantics  `Eval: Expr --->  Expr` to semantics on extended expressions
```
`Eval*: Expr* --->  Expr*
``` 
by letting
```
	Eval*(p) = Eval( p* ).
```


Extended Trivium Lisp expressions are, from a technically speaking mere semantic sugar, but without them, the life of a Trivium Lisp programmer would be "semantically bitter" to an extreme extent. The feature is indispensable for the creation of readable code. 

Consider the examples of the function `B` that maps parameters `x, y, z` into `x(yz)` and the identity function. With extended Trivium Lisp expressions we can write:

```
B =   [x y z] ( x ( y z ) )
id =  [x]x
```


### The Choice of y*(f;p)

Let us assume that the semantics for defining a named objects we use the "naive"  definition that does not permit recursion:

```
Eval[ (def f p q ) ] = Eval[  < p / f / name_clash<f> >q ].  
```


With this pick and all the previous formulas for `Eval` we have a complete semantic description of `Eval` and `Eval*`, or for (extended) Trivium Lisp without recursion.

We pick 
```
    y(f;p) = ([f] p  (Y* [f]p))
```
and reduce it to
```
	y*(f;p) = Reduce[ y(f;p) ].
```
This is an expression in `Expr`.

If now we replace the naive choice by of `Eval[ (def f p q) ]` by

```
Eval[ (def f p q ) ] = Eval[  < y*(f;p) / f / name_clash<f> >q ],
```
then we have our desired recursive functions.



It is by no means a priori clear that this is well-defined. The construction of `y*(f;p)`, while given formally in a correct setup here, can only be truly understood in the context of combinatory logic, its simulation of the λ-calculus, and fixed-point caclculators. 


The intersted reader is referred to the first three chapters of *Hindley*




## Representations of Symbolic Expressions

We have discussed Trivium Lisp in a merely abstract setup without consideration how we represent expressions in C++. This representation will now be discussed.

The framework contains two class templates:

```
namespace lt
{
    template<  typename...  > struct s {};

	template< char... > using text = ...; 
	
	template< int N >
	using integer = ... ;
	
}
```



We define a function `CppRep: Expr --->  C++-Types` by a set of pattern matching rules, ordered by decending priority:

We assume that x... is an arbitrary set of n > 0 elements.

```
1.  CppRep[ (list x... ) ] =  lt::s<  lt::list<n>,  Rep[x]...  >
2.  CppRep[  () ]          =  lt::s<>
3.  CppRep[  (list) ]      =  lt::s<>

4.  CppRep[ list ]         =  lt::text< 'l','i','s','t' >

5.  CppRep[ (x...) ]       =  lt::s< Rep[x]... >

6.  CppRep[ z ]            = integer<n>
    for z an integer literal
 
7.  CppRep[ z ]            = lt::text< z0, z1, ... >  
         for an identifier z = z0 z1 z2... 
```

Consider the expression p = `(list 2 -3 list abc)`. The first rule that matches is rule 1. Thus we gain

`CppRep[p] =  lt::s< lt::list<4>, Rep[2], Rep[-3], Rep[list], Rep[abc] >`.

For the inner 'list' argument, rule 2 applies, for the integer arguments rule 4 applies. For 'abc' rule 5 applies. We conclude

```
CppRep[p] = lt::s<  lt::list<4>
                 ,  lt::integer<2>
                 ,  lt::integer<-3>
                 ,  lt::text<'l','i','s','t'>
                 ,  lt::text<'a','b','c'>
                 > 
```


Now we give a "opposite" partial function
```
	SymRep:  C++-Types  ---> Expr
```

For types `Xs...` and ascii-values `txt...` we let
```
	SymRep[ lt::s< Xs... > ]  = ( Symrep[Xs]... )
	SymRep[ lt::integer<n> ]  =  n  (as an integer literal)
	SymRep[ text< txt... > ]  =  txt... ( as a character literal )
```

For  n = sizeof...(Xs) we let
```
    SymRep[ s<  lt::list<n>, Xs... > ] = ( list  SymRep[Xs]... )
```

For n < sizeof...(Xs) we let
```
    SymRep[ s< lt::list<n>, Xs... > ] = ( lt::list<n>,  SymRep[Xs]... )
```

The case n > sizeof...(Xs) is invalid input.

For any other C++-type `X` we let
```
    SymRep[ X ] = X.
```


The maps SymRep and CppRep are not inverses of each other. This requirement would be too strict to be practical. Instead they are pseudo-inverses. They satisfy the identities
```
   SymRep o CppRep o SymRep  =  SymRep
   CppRep o SymRep o CppRep  =  CppRep
````

This is enough to give Trivium Lisp a "meaning in C++"


### s_expr: The C++-Representation Of Symbolic Expressions


[TO BE CONTINUED AT 10 FEBRUARY 2025]