# TRIVIUM: A LIBRARY FOR SYMBOLIC METAPROGRAMMING IN C++


*Metaprogrammed code in C++ can be as simple, clear, reusable, modular and configurable as code that is written in a functional language like Lisp or Haskell.*


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
3. map.hpp:                   a compile-time dictionary to hold symbol tables;
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

                (@'C++ ( subst ( @'Symbolic @'term )
                               ( @'Symbolic @'from ) 
                               ( @'Symbolic @'to   ) ) )
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
( @'C++ ( subst ( @'Symbolic @'term )
                ( @'Symbolic @'from )
                ( @'Symbolic @'to   ) )).
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
would be identical with the original type
```
    foo< static_cast< std::allocator<int>* >(nullptr) >.
```



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
typename std::conditional_t<  std::is_integral_v<T>
                           ,  s_expr<"true">
                           ,  s_expr<"false">
                           >;
```


Then we can compute the desired class template:


```
template<  typename X  >
using result =  
lt::eval<  

    "(  @'C++ ( list 
                'class_template 
                ( if (@'is_integral @'x)  @'f  @'g ) 
                @'x  ))"

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
4. The Symbolic Representation of the C++ typesystem
5. Libraries of Trivium Lisp Functions



# Prerequisites From Symbolic Programming
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


Sometimes we call a symbolic atom an *atomic expression* or merely an *atom*. An expression that is not atomic is generally called a *non-atomic expression*.



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



# Abstract Syntax and Semantics of Trivium Lisp.

This chapter contains a description of the syntax and the semantics of Trivium Lisp from an abstract perspective that is independent from the textual representation of source code. We describe the language as a transformation
```
    Eval:  Expr  ---> Expr
```
on expressions that are represented as abstract syntax trees. A concrete EBNF description of Trivium Lisp and its grammar will be given in the next chapter.


## Trivium Lisp Expressions

The following items are the symbolic atoms of Trivium Lisp:

1. Keywords:
```
apply    and    C++    cons    drop_first    eval    eq
first    if    irreducible    not    or    quote requires
symbolic    xor    true    false
==    !=    <    <=    >    >=    +    -    *    /    %
```


2. C++-types;

3. Identifiers;

4. The empty list `()`;

5. A symbol `Y*`, which we shall call the "recursion symbol".


We assume that each atom can only belong to one of the previous categories. 

We shall pick identifiers to be ascii-strings that satsify certain constarints. This shall be described in the next chapter. At this point we only need to note that identifiers give a pool of atomic symbols.


The set `Expr` of Trivium Lisp expressions consists the symbolic expressions over the symbolic atoms.


## Semantics


### Symbolic Atoms



For a symbolic atom `p` we let

```
Eval[  p  ]  = p
```
whenever `p` one of the following items:

a) the empty list `()`
b) the recursion symbol `Y*`
c) an instance of the following class templates defined in the framework:
```
namespace lt
{
    template<  auto x  >
    using value = ... ;
 
 
    template< unsigned n, template< typename... > class F>
    struct combinator;
 

    template< unsigned n >
    struct list;
    
    
    template<  typename... > struct map;
}
```

When `p` is none of the atomic symbols in a) - c), then `Eval[p]` is not defined, unless other definitions that follow apply.




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

Pick  to arbitrary expressions x and y. The evaluation
```
Eval[ (eq x y )]
```
is defined if and only if the evaluations `Eval[x]` and `Eval[y]` are defined. 
Then we let  `Eval[ (eq x y) ] = true` whenever `Eval[x]` and `Eval[y]`  are identical expressions. If the evaluations are not identical, then we let `Eval[(eq x y)] = false`.



### Boolean Operators

```
( not x )
( and x y )
( or x y )
( xor x y )
```

The evaluation of the expressions above is defined if and only if x and y are expressions that evaluate to either true or false. The return value of Eval is then either true or false in accordance with the standard boolean comparisons.

Note that expressions like `(and 1 1)` or `(and 1 true)` are not admissible, because 1 does not evaluate to the string "true" or  to the string "false".


### Comparisons


```
( == x y )
( != x y )
( <  x y )
( <= x y )
( >  x y )
( >= x y )
```




Let `op` be one of the operators  ==, !=, <, >, <=, >=.  The expression `(op x y)` is defined if and only the following conditions hold:

1. There are C++-values `a` and `b` to satisfy `Eval[x] = lt::value<a>` and `Eval[y] = lt::value<b>`

2. The C++-expression (a op b) is defined, evaluable at compile-time, and convertible to bool at compile-time.

When the evaluation is defined, then we let

```
    Eval[ (op x y) ] = true     when a op b holds,
                     = false    when a op b does not hold.
```


### Binary Operators

```
( + x y )
( - x y )
( * x y )
( / x y )
( % x y )
```

Let `op` be one of the operators  ==, !=, <, >, <=, >=.  The expression `(op x y)` is defined if and only the following conditions hold:

1. There are C++-values `a` and `b` to satisfy `Eval[x] = lt::value<a>` and `Eval[y] = lt::value<b>`

2. The C++-type  lt::value< a op b > exists. (This means that `a op b` is defined and evaluable at compile-time, and that the resulting value can be a non-type template parameter).

When the evaluation is defined, then we let
```
    Eval[ ( op x y ) ] =  lt::value< a op b >.
```

Example:  We witness 
```
    Eval [ ( + lt::value<int{2}> lt::value<int{3}> ) ] = lt::value< int{5} >
```



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

gives the C++ representation of an symbolic expression `x` that encodes a C++ type.

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

We give a sequence p... of symbolic expressions and consider the expression
`( lt::list<n> p... )`:

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



## The Recursion Symbol Y*

The atomic symbol Y* is not meant to be used by a programmer directly. It is a mere auxiliary tool to enable recursive functions. It is, as we shall discuss later, implicitly present in recursive code.

We endow it with the meaning
```
Eval[ (Y* f x) ] = Eval[  f (Y* f) x)  ].
```


### Substitutions and Definitions

Fix two expressions p, q and an atomic expression x. We write  <q/x>p for the unique expression that arises by replacing every occurrence of x in p by q. We call the operation <q/x> the *substitution* of `x` by `q`.

```
<q/x>p = q                           for p atomic with p = x.
<q/x>p = q                           for p atomic with p != x.
<q/x>p = ( <q/x>r0, ... , <q/x>rn )  for p = (r0, ...,  rn).
```


Fix a function `c: Expr --->  { true false }`, giving a (partially defined) condition on expressions. We define the *conditional substitution* `<x/q/c>` of `x` by `q` under the conditon `c` by the following identities:

```
<q/x/c>p   undefined for c(p) undefined;

<q/x/c>p = p  for c(p) = false;

<q/x/c>x = q  for  c(x) = true;
<q/x/c>y = y  for an atomic expression y that is different from x;

<q/x/c>( r0 .... rn )  = (  <q/x/c>r0  ... <q/x/c>rn  ).
```



Assume that `f` is an identifier and that `p` and `q` are arbitrary expressions. 

We give a condition  `no_name_clash<f>: Expr --->  Expr` that shows whether a symbol 'f' has a locally defined meaning, which must be preserved, or not.


If `p` is an expression of the form `p = (def f q)` or `p = (def f q r)`, then the local meaning of `f` given by `q`, shall be preserved. Then we let `no_name_clash<f>[p] = false`. Otherwise, `f` can be safely be replace with an outer definition. We let `no_name_clash<f>[p] = true`.



We can now describe the intended meaning  of an expression `expr = (def f p q)`:

1. Associate the expression `p` with the name `f`
2. Replace every appearance of f in q with the expression p, but do not overwrite a local inner definition of `f` if one arises.

A naive specification would read as

```
Eval[ (def f p q ) ] = Eval[  < p / f / name_clash<f> >q ].  
```


Such a specification however would not allow us to write recursive functions, where f appears in p. For this to happen we need an expression y(p; f) that takes the role of a fixed point combinator. We discuss this later. For now it suffices to say that we specify

```
Eval[ (def f p q ) ] = Eval[  < y(p;f) / f / name_clash<f> >q ].
```

We also note that y(p;f) will be picked to coincide with p when f does not appear in p.



## Name Lookup With Symbol Tables (lt::map)

```
namespace lt
{
	template< typename... > struct map;
}
```
An instantiation `lt::map< Entry... >` must have the following properties:

1. There are types `Entry::key` and `Entry::value` for every parameter Entry
2. The list `( Entry::key... )` has no duplicates


The value
```
Eval[  ( map<Entry...> p )]
```
shall be defined if and only the following properties are satisfied

1. `X = Eval[p]` is defined;
2. `X` is a C++ type;
3. There is a (necessarily unique) parameter `E` in the parameter pack `Entry...` such that `E::key` coincides with X.

Then we let
```
Eval[  ( map<Entry...> p )] = E::value;
```



## Lazy Evaluation and Currying

We have hitherto given the semantics of operations `op` through equations of the form

```
	Eval[ (op p... ) ]  = something.
```
We shall call the number of arguments in p... the *arity* of op. For instance the command `first` has the arity 1: When we give it one argument xs, then xs will be evaluated, and then the first element of the resulting list will be returned. 


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
 


*IMPORTANT NOTE*:  The `list` command has no arity.  Hence it is excluded from the rule on incomplete expressions. We give an example:

The expression `((list 1 2 3) 4)` does not evaluate to (1 2 3 4). Instead the evaluation leads to  the evaluation
```
    Eval[ (( list 1 2 3) 4) ] = Eval[ (1 2 3) 4 ] = *undefined*.
```


The command `lt::list<n>` with n!=0,  which can be read as "build a list with exactly n elements", does of course have an arity, which is `n`. Thus `lt::list<n>` is not excluded from the rule on incomplete expressions.


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

At this stage, if we completed the semantics of `def` by giving a definition for `Y*(f;p)`, using conditional substitutions and `lt::S`, `lt::K`,  we could have finished our semantic description of Trivium Lisp.


The language as we have given it now is Turing complete, but hardly usable human programmers as we do not have a method to specify functions in a readable manner.

We give some examples of functions:

1. The identity function `x --> x`.  

We let `id := (lt::S  lt::K  lt::K)`. ( We then verify that `Eval[ (id x) ]= Eval[x]` holds.

2. The function B:(x, y, z) ---> x (yz).  

we let 
```
		B := ( lt::S, (lt::K, lt::S), lt::K )
```
and verify the identity  `Eval[ (B x y z) ] = Eval[(x (y z))]` for arbitrary expressions `x, y, z`.


As we said, the representations of `id` and `B` are good for a compiler to deal with, but very bad to follow for a human programmer.


We need to add a feature to Trivium Lisp to deal with unnamed functions. For this reason we extend the set Expr of symbolic expressions to a set `λ-Expr` of *(Trivium Lisp) λ-expressions*, which arise by adjoining a symbolic atom λ that is not contained in Expr to the symbolic atoms of Trivium Lisp. Evidently `Expr` is a subset of λ-Expr.




We call a Trivium Lisp λ-expression p well-formed if it has the following form:

a) `p` is an expression in Expr
b) `p = ( q... )` with  `q...` well-formed λ-expressions
c) `p = ( (λ x q)`  with a variable `x` and a well-formed λ-expression q.


We are only interested in well-formed λ-expressions. All other λ-expressions have no meaning.



Our goal is to endow expressions of the form `p = (( λ x ) q)` with semantics satisfy  `Eval[ (p w) ] = Eval[ <w/x>p ]`.

An expression of the form `(λ x p)` will have the meaning of the anonymous  function that bind a formal parameter  `x` to a function body `p`.


PROPOSITION: Pick an arbitrary pair `(x, q)` with `x` an identifier and `q` an expression in Expr. We can select an expression `fun[x, q]` in Expr to satisfy the following property:
```
   Eval[ (fun(x,q)  t )} =  Eval[ <t/x> q ].
```
We can choose the selection `fun[ , ]` to be computable in linear time.



We use the selection `fun` to construct a function 
```
    Reduce: λ-Expr ---> Expr,
```
which replaces every λ-expression by a computationally equivalent simple expression that contains no λ.

Definition by pattern rules, ordered by descending priority:
```
	Reduce[  ( λ x p) ] = Reduce[  (λ x Reduce[p]) ]  for p not in Expr;

	Reduce[  (λ x p)  ] = fun(x, p)                     for p in Expr;

	Reduce[ ( ps... ) ] = ( Reduce[ps]... );

	Reduce[ p ] = p  for p in Expr;
```


DEFINITION:  We write `p* = Reduce[p]`  and call `p*` the *reduced representation* of p.



Now we can extend the semantics  `Eval: Expr --->  Expr` to semantics on λ-expressions

```
Eval*: λ-Expr --->  Expr  c  λ-Expr,    Eval*(p) = Eval( p* ).
```



Trivium Lisp λ-expressions are, from a technically speaking mere semantic sugar, but without them, the life of a Trivium Lisp programmer would be "semantically bitter" to an extreme extent. The feature is indispensable for the creation of readable code. 

Consider the examples of the function `B` that maps parameters `x, y, z` into `x(yz)` and the identity function. With extended Trivium Lisp expressions we can write:

```
B =   (λ x ( λ y ( λ z ( x ( y z ) ) ) ) )
id =  (λ x. x)
```

At a later stage we shall reduce the number of brackets significantly. (In Trivium Lisp source code we would have used  the line `"(def B [x y z] (x(yz))"` for the map `B`.



### The Choice of y(f;p)

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
	y(f;p) = Reduce[ y(f;p) ].
```
This is an expression in `Expr`.

If now we replace the naive choice by of `Eval[ (def f p q) ]` by

```
Eval[ (def f p q ) ] = Eval[  < y(f;p) / f / name_clash<f> >q ],
```
then we have our desired recursive functions.



It is by no means a priori clear that this is well-defined. The construction of `y(f;p)`, while given formally in a correct setup here, can only be truly understood in the context of combinatory logic, its simulation of the lambda-calculus, and fixed-point caclculators. 


The intersted reader is referred to the first three chapters of *J Rodger Hindley, Introduction to Combinators and Lambda-Calculus*.




# Concrete Semantics of Trivium Lisp

We have described the syntax and the semantics of Trivium Lisp from a merely abstract perspective, where symbolic expressions are abstract syntax trees, and programs are transformations on them, having again themselves the form of abstract syntax trees.


We have not yet specified how to encode an abstract symbolic expression in a string and how to parse that string. This shall be done in the current chapter.


Trivium Lisp programs are given in zero-terminated ascii-strings.

They will be transformed into abstract symbolic expressions in three different phases:

1. Tokenization;
2. Parsing;
3. Postprocessing of the parsing result.


This chain realises a map 
```
    translate: C-Strings ---> λ-Expr
```
that translates C-strings into λ-expressions encoded by them.


## Tokenization

```
Input:  A zero-terminated C-string called `program`
Output: A sequence `t = t0, t1, ... ` of tokens.
```

CONVENTION: Single ascii-character shall be placed in single quotes. For instance we write `'a'` for the character *a*. A string of several characters will be written in double quotes. For instance we write "abc" for the sequence 'a' 'b' 'c'. Strings in double quotes are not assumed to be zero-terminated.


### Step 0:

We replace the '\0' at the end of `program` with the newline sign '\n'. We assume that `program` is free of any occurrence of '\0' from this point.


### Step 1: 

The input string is transformed into a list of tokens, with certain regions of the string that mark whitespaces and comments being discarded.


Comments are given by the EBNF-rules that follow. We use the following convention in them. 

```
not_end_of_line :=  *any character except '\n' or '\0'*
comment         := ';', {not_end_of_line}, '\n' ;
```

We generate a sequence called `pre_token_sequence` with the help of the following EBNF-rules:

```
whitespace       :=  ' '  |  '\t'  |  '\v'  |  '\f'  |  '\r'  |  '\n'  ;

whitespace_seq  := whitespace, {whitespace};

separator       :=  {  (comment | { whitespace }  }

mono_token      := '[', ']', '(', ')', '{', '}' ''', '@'

letter          :=  any character that is not
						a) a mono_token
						b) a whitespace
						c) a semicolon ';'
						d) '\0'

token           := mono_token | letter, {letter};

pre_token_sequence := { [separator], token }, [ separator ]
```

The pre_token_sequence has the form `[s0] t0 s1 ... tn [sn]`, with  the `sk` being instances of sequence, and the tk being instances of `token`. The elements `s0` and `sn` may or may not be present in pre_token_sequence. 

The sequence of tokens  `t = t0 t1... ` is our desired result of step 1.



### Step 2:  Postprocessing of Tokens

We write `t ++ r = ts...  rs...  ` for the concatenation of two token sequences `t = ts...` and `r = rs...`.

We postprocess tokens by splitting tokens of the form `"@tok"` or `"'tok"` into token sequences `'@' "tok"` and ''' "tok"` This is done with the function `split_token`.

For tokens the function `split_token` is defined by the following set of pattern matching rules, ordered by descending priority:

```
split_token[p] :=  '@'  ++ split_token[q]  for p = "@q";
split_token[p] :=  '''  ++ split_token[q]  for p = "'q";
split_token[p] :=  p                       for any other token
```

For a token list p... we let
```
split_token[ p... ] := split_token[p0] ++ ... ++ split_token[pn];
```

We apply the function `split_token` several times to the token sequence `t` from step one until we have split up all tokens of the form "@x" or "'x".

For example, the token "@'x"  is completely aplit  by applying `split_token` twice:   `split_token[ split_token[ @'x ] ]` =  '@'  '''  'x'.



## The Parser

At this stage, the following symbols are not contained as letters in any token:
```
    ';',    whitespace, '\0'
```


A single token is either an `identifier`, an `integer_literal`, or a `mono_token`. They are given by the following EBNF-rules:

```
mono_token       :=  '[', ']', '(', ')', '{', '}', '@', '''

letter           :=  any character except mono_token, ';', whitespace

identifier_begin :=  any character except 
                        (a) one of the digits 0-9
                        (b) a mono_token
                        (c) '.'

identifier :=  identifier_begin, { letter };

integer_literal := [+ -] ( hex_num | dec_num | oct_num | bin_num );

bin_digit := '0', '1';

oct_digit := '0', '1', '2', '3', '4', '5', '6', '7';

dec_digit := '0', '1', '2', '3', '4, '5', '6, '7', '8','9'

hex_digit := '0', '1', '2', '3',  '4', '5', '6', '7', '8', '9', 
             'A', 'B', 'C', 'D', 'E', 'F',
             'a', 'b', 'c', 'd', 'e', 'f';

hex_num := ('0x' | '0X'), hex_digit, { hex_digit };

dec_num := ('1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9' ) { dec_digit };

oct_num := '0', {oct_digit};

bin_num := ( '0b' | '0B' ), { bin_digit }
```




Now we can describe a parser Trivium Lisp program listings:

```
parser := atomic_epxr | macro_expr | lambda_expr | s-expr | rs-expr

atomic_expr := integer | literal | mono_token;

macro_epxr  :=  (''', program_expr) | ('@', program_expr);

lambda_expr := '[', identifier, { identifier } ']', program_expr;

s-expr      := '(', { program_expr }, ')';

rs_expr     := '{', { program_expr }, '}';
```

The term `rs-expr` stands for *right associative s-expr*. Its purpose will become clear at a later stage.



### Syntax Trees

The parser can be used to derive a concrete syntax tree. Before we do this, we formalize the notion of tree.


DEFINITION: A tree with nodes in a set `X` is one of the following:
1. an element of `X`
2. A sequence `T =  (R S1 ... Sn ), 
       where S1... Sn are other trees and R is an element in `X`.


Note that a tree over `X` is also a symbolic expression over 	`X`, but not every symbolic expression is also a tree in the sense of our definition. The empty expression `()` and symbolic expressions of the form `(p)` with a single sub-expression `p` are not valid trees in the sense for our definition.


For a tree `T  = (R S1 ...  Sn)` the node `R` is the *root* of the tree `T`, whereas `S1, ..., Sn` are subtrees with an order on them. `Sk` is the `k`-th subtree, with the root of `Sk` being the `k`-th child node of `R`. A tree with exactly one node has the form `T' = R`. The root of `T'` is `R`.


We build an abstract syntax tree `AST` from the EBNF-rules of the parser.
For every integer `n` we fix atomic symbols `<{n}>`,  and `<(n)>`. We give the  rules for `AST` inductively:

Let p... be a token sequence with n tokens.

```
rs-expr:     AST["{ p... }"]   :=  ( <{n}>  AST[p]... )
s-expr:      AST["( p... )"]   :=  ( <(n)>  AST[p]... )
λ-expr:      AST["[x ys...]p"] := ( λ x  AST["[ys...]p") )
             AST["[x]p"]       := ( λ x  AST["p"] )
integer:     AST["x"]          := lt::value<int{x}>
identifier:  AST["x"]          := x
             AST["@"]          := "@"
             AST["'"]          := "'"
```



## Transformation of Syntax Trees into Lisp λ-Expressions


Step 1: We inductively define a function `simplify` that simplifies the syntax-trees from above:

a) Removal of right-associative symbolic expressions:

In Trivium Lisp listings we use expressions of the form `{ xn ... x1 }`, which we shall call *right-associative symbolic expressions* as short-hand for `( xn ( ....( x3 ( x2 x1 ) ) ... )` in order to save brackets.

```
simplify[ ( <{1}> p)  ]      :=  (  <(1)>  simplify[p] )

simplify[ ( <{2}> p q ) ]    :=  ( <(2)>  simplify[p]  simplify[q] )

simplify[ ( <{n}> p q... ) ] :=  ( <(2)>  simplify[q] simplify[ <{n-1}> q... ] )
     for n-1 sub-expressions q...
```


b) Resolution of "'" and "@"

The symbols "'" and "@" work like Lisp macros. They will be resolved to ordinary expressions. The meaning of `"@epxr"` will be fixed as `(` "expr")`, whereas the meaning of `"'expr"` will be fixed as `(quote expr)`.

```
simplify[ ( @ p ) ] :=  ( <(2)> "`"  p )
simplify[ ( ' p ) ] :=  ( <(2)> "quote" p )
```



c) Other expressions:
```
simplify[ ( <(n)> p... ) ] = ( <(n)> simplify[p]... )
simplify[ x ] := x       for all atoms (including λ)
```


Step 2: Resolution of list commands

Subtrees of the form `( <(n)> "list"  ... )` will be transofmred to subtrees of the form `( lt::list<n> ... )`. This is done with a transformation called `resolve_list`.

The rules are ordered by descending priority:
```
resolve_list[ ( <(n)> "list" p... ) ]  :=  ( list<n>  resolve_list[p]... )
resolve_list[ ( p... ) ]               :=  ( resolve_list[p]... )
resolve_list[ p ]                      := p    for atoms p.
```

Step 3: Extraction of symbolic expressions from the syntax trees.

The resulting tree of `resolve_list` is "almost" a λ-expression. It only remains to cross out instances of the symbols `<(n)>`, which do not appear in λ-expressions. This is done by a simple operation, which we shall call `finalize` here.


The rules are orderd by descending priority:
```
finalize[ ( <(n)> p... )] := finalize[ ( p... ) ]
finalize[ ( p... ) ]      := ( finalize[p]... )
finalize[ p ]             := p          (for atomic p)
```


This was last step in the chain that realises transformation
```
    translate: C-Strings ---> λ-Expr.
```



# C++-Representations Of Symbolic Expressions

Symbolic expressions are eventually interpreted by the C++-compiler. Thus we are in a need for a representation `S_Expr:  Expr ---> C++-Types` of symbolic expressions as C++-types. The Trivium framework contains the following class templates that are used to provide this representation:

```
namespace lt
{
	template< char... > using text = ...; 

    template<  typename...  > class s {};

    template<  int n >
    using integer = value< n > ;
}
```



We define `S_Expr` by a set of pattern matching rules, ordered by descending priority. In the rules we assume that x... is an arbitrary set of n > 0 expressions. 

```
1.  S_Expr[ (list x... ) ] :=  lt::s<  lt::list<n>,  S_Expr[x]...  >
2.  S_Expr[  () ]          :=  lt::s<>
3.  S_Expr[  (list) ]      :=  lt::s<>

4.  S_Expr[ list ]         :=  lt::text< 'l','i','s','t' >

5.  S_Expr[ (x...) ]       :=  lt::s< S_Expr[x]... >

6.  S_Expr[ T ]            :=  T    for a C++-type T;
 
7.  S_Expr[ z ]            :=  lt::text< z0, z1, ... >  
         for an identifier z = z0 z1 z2... (sequence of ascii-characters).
```




We give an opposite maaping `S_Expr*: C++-Types  --->  Expr` that decodes C++-types as symbolic expressions. This is done again by a set of pattern matching rules, ordered by descending priority:

```
1. S_Expr*[ text< x... > ] = "x..." ;

2. S_Expr*[ Xs... >] =
     ( lt::list<sizeof...(Xs)>, S_Expr*[Xs]... ) ;

3. S_Expr*[ lt::s<  lt::list<n>,  Xs...  > ]  =
      (  lt::list<n>,  S_Expr*[Xs]...  )  for n != sizeof...(Xs);


4. S_Expr*[ lt::s< Xs... >  ] = ( S_Expr*[Xs]... );


5. S_Expr*[ T ] = T   for any other C++-type.
```

Note that `S_Expr` and `S_Expr*` are not classical inverses of each other, but they are pseudo-inverses. By this we mean that they satisfy the conditions
```
    S_Expr* o S_Expr o S_Expr*  =  S_Expr*
```
and
```
    S_Expr o S_Expr* o S_Expr  =  S_Expr.
```


We consider two symbolic expressions `x` and `y` with `y = S_Expr*[ S_expr[x]]` as *essentially identical* in the sense that, while they may not be literally identical, they will always describe the same entity in Trivium Lsip. They will not be distinguishable with the means of Trivium Lisp. For example, the number `x=2` and the atomic symbol 
```
    y = lt::value<2> = S_Expr*[S_Expr[2]]
``` 
are essentially identical. 

This observation motivates the definition of the map

```
InvRep:  Expr --->  Expr,   InvRep[x] = S_Expr*[S_Expr[x]]
```

For an expression `x` we call `InvRep[x]` the *invariant representative* of `x`. 

Now we can give an exact definition of what it means for two exprssions to be essentially identical:

We call two expressions `x` and `y` *essentially identical* when they have the same invariant representative `InvRep[x] = InvRep[y]`.


We note that the fact that essentially identical expressions cannot be distinguished will be a *consequence* of our subsequent semantics definition that justifies the choice of the wording "essentially identical" a posteriori.


[To be continued on 15 March 2025]