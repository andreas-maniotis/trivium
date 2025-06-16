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




------------------- broken from here.
# Trivium Lisp Expressions

We define the set `Expr` of symbolic expressions of the language Trivium Lisp and its semantics, which is given by an interpreter function `Eval: Expr ---> Expr`.




***Identifiers:*** We call an ascii-string *x* an identifier when it satisfies the following properties:

(a) *x* contains no whitespace character;

(b) *x* contains none of the following symbols: `{` `}` `[` `]` `(`  `)`

(c) *x* does not start with any of the following symbols: `;` `'`  `@` ` `

(d) *x* does not contain a prefix that is also a prefix of a C++-number literal (ie something like  `0x`, `7`, `.2` etc).



***Keywords:*** Trivium Lisp keywords are organised in four classes: regular keywords, the definition keyword, list keywords, and combinatory keywords. Every keyword `x`, except the keywords `"def"`, `"list"` and `"quote"`, has a natural number `arity[x]` assigned to it. We call this number the ***arity*** of  `x`


1. ***Regular keywords***, *ordered by* ***arity:***

```
no arity:    def    list    quote    `


arity 0:     true    false 


arity 1:     C++           drop_first     eval    evaluable
             first         irreducible    not     requires
             show_error    symbolic


arity 2:     apply          and            cons    eq
             if_evaluable   or             xor     ==
             !=             <              <=      >
             >=             +              -       *
             /              %

arity 3:    if
```


2. ***The definition keyword:*** `def`

3. ***List keywords:*** A list keyword is either...

... the ascii string `list`, which has no arity, or...

... an instance of the following class template:
```
namespace lt
{
	template< unsigned n > struct list{};
}

arity[ lt::list<n> ] := n   for every integer n >=0
```



4. ***Combinator keywords***: 
A combinator keyword is either one of the types `lt::S, lt::K, lt::Y`, or an instance of the class template lt::combinator defined below:

```
namespace lt
{
	struct S{};   // arity[S] = 3
	struct K{};   // arity[K] = 2
	struct Y{};   // arity[Y] = 2
	
	template<  template<  typename... > class F
	        ,  unsigned n
	        > 
	class combinator {};    // arity[ combinator<F,n> = n ]
}
```

***Variables:*** An identifier that is not a regular keyword and not the list keyword `list` is called a *variable*. (Note that variables are not keywords).


***Atomic Trivium Lisp Expressions:*** An atomic expression is one of the following:

1) Every keyword except the list-keyword `"list"`;
2) a variable;
3) an integer;
3) a C++ type;
4) The empty list `()`.



***Trivium Lisp expressions*** are defined inductively. Let `p` an arbitary Trivum Lisp expression and let `q...` be a possibly empty list of Trivium Lisp expressions that have already been defined. Let `x1 ... xn` be a non-empty list orf variables. The following items are then Trivium Lisp expressions, too:

1. Every atomic expression;

2. `'p`    and    `@p`;
 
3. `[ x1 ... xn ]p`;

4. `( q... )`;

4. `{ q... }`.


We write `Expr` for the set of Trivium Lisp expressions.



***Definition Expressions:*** We call a non-atomic expression that starts with the keyword `def` a definition expression. Every definition expression must either have the form
```
	(def f p)    or    (def f p q),
```
where `f` is a variable and `p, q` are arbitrary expressions.


***Unnamed Function Expressions:*** An expression of the form `f := [x]p` is called an unnamed function expression in one variable. The term `x` is a variable that may occur in the expression `p`. Here `p` can be considered as the body of a function, whereas `x` is a a formal input parameter. When we evaluate an expression of the form `(f q)`, then we expect to see
```
	Eval[ (f q) ] = Eval[ [q/x]f ],
```
where `[q/x]f` is the expression that arises when every "free occurrence of `x` in the term `f` is replaced by the input parameter `q`.  An occurrence of `x` is *free* in an expression if it is not bound in an expression.

An occurrence of a  variable `x` is said to be *bound* in a term `f` if either it appears in a sub-expression of the form  `[x1...xn]q` of `f` as one of the variables `x1, ..., xn`, or it appears in a sub-expression of `p` that has the form `(def x m)` or `(def x m n)`. If


***Examples:***

1.  `f := (x (def x 2 (* x x) )`. The first occurrence of `x` is free, the other occurrences are bound.

2. `f := [x y](* x z)`.  (The only occurrence of) `x` is bound, whereas (the only occurrence of) `z` is free in `f`.



***Substitution of Free Variables in Expressions:***  The procedure of replacing all free occurrences of a variable `x` by an expression `X` is carried out inductively:


1. `[X/x]x           := X`;

2. `[X/x]p           := p` for every atomic expression `p != x`;

3. `[X/x]([x]p)      := [x]p`;

4. `[X/x]([y]p)      := [y] [X/x]p`    for `y != x`;

5. `[X/x]( def x p ) := ( def x p )`;

   `[X/x]( def x p q ) := (def x p q)`;

6. `[X/x]( def y p ) := ( def y [X/x]p )`  for y `y != x`;

   `[X/x]( def y p q ) := ( def y [X/x]p  [X/x]q )`;

7. `[X/x] ( M N )    := ( [X/x]M [X/x]N )` for arbitrary expressions `M, N`.


***Remark on functions in several variables:*** We have not defined the substitution for function expressions `f := [x1..xn]p` of several variables. This is not necessary, because such an expression will be interpreted as an alias for a curried function expression `f' := [x1]...[xn]p`.


The rules (3) and (5) say that when a meaning is attached to a variable in an inner scope, then this should not be overwritten by an outer scope. An outer scope should not break into an inner scope.


*Explanations for (3):* Consider the function `e2 := [f]( f 2 )`, which evaluates every function `f` at the value 2. We let `i := [y]y` denote the identity function. Then:
```
	Eval[  (e2 i) ]  =  Eval[  [i/f](f 2)  ]  =  Eval[  (  [i/f]f  [i/f]2  )  ]
	                 =  Eval[  (  i  2  ) ]
	                 =  Eval[  (  [y]y 2 ) ]
	                 =  Eval[   [2/y]y  ]
	                 =  Eval[  2  ]
	                 =  2.
```

The choice of the variable `y` in `i := [y]y` is arbitrary. If a library implementer, who knows nothing about the variables used in `e2` choses `i := [f]f` then we witness:
```
	Eval[  (e2 i) ]  =  Eval[  [i/f](f 2)  ]  =  Eval[  (  [i/f][f]f  [i/f]2  )  
```
Here we have a name collision in `[i/f][f]f`. In accordance to the rule (3) The replacement `[i/f]` is ignored here. We witness
```
[i/f][f]f
```
If we did not ingore the replacement, then the semantic behaviour would be erratic: Letting `[i/f][f]f = [f][i/f]f = [f]i` would lead to
```
Eval[ (e2 i) ] = Eval[  ( [i/f][f]f  [i/f]  2 ) = Eval[ ( [f]i  2 ) ] = i.
```

In short: The semantics `(e2 i)` with `i` the identity would depend on whether whe chose the definition `i := [x]x`  or `i := [f]f` for the identity map. This is not what we want. Thus have protected the inner scope with rule (3)


*Explanations for (5):* This rule is about protecting inner scopes in definitions. An expression of the form `(def x p)` means that the variable `x` shall be bound to the term `p`. Such a definition should not be overwritten from outside.



## Macro Expansion and Simplified Expressions

We an expression a ***macro expression*** if it contains a sub-expression of one of the following forms:
```
	'expr,    @expr,    [x1 ... xn]expr,    or { expr... }
```
These expressions are not directly evaluated, but first another expression is computed from them, which is then evaluated. We call them macro expressions, because their behaviour is analogous to macros in Lisp. However, macro expressions are not user-defined in Trivium Lisp. Only the built-in macros above exist.


The Trivium Lisp interpreter acts in two phasses. First a procedure `expand` is applied to an expression, which expands all macros, that is it computes the expressions denoted by them. Then, this expanded expression is evaluated.


We call an expression an ***expanded expression***, when no macro expression appears as a sub-expression of it. We write `ExpExpr` for the set of expanded expressions. Note that the set of expanded expressions consists exactly of all symbolic expressions whose atoms are atomic Trivium Lisp expressions.


***The expansion map:*** The expansion map
 ```
 	Expand:  Expr ---> ExpExpr
``` 	

is given inductively. Let `expr, p, q` be expressions, let `r...` be an arbitrary non-empty sequence of expressions, and let `x`, `x1, ... xn` be arbitrary variables. Then:

```
	Expand[ (p...) ]            :=  ( expand[p]... );
	Expand[ {} ]                :=  ();
	Expand[ {p}]                := ( Expand[p] );
	Expand[ { p q }]            := ( Expand[p]  Expand[q] );
	Expand[ { p q r... } ]      := ( p ( q Expand[{ r... }] );

	Expand[ ( list p... ) ]     := ( lt::list<sizeof...(p)>, Expand[p]... )

	Expand[ expr ]              :=  expr                    for expr atomic;
	Expand[ 'expr ]             :=  ( quote Expand[expr] );
	Expand[ @expr ]             :=  ( `     Expand[expr] );
	Expand[ [x x1...xn ]expr ]  :=  Expand[ [x][x1...xn]expr  ];
	Expand[ [x]expr ]           :=  λx*[ Expand[expr] ]  (with λx* defined below).
	
```
Unnamed function expressions are expanded with the help of a map 
```
	λx*: ExpExpr --->  ExpExpr.
```
Its exact form is not important. It only matters that `λx*` has the correct semantic effect, which is described by the identity
```
	Eval[ (λx*[expr] q) ] = Eval[ [q/x]expr ].     (+)
```

We give the syntactic construction of `λx*`, which encodes the function call with the help of combinators `lt::S`  and `lt::K`. The semantics of `lt::S` and `lt::K` will then later be defined to make the equation `(+)` hold. From a user's perspective, the exact form of `λx*` is unimportant. Only two facts 
matter:

1. The equation `(+)` is satsified.

2. The algorithm to compute λx* is simple. Hence the map `Expand` can be computed without consuming too many resources by the compiler.




*The exact definition of `λx*: ExpExpr ---> ExpExpr`:*


1. `λx*[x] := ( lt::S lt::K  lt::K )`

2. `λx*[p] := ( lt::K p )` for every atomic expression `p !:= x`

3. `λx*[ ( U x ) ] := U`  when `x` does not appear in `U`

4. `λx*[ (U V) ] := ( lt::S  λx*[U]  λx*[V] )` when neither (1) nor (3) applies.



***PROPOSITION:***  The function `Expand` is invariant on expanded expressions, i. e. it satisfies
```
	Expand[ expr ] = expr 
```
for every expression `expr` in `ExpExpr`. As a consequence of this fact, `Expand` is idempotent, i e 
```
	Expand[ Expand[expr] ] = Expand[expr]
```
holds for every expression `expr` in `Expr`.



### The Uses of Macro Expressions



***Right-Associative List-Expressions:***

We call an expression of the form `expr = { P1... Pn }` a *right-associative* list expression. This is because by the simplification map it will be expanded to a list of the form 
```
 	S_Expr[expr] = (p1 ( p2 ( ... ( p{r-1} pr ) ... ) ),  with pk = S_Expr[Pk]
```
by the simplification map.

Right-associative maps are very useful in programs that contain cascades of definition. For instance consider the expression
```
	f := {  ( def x 2 )  (def y 3)  [z]( + (* x z) y )  }
```
This expression evaluates to the function, which takes an argument `z` and return `x * z + y`, with `x` defined as `2` and `y` defined as `3`. 

If we replace the curly brackets by round brackets, we calculate an incorrect result, because ordinary list expressions are left-associative.

The  expression `(op x y)` would be interpreted as `((op x) y)`. 

The result:

```
	Eval[ ( (def x 2)  (def y 3 ) [z]( + (* x z) y )  ) ] =
	Eval[ (def x 2 ( def y 3) ) [z]( + (* x z) y )  ) ]  =
	Eval[ (def y 3) [z]( + (* x z) y )  ) ] ] =
	[z]( + (* x z) 3)
```

The correct definition with round brackets is given with an extra-pair of round brackets to enforce right-associativity:
```
	f := (  ( def x 2 ) ( (def y 3)  [z]( + (* x z) y ) ) )
```

This might be acceptable in very simple cases, but it becomes unreadable even in small realistic cases that may contain two, three or more locally defined entities.



## Evaluation and Expansion

We have already mentioned that the interpreter `Eval` works in to phases. First expressions are expanded. The macro-free expressions resulting from this expansion steps are then further processed:
```
	Eval[ expr ] = Eval'[ expand[expr] ],            (++)
```
where `Eval': ExpExpr ---> Expr` the interpreter `Eval` restricted to expanded expressions. Thus `Eval` is completely defined by its action on expanded expressions.

The interpreter will be defined in a way that it maps expanded expressions on expanded expressions. Hence it restricts to a map `Eval': ExpExpr ---> ExpExpr`

As a direct consequence of the equation `(++)`, for every expression `expr` we witness

```
	Eval[expr] == Eval[ Expand[expr] ].              (+++)
```


The equations `(+++)` implies that the interpreter is fully specified when
Expanded expressions can be seen as an intermediate language that is structurally simpler than the full language `Expr`. It has, also by `(+++)`, the same expressive power as the full language `Expr`.


Expanded expressions, being structurally simple, are simple to interpret. But the same property, extreme structural simplicity, makes them extremely difficult if not impossible to deal with directly by a human programmer. This gap between the needs of the compiler and the human user is overcome by the `Expand`-function.



## Representations of Trivium Lisp Expressions in C++

Metaprograms and their inputs, given as Trivium Lisp expressions are to be interpreted by the C++-compiler. For this purpose we need a representation of Trivium Lisp expressions in C++. We shall give such a representation
```
	C++Rep: ExpExpr ---> C++Types,
```
which is then naturally extended to a representation of all expressions
```
	C++Rep: Expr --->  C++types
```
by letting 
```
	C++Rep[expr] := C++Rep[ expand[expr] ]
```
for macro-expressions.



***Keywords an Variables:***  Keywords and variables are represented by the class template lt::text form the header text.hpp, which gives compile-time strings.

For an identifier `x` made of a sequence of ascii-characters `x1 ... xn` we agree on
```
	S_Expr[ x ] := lt::text< x1, ..., xn >
```

*The details of* `lt::text:`
```
namespace lt
{
	template<  char... Text  >
	struct t
	{
		template< template< char... >  F  >
		using interpret_as = F< Text... >;
	};
	
	template<>
	struct t<>
	{
		template< unsigned N >
		struct literal
		{
			char const content[N];
			
			// ... other constructors
			
			constexpr literal( char (&input)[N] ) noexcept
			// copies input into content at compile-time
			
			template<  template< char... > T  >
			using as_type =  ... ; // T<xs...>  with xs.. charaters
			                       // in content.
		};
		
		
		template< unsigned N >
		literal(  char const (&)[N]  ) -> literal< N >; // CTAD
		
		
		template<  char... src  >
		literal(  t< src... > ) -> literal< sizeof...(src) + 1 > // CTAD
		
		
	}
	
	
	template<  typename... char Text  >
	using text = t< Text... >;
	
	
	template<  char... x,  char... y  >
	constexpr bool operator==(  text<x...>,  text<y...>  ) noexcept;
	
	
	template< t<>::literal Text >
	constexpr auto operator ""_text();
}

```

Three things that we need to remember:

1) A call of the form `"abc"_text` of the literal operator `""_text` returns the type `lt::text<'a',',b','c'>` at compile-time. This is, in most cases, everything that we need to now about `text.hpp`. 

2) We can compare compile-time texts with the comparison operator.

3) If users want to define their own literal operators that turn string literals into other types, this can be done easily.

Assume that `template< char... input > struct F` is given by the user and produces a C++-type from textual input.

We can define a literal operator "`_make_F`" as follows:

```
template< lt::text<>::literal Input >
constexpr auto operator""_make_F() noexcept
{
	return  text<>::as_type< F, Input >();
}
```


***Integers:*** Integers are repserented by the class template alias `lt::integer. For an integer `n` we let
 
```
	S_Expr[n] := integer<n>;
```
 
 
The details of 	`lt::integer` (symbolic_expresions.hpp):

```
namespace lt
{
	template<  auto const x  >
	struct v
	{
		using type = decltype(x);
	};
	
		
	template< auto const x >
	using value = v<x>;


	template<int n>
	using integer = value<n>;
}
```

The class template `lt::value` is not only used to represent integers, but it is used in other contexts, too.




***C++-Types:*** Every C++-type is an atomic Trivium Lisp expresison. Its representation is trivial: Every C++-type `X` is represented by itself:

```
	S_Expr[X] := X
```



***Lists***:

Lists are represented with the help of the class template `lt::s`, given in symbolic_epxresions.hpp:

```
namespace lt
{
	template<  typename... >
	struct s {};
}
```
We let

```
	S_Expr[ ( xs... ) ] := lt::s<  S_Expr[xs]... >;
```



***Non-Uniqueness of `S_Expr`:*** Note that the map `S_Expr` is not unique. 
If we are given a Trivium Lisp expression that `P` that is not a C++-type, then we witness `P != S_Expr[P]`, but the C++-types `S_Expr[P] and `and `S_Expr[S_Expr[P]] = S_Expr[P]` are identical.

For greater concreteness consider the expression `P = (1 2 3)`, which is not a C++-type and the C++-type `p` given by
```
	p := S_Expr[P] :=: lt::s<  lt::integer<1>, lt::integer<2>, lt::integer<3> >
```
The expressions `P` and `p` are not identical but they satisfy 
```
	S_Expr[P] :=: p = S_expr[p].
```

In short: *Several different Trivium Lisp expressions are represented by the same C++-type*. This ambiguity however is not problematic, because the semantics of Trivium Lisp is defined in such a way that two expressions are only represented by the same C++-type when they "represent" the same thing. For instance the integer `3` and `lt::value<3>` can both be considered as representations of the number '3', and in every context that can be thought of, replacing one with the other has not visible effect. We shall state this observation with greater exactness:


**DEFINITION**: We call two expressions `p, q` in `Expr`  ***semantically equivalent** when they satisfy the equation `S_Expr[p] = S_Expr[q]`.


**PROPOSITION:** Let `p` be an expression in `Expr` with a subexpression `q`.
Assume that  `q'` is an expression that is semantically equivalent to `q` and that `p'` is the expression taht arises from `p` by replacing one occurrence of `q` by `q'`. Then the expresison `p'` is semanticallly equivalent to `p`.



# A Parser For Trivium Lisp Expressions

We have presented Trivium Lisp expressions with the help of abstract tokens, such as variables, the symbols `(`, `)`, `{` `}`, C++-types etc. But eventually we need a mean to transform a compile-time string literal into a representation of a symbolic expression by the means of the C++-type system when we want to use the C++-compiler to evaluate metaprograms.


Trivium contains a tokenizer 
```
	tokenizer: CompileTimeStrings ---> TokenSeq
``` 
that generates a token sequence from a listing given as a compile-time string.


Trivium also contains a parser
```
	parser: TokenSeq ---> Expr
```
that generates Trivium Lisp expressions from token sequences.


A symbolic expression is represented in C++
```
	template< lt::text<>::literal listing, ... >
	using s_expr =
  	cpp_rep[ S_Expr[ parser[tokenizer[listing] ] ] ]
```


The given use of `lt::s_expr` does not allow yet to use C++ types as atomic subterms in Trivium Lisp expressions. This is achieved by a second parameter. We shall dicuss this later.



### The Tokenizer

```
Input:  A zero-terminated compile-time string called `program`
Output: A sequence `t = t0, t1, ... ` of tokens.
```

CONVENTION: Single ascii-character shall be placed in single quotes. For instance we write `'a'` for the character *a*. A string of several characters will be written in double quotes. For instance we write "abc" for the sequence 'a' 'b' 'c'. Strings in double quotes are not assumed to be zero-terminated.


***Tokenizer Step 0:***

We replace the character '`\0`' at the end of `program` with the newline sign '`\n`'. We assume that `program` is free of any occurrence of '`\0`' from this point.


***Tokenizer Step 1:*** 

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



***Tokenizer Step 2:  Postprocessing of Tokens***

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



### The Parser

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



***The complete parser:***


```
parser := atomic_epxr | macro_expr | lambda_expr | s-expr | rs-expr

atomic_expr := integer | literal | mono_token;

macro_epxr  :=  (''', program_expr) | ('@', program_expr);

lambda_expr := '[', identifier, { identifier } ']', program_expr;

s-expr      := '(', { program_expr }, ')';

rs_expr     := '{', { program_expr }, '}';
```

The term `rs-expr` stands for *right associative s-expr*. Its purpose will become clear at a later stage.


***A Parsing Example:***
Consider the expression `expr := {[x]xx 5}`. We witness 
```
	lt::s_expr<"{[x]xx 5}"> = C++Rep[  Expand[ { [x]xx 5 } ]  ]

	= C++Rep[ ( lt::S (lt::S lt::K lt::K) (lt::S lt::K lt::K) 5 ) ]

	= C++Rep[   (5 5) ]

	= lt::s< lt::integer<5>, lt::integer<5> >
```

The intermedate steps are of no interest to users. They are merely a show-case to examplify how expanded expressions look.



## `lt::map`: A Lookup-Table for C++-Types in Symbolic Expressions

The hitherto explained uses of `lt::s_expr` allow to form symbolic expressions that only contain ascii-identifiers and numbers as atoms, but no C++-types. C++-types are added to symbolic expressions with the help of a special lookup-table class template, which is called `lt::map`. It is defined in `map.hpp`:



Given two C++ types `S` and `T`, we write `S :=: T` whenever the types are identical.


We shall call a C++ type `X` a *key-value-pair* if it has a subtype `X::key`, which we refer to as the key of `X`, and a subtype `X::value`, which we refer to as the value of `X`.

```
// map.hpp

namespace lt
{
	tmeplate< typename... Xs  >
	concept map_parameters = ... ;
}
```

A seqeuence of `Xs...` of types satisfies the concept `lt::map_parameters`, when the following conditions hold: 
(a) Each of the parameters is a key-value-pair.
(b) The pack  `typename Xs::key...` consisting of the keys contains no duplicates. 

Tis means that for every key `K` appearing in `typename Xs::key...` there is exactly one value parameter `X` in the pack `Xs...` such that `typename X::key` is identical to `K`. 


Map parameters are organized in instances of the template class `lt::map`, which realises a compile-time dictionary:


```
// map.hpp

namespace lt
{
	template<  typename... Key_Value_Pairs >
	struct map
	{
		constexpr map( Key_Value_Pairs... ) noexcept;
		constexpr map() noexcept;
		constexpr map(map const&) noexcept;
		
		
		template<  typename Key,  typename... Fallback  >
		using lookup = ...;
		
				
		template<  typename... KV_Pair >
		using add_or_eplace = ...;
		
		
		template<  typename... KV_Pair  >
		using add ... ;
				
		
		template<  tmeplate< typename... > class F
				,  typename...             Prefix
				>
		using apply = F<  Prefix..., Key_Value_Pairs... >;
	}
	
	
	template<  typename... KV  >
	map( KV... ) -> map< KV... >;  // CTAD
	
}

```


***Lookup Of Types:***

Fix a map of the form `lt::map< KV... >`. For any two types `K` and `FallbackT`, there is a  type
```
	T :=: map< KV... >::lookup< K, FallbackT >
```
which coincides either with the unique key-value pair `P` in the pack `KV...` that satisfies `P::key :=: K`, if such a key-value pair exists, or with the type `FallbackT`, if such a key-value pair does not exist.


It is also possible to provide only one parameter for `lookup`: The type
```
  T :=: map< KV... >::lookup<K>
```
coincides with teh unique key-value pari `P` in the pack `KV...` that satisfies `P::key :=: K`. It is not defined if such a key-value pair does not exist.


The static method `contains_key<K>()` returns true when the type `map< KV... >::lookup<K>` is defined, that is, when the key `K` is contained in the key-value pairs `KV...`.


***Extensions Of Maps:***

It is possible to extend maps, either by adding key-value pairs with keys that are not yet contained in the map, or by altering values for keys that exist in the map:

*Addition of new keys:*
```
	map< KV... >::add< More_KV... >  :=: map< KV...,  More_KV... >
```

The equality in the equation above is meant to hold in the symmetric sense that either both sides are defined and equal, or both sides are undefined. Both sides are defined if and only if the concept `lt::map_parameters< KV..., More_KV...>` is satisfied.


*Modification of existsing key-value pairs:*

First we explain how one new key-value pair `New_KV` is added under this scenario. If the key `New_KV::key` is not contained in the keys `typename KV::key...`, then we witness

```
	map< KV... >::add_or_replace_types< New_KV > :=: map< KV..., New_KV >
```
Otherwise, we witnes
```
	map< KV... >::add_or_replace_types< New_KV > :=: map< |KV / New_KV|... >,
```
where |KV / New_KV| is given as follows:
```
(a) | KV / New_KV | :=:
             New_KV   for  typename KV::type :=: typename New_KV::type
             KV       otherwise
```

We see that in this case exactly one parameter `KV` is replaced by by the new parameter `New_KV`, namely the one which shares the key type with `KV`.

***Processing of Key-Value-Pairs:***

The parameter pack KV... can be accessed and processed with the help of `map::apply`. For a class template of the form `template< typename... > class F`we witness the type identity
```
	map< KV...>::apply< F >  :=:  F< KV... >.
```


## The Full Description of `lt::s_expr`







# The Trivium Lisp Interpreter (Abstract Definition)

We give the full definition of the function `Eval`. As we have seen it suffices to clarify the behaviour of `Eval` on expanded expressions. Thus, without a loss of generality we assume that every expression in this section be an expanded expression. In other words, `Eval` here has the restricted form
```
	Eval: ExpExpr ---> ExpExpr.
```

In the following equation we let `p, q` denote arbitrary expanded expressions, and we let `p...`, `q...` denote arbitrary sequences of expanded expressions.


***Group 0: Constants***
```
	Eval[ lt::value<x> ] := lt::value<x>;
	Eval[ n ]            := lt::integer<n> = lt::value<int{n}>  for integers n
	Eval[ true ]         := lt::value<true>
	Eval[ false ]        := lt::value<false>
	Eval[ () ]           := ();
	Eval[ op ]           := op       for op a keyword of arity > 0;
	Eval[ lt::list<0> ]  := lt::list<0>.

```


***Group 1: General Expressions.***

a) *Evaluation of sub-expressions:*
```
	Eval[ ( p ps...) q qs... ) ] = Eval[ (p ps... q qs... ) ];
```

b) *Evaluation of sub-expressions starting with a keyword:*

If `f` is keyword with `arity[f] = 1 + sizeof...(ps) > 0`, then
```
	Eval[ ( f p ps... q qs... ) ]      = Eval[  ( Eval[ ( f p ps... ) ] q qs... )  ]
	
```

c) *Quoted Expressions*
```
	Eval[(quote x)] = x.
```



***Group 2: Definition Expressions.***

Definition expressions are evaluated with the help of the fixed-point combinator `lt::Y`. Its arity is two and its semantics is given by

Its semantics are given by
```
	Eval[ (lt::Y p q ) ] = ( p (lt::Y p ) q )
```


```
	Eval[ ( def f p q ) ]          :=  Eval[ ( def*[f p] q ) ].
```
The expression `def*[ f p ]` is realtively complex, involving the fixed-point cominbator `Y` in order to allow for recursive function definitions, where the whole named function `f` is used in `p`. Let `F` be an arbitrary variable that is not free in `p`. Then let 
```
def*[f p] := ( [F][F/f]p  ( Y [F][F/f]p ) ).
```

*The other definition expressions:*

```
	Eval[ ( def f p ) ]           := ( def f p )
	Eval[ ( def f p q r rs... ) ] := Eval[  Eval[(def f p q )] r rs... ];
```


***Group 3: Keywords of arity `>0` and `lt::list<0>`:***

Keywords of arity `>0` are subject to the *auto-currying rule*. Before describing it we give a short explanation of the process of currying:


Fix three arbitrary sets `X1, X2, Y`. A function of the form `f: X1 x X2 ---> Y` can be split into a function 
```
	[curry f]:  X1 ---> ( X2 ---> Y )
```
via the identity 
```
[curry f](x1)(x2) = f(x1 x2)
```
The value `[curry f](x1)` is the unary function `g: X2 ---> Y` given by `g(x2) = f(x1, x2)`. 

The procedure of replacing a function `f` by `[curry f]` is called **currying**.
In Trivium Lisp, like *every* function described by a keyword of arity `>0` is curried. 
In this regard, Trivium Lisp is closely related to Haskell, but fundamentally different from other Lisp dialects.


***The Auto-Currying Rule:***
If `f` is a keyword of arity `n>0`, then for any `k<n` expressions `p...` the following identity holds:
```
	Eval[ (f p... ) ] = ( f p... )
```

It may not be clear at this stage how the auto-currying rule is realted to currying at all. For now we will postpone its deeper discussion until the whole interpreter is defined. Then we give certain examples of the auto-currying rule at work.

**Note that the auto-currying rule does not apply for the keywords `def` and `list`!**


### The Semantics Of Non-Constant Keywords

***`quote`***:
```	
	Eval[ (quote expr) ] := expr
```



***`lt::Y`:***
```
	Eval[ ( lt::Y p q ) ] = Eval[ ( p ( lt::Y P ) q ) ]
```


***`lt::def`:***
(described in the section on definition expressions).



***`lt::S:`***  
```
	Eval[ ( lt::S X Y Z ) ] :=  Eval[ (X Z ( Y Z )) ]   (arity 3).
```



***`lt::K`:***
```
	Eval[ (lt::K X Y ) ] := Eval[X];  (arity 2).
```




***`lt::combinator<F, n>`***
```
	Eval[ ( lt::combinator<F,n> p1 ... pn ) ] :=  F< P1, ... Pn >
	
	with Pk = C++Rep[ Eval[pk] ]   (for k = 1..n)
```

```
	Eval[ ( lt::combinator<F, 0> ] ) ] = F<>;
```



***Elementary List Commands:***


***`lt::list<n>`:***
```
	Eval[ ( lt::list<n> p1 ... pn ) ] := ( Eval[p1] ... Eval[pn] );
	Eval[ ( lt::list<0> ) ]           := ().
```

***`cons / first / drop_first`***:

```
	Eval[ (cons x  xs... ) ] = CONS[ eval[x] eval[xs] ],
		with CONS[ p (q...) ] = (p q...)
```
If the second argument of the auxiliary funciton `CONS` is not a list of the form `(q...)`, then neither side of the equation above is defined.



```
	Eval[ (first xs) ] = FIRST[ Eval[xs] ]
		with FIRST[ (p q...) ] = p.
```


```
	Eval[ (drop_first xs) ] = DROP_FIRST[ Eval[xs] ]
		with DROP_FIRST[ (p q...) ] = ( q... ).
```


***Evaluation and application:***

```
	Eval[ (eval p) ]  = Eval[ Eval[p] ]
```

```
	Eval[ (apply q xs) ] := Eval[ (q (quote y)...)]
		if and only if (y...) = Eval[xs],
		otherwise undefined.
```

```
	Eval[ (evaluable expr) ] := true
		if Eval[expr] is defined;
		otherwise Eval[ (evaluable expr) ] = false.
```


***atomicity test:***
```
	Eval[ (irreducible p )] = IS_ATOMIC[ Eval[p] ]
	with IS_ATOMIC[expr] == true if exor is atomic;
	otherwise IS_ATOMIC[expr] == false.
```


***Boolean Operations:***
We say that an expression `p` is ***admissible in a boolean operation*** if satisfies `Eval[p] = lt::value<true>` or `Eval[p] = lt::value<false>`.

The following expressions are defined if and only if the subexpressions `p`, `q` in them  are admissible in a boolean operation:

```
	(not p)    (and p q)    (or p q)    (xor p q)
```

Letting `t_p` and `t_q` the boolean values with `Eval[p] = lt::value<t_p>` and `Eval[q] = lt::value<t_q>`, the semantics of the boolean operations are then given by


```
	Eval[ ( not p )   ]  :=  lt::value< ! t_p >;
	Eval[ ( and p q ) ]  :=  lt::value< t_p && t_q >;
	Eval[ ( or  p q ) ]  :=  lt::value< t_p || t_q >;
	Eval[ ( xor p q ) ]  :=  lt::value< t_p != t_q >.
```

***Equality Test `eq`:***

`Eval` is defined for the term `(eq p q)` if and only if `Eval[p]` and `Eval[q]` are defined.

Then the identity

```
	Eval[ (eq p q) ] == lt::value<true>
```
holds when `Eval[p]` and `Eval[q]` are identical.

Otherwise, the identity
```
	Eval[ (eq p q)  == lt::value<false
```
shall hold.


***Value Comparisons:***
Let `_x_` be one of the operators `==`, `!=`, `<` `<=`, `>`, `>=`. Then
`Eval` is defined for the term `( _x_ p q)`, if the following statements hold

1. `Eval[p]` and `Eval[q]` are defined;
2. There are values `x_p` and `x_q` with `Eval[p] = lt::value<x_p>` and `Eval[q] = lt::value<x_q>`;
3. The C++-expression `x_p _x_  x_q` is defined and its result can be statically cast to a boolean value.


Then 
```
	Eval[ ( _x_ p q ) ] := lt::value< static_cast<bool>( x_p _x_ x_q ) >;
```


***Remark**: Not the difference between `==` and `eq`. The expressions
`(eq 7 ())` is defined and yields `lt::value<false>`, whereas the expression `(== 7 ())` is not defined. When both expressions `(eq p q)` and `(== p q)` are defined for subexpressions `p` and `q`, then their evaluation results are identical. An analogous observation holds for `( != p q )` and `(not (eq p q) )`.


***Binary Operations:***
Let `_op_` be one of the operators `+`, `-`, `*`, `/`, `%`:  ***coming soon***



***Conditionals:***
`Eval` is defined for the expression `(if cond p q)`, when one of the following conditions hold:

1. `Eval` is defined for `cond`, `Eval[cond] == lt::value<true>` holds, and `Eval[p]` is defined.


2. `Eval` is defined for `cond`, `Eval[cond] == lt::value<false>` holds, and `Eval[q]` is defined.


In the first case let
```
	Eval[ (if cond p q) ] = Eval[p].
```

In the second case let
```
	Eval[ (if cond p q) ] = Eval[q].
```


***The conditional `if_evaluable:`***

```
	Eval[ (if_evaluable p q) ] := Eval[ ( if ( evaluable p ) p q ) ]
```
The conditional `if_evaluable` is defined, because for some compilers it might be faster to compil than a direct use of the (semantically) equivalent right-hand side.



***Assertions:***
```
	Eval[ (requires cond p ] = Eval[p]
```
if and only if `Eval[cond]` is defined and it evaluates to `Eval[cond] = lt::value<true>`. Otherwise, the code will not compile.



***`c++` and `symbolic`:***
`(c++ x)` returns a C++-type from a symbolic representation of it.
`(symbolic T)` generates a symbolic representation from a C++-type. The details are discussed later.


***show_error:***
If `p` is a program for which, due to an error, `Eval[p]` is not defined. We can look at the value `Eval[(show_error p)]`, which is always defined. This call evaluates "as much as possible" and shows some debugging information that can give hints about the error.  No guarantees are given about the exact form of the output.





## The Three Semantic Roads

The smantics of a programming language `L`, from a denotational point of view, are given by a map `L ---> D`, where every program is interpreted as an object in some structure `D`. 

Since Tivium Lisp is a homoiconic language, we witness `L = D = Expr`. Thus the semantics of Trivium Lsip has the form
```
	eval: Expr --->  Expr.
```

But, apart from `eval` we need to other interpreters give Trivium Lisp a meaning as a domain-specific language for template metaprogramming: We need to encode C++-types symbolically in `Expr`, and we need to decode symbolic representations of C++-types. This is done with the help of two interpreters

```
	cpp2lisp:  C++Types ----> Expr
	lisp2cpp:      Expr ----> C++Types     
```

A symbolic expression of the form `p` then denotes the template metaprogram of the form
```
<Xs...>  |---> cpp_eval[p<Xs...>]
```
where `<Xs...>` is a pack of C++-type parameters:

```
cpp_eval[p]< Xs... > :=  lisp2cpp[ eval[ (apply p ( list cpp2lisp[Xs]...) ] ]
```

The Lisp-keyword `apply` has not yet been introduced. We have not discussed the semantics of Trivium Lisp in detail yet. But we can already summarize the meaning of the right-hand side. It evaluates to
```
	lisp2cpp[  eval[ (p 'xs... ) ],
```
with the parameters `'xs...` expressions that evaluate to `lisp2cpp[Xs]...`.

We demonstrate the connection of the three maps `eval`, `lisp2cpp` and `cpp2lisp` in a diagram:

```
                 symbolic program
                                            
      Expr ----------- eval[p] --------> Expr
       Δ                                  |
       |                                  |
    cpp2lisp                           lisp2cpp
       |                                  |
       |                                  |
       |                                  V
   C++Types ....... cpp_eval[p]......> C++Types

               Template metaprogram
```


We call these maps metaphorically the *three semantic roads* to symbolic metaprogramming. In Latin, a "trivium" is the "place where three roads meet".

Example: (to be elaborated)



# The Trivium Lisp Interpreter (Concrete Definition)



<!--
## Simplified Expressions and Tacit Programming

In Trivium Lisp, the set of simplified expressions has exactly the same semantic power as the general set of expressions `Expr`. It is structurally simpler, because it is a proper subset of `Expr`.

Simplified expressions act as an intermediate language that is easy to interpret, but difficult or even impossible to deal with by a human programmer.


We shall define a map `S_Expr: Expr ---> Expr` that maps every expression `expr` into a simplified expression expr'. The transformation will be invariant under `Eval`, i. e.
```
	Eval[  S_Expr[  expr  ]  ]  =  Eval[ expr ]    for every expresison expr
```

In fact, shall define `Eval` only on simplified expressions, and then we extend it to `Expr`, enforcing the above equality by definition:
```
	Eval[ expr ] := Eval[  S_Expr[ expr ]  ].
```





## Right-Associative Expressions

If `P...` is a (possibly empty) list of Trivium Lisp expressions, then the term 
```
	Q := { P... }
```
shall be a Trivium Lisp expression, too.

The epxression `Q := { P1 ... Pn }` shall be synonymous to
```
	Q' := ( P1 ( P2 ( P3 ( ... (P{n-1} Pn) ... ) ) )
```
in the sense that `Q` can be a drop-in replacement for every use of the more cumbersome expression `Q'`. 

We call `{ P... }` a *right-associative expression*, because the round brackets in the formula for `Q'` bind to the right.


-->

## Technical Excurse: Tacit Programming in Trivium Lisp



### Combinatory Logic

Combinatory logic (CL) is a structurally simple model of computation, which is used internally in Trivium Lisp to encode functions. The set of combinatory logic term, shorter CL-terms, is defined inductively as follows:

1. CL contains two distint symbols `S` and `K`
2. CL contains a set of variables `x1, x2...`;
2. CL contains a (possibly empty) set of constants;
3. If `M` and `N` are CL-terms, then `(MN)` is a CL-term, too.

We way that a CL-term `M` weekly reduces to a CL-term `N` when





Functions are usually defined in programming languages by three entities: a function body, a function name, and a signature given as a list of variables. An unnamed function then only has a signature and a body. From a programmer's perspective, Trivium Lisp behaves exactly in this manner, but internally it usese a different strategy




possesses a function signature and a function body, but as the notion says, not a a name. We may write `[x]p` for the unnamed function that is given 



We shall write, without specifying the details at first, `[x]p` for arbitrary symbolic expressions `p`

Trivium Lisp contains two atomic expressions `S` and `K` that have special semantics. They satisfy
```
	Eval[ (K X Y)] = Eval[X]
```
and
```
	Eval[ (S X Y Z) ] = Eval[ ( X Z ( Y Z ) ) ]
```

Consider the expression `I := (S K K)`. For every expression `X` we witness 
```
	Eval[ (I X) ] = Eval[ (S K K) X ] = Eval[ ( S K K X ) ] 
	              = Eval[ ((K X)(K X)) ] 
	              = Eval[X]
```
Thus `I` evaluates to the identity map `[x]x`


Trivium Lisp contains three atomic expressions `S, K, Y,` called combinators, with very special semantics:

``` 
	Eval[ (K X Y R... )]    := Eval[ (X R...) ]
	Eval[ (S X Y Z R... ) ] := Eval[ X Z (Y Z) R... ]
	Eval[ ( X... ) Y... ]   :=
	

	Eval[((K X) Y)] := Eval[X]
	Eval[( K X )]   := (K X)
	Eval[ K ]       :=  K
	
	Eval[ (((( S X ) Y) Z) R... )  ] 
	                      :=  Eval[ Eval[ ] R... )
	Eval[ (((S X) Y) Z) ] :=  Eval[ ( X Z (Y Z)) ]
	Eval[ ((S X)Y) ]      := ((S X) Y)
	Eval[ (S X) ]         := (S X)
	Eval[ S ]             := S

	Eval[((Y F) X)]  := Eval[  (F (Y F) X )  ]
	Eval[(Y F)]      := (Y F)
	Eval[ Y ]        := Y
```

The expressions `S` and `K` are used to implement functions, whereas `Y` is a tool to allow functions to recursively call themselves.

We give a few examples of combinator expressions in action.

1. Consider `I := (S K K)`. For every expresison `p` we witness
```
	Eval[ (I p) ] = Eval[ ((S K K) p)    ] 
				  = Eval[ (((S K) K) p) ] 
				  = Eval[ ( K p (K p))  ]
				  = Eval[ ((K p) (K p)) ]
				  = Eval[ p ]
```
Hence `I` (extensionally) equals the identity map `X |---> X`

2. Consider the expression B := (S (KS) K) For expressions `X, Y, Z` we witness
```
	Eval[ (B X Y Z) ]  =  Eval[ ((S (KS) K X ) Y Z )]
	                   =  Eval[ ( (KS) X (KX) Y Z ) ]
	                   =  Eval[ ( S (KX) Y Z ) ]
	                   =  Eval[ ((KXZ) Y Z)]
	                   =  Eval[ (X ( Y Z )) ]
```
`B` can be considered to be a realisation of the curried map 
```
	X |---> ( Y |---> ( Z |--->  (X (Y Z)) ) )
```

3. Consider the expression `(S I I)`. We witness
```
	Eval[ ((S I I) X) ]  =  Eval[ ((( S I ) I ) X ) ] 
	                     =  Eval[ (I X (I X)) ]
	                     
	
```


## Unnamed Functions in Trivium Lisp And Tacit Programming
We have seen that an expression `p`  can be interpreted as a function 
```
	f_x: Expr ---> Expr
``` 
operating on an input `x` by letting `f_p[x] = Eval[(p x)]`.

Normally, functions are defined differently in programming languages, where a function body `p` is attached to a formal input variable `x`. When a function is called with an argument `X`, then every appearance of the formal parameter `x` in the function body is replace by `X`, and the new term, here to be denoted by `[X/x]p` is then evaluated.
Let us write, without specifying the details for now, `f := [x]p` for the function with the function body `p` and a formal input parameter `x`. If we put our words in a formula, then this would translate to
```
	Eval[ ( [x]p X ) ] = Eval[  [X/x]p  ].
```

We have used combinatory

We could define functions in Trivium Lisp in the same manner as it is done for other Lisp dialects, but we face the problem


1. Other Lisp dialects are not curried




## Variables and Tacit Programming
In most programming languages, functions are defined with the help of formal variable parameters that appear in their signature and their function body. When a function is called for its arguments, then the formal parameters are replaced by the arguments of the call and the body is executed. This can be seen in the following simple C++-code snippet:

```
int sum(int x, int y) { return x + y };
int z = sum(2, 3);
```



A term `p` in a symbolic programming language can be interpreted as a function `f_p` that maps expressions into other expressions via the identity
```
	f_p(q) := eval[(p q)]
```

Imagine now that a symbolic programming language has a set of atomic symbols `x...` that can act as formal parameters in function definitions and an atomic symbol `λ`




A term `p` in a symbolic programming language can be interpreted the function that maps a sequence `q1... qn` into the expression `(p q1 ... qn )`. The function associated with `p` is then the map

```
f_{p,n}:  Expr^n ---> Expr,  f_{p,n}(q1 ... qn) := Eval[(p q1 ... qn)]
```

This variable-free strategy of dealing with functions is called *tacit programming* or "variable-free programming". While there are real-word high-level programming languages, such as Forth or APL, that follow this approach, it is not what programmers are usually accustomed to. Trivium Lisp is a language that is "tacit in disguise": There is a subset `Simple_Expr` of `Expr`, whose elements are called *simple expressions* that has the same expressive power as `Expr`. Trivium Lisp reduced to this subset is a tacit language. This simplified Trivium Lisp is the subpart of the language, which the interpreter actually deals with. It serves as an intermediate language that provides the following advantages:

1. The description of the semantics of the interpreter becomes sipmle
2. The implementation becomes simple and fast, too.

Every design of a domain-specific language for template metaprogramming faces the challenge to reconcile two goal that contradict each other:


(a) The language should be nice to handle by a human programmer in the sense that it is easy to read and to write its code.

(b) The language should be structurally simple to ensure that the compiler is not overburdened when dealing with the metaprogrammed code.


Trivium builds a bridge between the first and the second requirement by providing the user an enriched set of expressions that is simplified for the interpreter. 

The simplified expressions are, accessible, but often unreadable to a human programmer, whereas the original expressions are too complex to be dealt with the limited resources of compile-time evaluation and template resolution in one step by C++-compilers.







A *simplified Trivium Lisp expression* is an s-expression over the atomic Trivium Lisp expressions.


A *Trivium-Lisp expression* is an expression that is given inductively by the following rules:

Let `expr...` be a possibly empty list of Trivium Lisp expressions:


1) Every atomic Trivium-Lisp expression is a Trivium-Lisp expression;
2) If `x` is a variable, then `@x` is a Trivium-Lisp expression;
3) If `x` is a Trivium-Lisp expression, then `'x` is a Trivium-Lisp expression;
4) A list of the form `( expr... )` is a Trivium Lisp expression.
5) A list of the form `( list expr... )` is a Trivium Lisp expression.
6) A list of the form `{ expr... }` is a Trivium Lisp expression;

7) If `expr` is a Trivium-Lisp expression and `x1 ... xn` are n variables, then
 ```
 		[ x1 ...xn ] expr
 ``` 


We shall write `Expr` for the set of of Trivium Lisp expressions and `Expr*` for the set of simplified Trivium Lisp expressions.


We refer to an  expression of the form `[x1 ... xn] expr` is an *unnamed function expression*. Such expressions are designed, as the name suggests, to describe unnamed functions, with input parameters `x1... xn` and a function body `expr`. We shall describe their syntax and semantics in detail later. For now we only need the syntactic notion of free variable: 

We call a variable `x` a *free variable* in a Trivium Lisp expression `P` if it is contained in the set `FV(P)` that is inductively defined as follows:

```
FV(c) = {}  (empty set)         if c is constant

FV(x)             :=  {x}       if x is a variable

FV((MN))          :=  FV(M) u FV(N)

FV([x1 ... xn]P)  :=  FV(P) - { x1 ... }
```


*Remark:* Only the simplified Trivium expressions fit our definition of an s-expression directly. But this constitutes no loss of generality, because we could easily interpret an expression fo the form `P = [x1 ... xn] Q` as short-hand for something like `( ([] x1 ... xn ) Q )`, where `[]` is meant to be an atomic symbol that captures variables. In a similar way we could introduce and operator `{}` with the meaning `({} x1 ... xn ) := { x1 ... xn }`.



*Convention:* From this point when we speak of expressions or simplified expressions we shall always mean Trivium Lisp expressions and simplified Trivium Lisp expressions.




*Simplified Expressions as an Intermediate Language:*

Simplified expressions, as their name suggests, are structurally simpler than general expressions in Trivium Lisp. The interpretation of an expression `x` by the Trivium Lisp interpreter `Eval: Expr ---> Expr` takes place in two steps.

First, an expression expr is simplified by the so-called simplification map 
```
	[ ]*:  Expr ---> Expr*,   expr |---> expr*
```
which we define in detail below.  Then the simplified expression `q = expr*` is interpreted by an interpreter `Eval: Expr* ---> Expr*` that does the actual work. We let
```
	Eval[ expr ] = eval[ expr* ]
```
Note that every evaluation result of `Eval` is a simplified expression. Thus we could have restricted `Eval` to a map of the form `Expr ---> Expr*.


Simplified expressions are not simplified for the human reader, but simplified for the compiler. A human reader will struggle to see the structure even in simplified expressions that encode relatively simple programs. But this is not a serious problem, because a programmer can treat every expression `expr*` as if it were the original expression `expr`. We will clarify this point after discussing the details of the simplification map.


We make use of the following C++-type, which is defined in `symbolic_expresisons.hpp`:
```
namespace lt {   template<unsigned> struct list {};  };
```
For complex expressions, like `Q:= [x y](RS)`, we use brackets of the form `[ x y ](RS) ]` todenote `Q*`.


```
Group 0:
expr*            = expr           for atoms;

Group 1: (macros)
[ @expr ]*            = ( ` expr* );
[ 'expr ]*           = ( quote x );




Group 2 (the list command)

( list M... )  = ( lt::list<n>  M*... ) for n = sizeof...(M)


Group 3: (sequences)

( M... )*      = ( M*... )
()*            = ()
{}*            = ()
{M}*           = (M*)
{M N}*         = (M* N*)
{M N R}*       = (M* (N* R*))
{M N R X... }* = (M* { N R X... }* )  for X... possibly empty.



Group 4: (functions with input variables)

(a)    [ [x]M   ]*  =  ( K[M*] ); 

(b)    [ [x]x   ]*  =  ( S K K );

(c)    [ [x]Ux  ]*  =  U          for x not free in P;

(d)    [ [x]UV  ]*  =  ( S [ [x]U ]* [ [x]V ]* ) 
                                  if neither (a) nor (c) applies;
                               

(e)    [  [x1 ... xn]P  ]* =  [  [x1]... [xn]P  ]*;



```

*Explanations for group 1 (macros):*

The notations `@expr` and `'expr` are mere syntactic sugar to reduce the number of brackets that appear in a program. They enhance readability.



*Explanations for group 2 (list command):*

In all Lisp dialects there is a command called `list`. Imagine that we are given n expressions X1 ... Xn, which evaluate to expressions x1 ... xn. The lisp expression `(list X1 ... Xn)` then evaluates to the s-expression `(x1 ... xn)`. The type `lt::list<n>` plays exactly the same role as the command `list` in ordinary Lisp, with the crucial difference that `lt::list<n>` expects exactly `n` arguments to follow for a list to be complete. This adaption is necessary in Trivium Lisp, because, in contrast to Lisp, Trivium Lisp is, like Haskell, a curried language. 

By this we mean that every function is unary. We give an example: The sum the integers `2` and `3` is computed with the expression `(+ 2 3)` in Trivium Lisp. But `+` is an unary function. The expression `(+ 2)` evaluates to the function "add 2 to the next argument".  The expression `+( 2 3)` is in fact an abbreviation for `((+ 2) 3)`.

In order to support this curried language behaviour, we cannot deal with variadic lists directly. We must fix the number of arguments that follow the `list` commands before we enter the interpreter. We do not give the number of arguments manually. Instead we add this informatino by replacing the `list` command with the more specific `lt::list<n>.`



*Explanations concerning group 3 (sequences):*

For expressions `M1, ... Mn` we witness 
```
	M_left_assoc := ( M1 ... Mn )* = ( M1* ... Mn* )
```
which is short-hand for 
```
	M_left_assoc = (... ((M1* M2*) M3* ... ) Mn).
```


In contrast to the round brackets, the curly brackets are right-associative. This means, that we see

```
	M_right_assoc := { M1 ... Mn }* = { M1* ... Mn* }
```
which is short-hand for 
```
	M_right_assoc = ( M1* ( M2* ( M3* ... (M{n-1} Mn)...))).
```


The curly expressions are extremely helpful when writing programs. The following expression `P` is a Trivium-Lisp program, whose semantics we have not defined yet, but we give sketchy idea in advance.


When the Trivium Lisp interpreter `eval` is given program of the form 
```
 P := ( (def x Z)Q ),
```
it binds the variable `x` to the term `Z`, and then it evaluates the term `[Z/x]Q`, which arises when every free occurrence of `x` is replaced by `Z` in `Q`.
In signs: 
```
	eval[ (def x Z) Q ] = eval[ [Z/x]Q ]
```

Now consider the program
```
P := ( (def x 3) ( (def y 4) (+ x y) ) )
```
The meaning of `P` is "define x as 3, then define x as 4, and then add x and y."

The evaluation path given by
```
	eval[P] = eval[  [3/x]( (def y 4)(+ x y) ) ]
	        = eval[         (def y 4)( + 3 y ) ]
	        	= eval[                  ( + 3 4 ) ]
	        	= 7
```


If we use curly brackets, then `P` can be replaced by a simpler program `P_simple`
```
P_simple := { (def x 3) (def y 4) (+ x y) }.
```


*Explanations concerning group 4 (function expressions):*

Every rule except (e) looks entirely cryptic and arbitrary to a reader who is not familiar with combinatory logic. The central idea is to "encode" unnamed functions with the help of to objects `S` and `K`. This encoding will be used by the Trivium Lisp interpreter to evaluate the unnamed function expression. The encoded unnamed function is structurally much simpler than the initial expression. 


PROPOSITION: The simplification map is idempotent, i. e. `expr** = expr*` holds for every expression `expr`.




PROPOSITION: If an expression `expr` has no free variables, then the expression `expr*` contains no variables at all.



Example:  `[  [x y z](x(yz))  ] = ( S (KS)  K )`.


*A note concerning simplified expressions:*

A programmer does in fact not need to care about simplified expressions. They do matter only in a few places where the semantics of Trivium Lisp needs to be described with exactness. But even there it merely suffices to know the motivation of the construction and the previous proposition, rather than the details , which are discussed in an appendix to this document.



Consider the equation `R* := (λx.P*)*` for `R = [ x ] P`. In `(λx.P*)*` the meaning of each of the two asterisks is different. The inner asterisk comes from the map `Expr ---> Expr*`, whereas the outer asterisk reduces the λ-expression in the brackets to a CL-expression.




*Definition:* We say that an expression `P` *reduces* to `Q`, or equivalently that `Q` is the *reduced expression* for `P`, when the identity `P* = Q` holds.


*Proposition:* 
1. The map `P |--> P*` is idempotent, i. e. it satisfies the identity `P** = P`.
2. If two expressions `P` and `Q` are α-equivalent, (i. e. if one arises from renaming bound variables in the other), then we witness `P* = Q*`.


### Tacit Programming Languages And Implementation Issues

Trivium Lisp is a "tacit programming language" in disguise. Tacit programming, also called point-free style, is a programming style in which functions do not specify their arguments as formal parameters. Instead functions merely compose other functions. While there are programming languages, such as APL and Forth, that enforce this sort of "variable-free" programming on programmers, Trivium Lisp has taken a different route. The compiler sees a tacit intermediate programming language given ty simplified expressions, whereas the human user sees a common, non-tacit language with variables. In this way we can keep the tacit language simple in structure at the cost of witnessing poor readability, whereas a user experiences good readability at the cost of higher complexity when the language is directly interpreted. The simplification map bridges this gap between programmer and compiler. Note that we need to be very careful not to overburden the compiler, because the C++-template mechanism was not designed for the heavy workload writing an entire universal interpreter (and a parser for program strings) with it. The expression simplification map  is a good trade-off mechanism to bridge this two goals that impede each other.







## A Parser for Symbolic Expressions
Our hitherto discussion of symbolic expressions does not take into account how we can actually encode them in a piece of code. We need a parser that does the following: 

1. It takes a string literal `str` as its input. (`str` somehow encodes a genrealized symbolic expresison `expr`).
2. It calculates the symbolic expression expr* (resolving λ-terms, curly brackets etc) as an intermedate value.
2. It transforms `expr*` into into the C++-type `S_Expr[expr*]`.


The parser is given by a class template (alias) called `lt::s_expr`.

We give a few example of `lt::s_expr`
```
lt::s_expr<"(1 2)">  :=: lt::s< lt::integer<1>, lt::integer<2> >
lt::s_expr<"abc">    :=: lt::text<'a,','b','c'>

lt::s_expr<"[x]xx">  :=: S_Expr[ S (S K K)(S K K) ] 
                     :=:  lt::s<  lt::S
                               ,  lt::s< lt::S, lt::K, lt::K >
                               ,  lt::s< lt::S, lt::K, lt::K >
                               >;
```

The third example show that even a modestly complex non-atomic generalized expression can have a C++-representation that is difficult to read.

We cannot directly


The parser can easily treat identifiers, but we have no direct way to enter C++-types in a string. 






takes a string literal `str` describing a symbolic expression `expr`, wh



# Fundamental Types

## Type Hulls

The header file `type_hull.hpp` contains a simple class template to wrap any type:

```
namespace lt
{
	template< typename T >
	struct th
	{
		using type = T;
	}
	
	template< typename T >
	using type_hull = th;
	
	
	template<  typename T,  typename S  >
	constexpr bool operator==( type_hull<T>, type_hull<S> ) noexcept
}
```

We refer to the instances of `lt::type_hull` as type hulls. The comparison of two type hulls `type_hull<S>` and `type_hull<T>` yields the same result as `std::is_same_v<S, T>` would, namely whether `S` and `T`  are identical types.


### The Template Alias Hack

We have defined a struct `th` and used the more descriptive template alias `type_hull` for the human reader, because mangled names of complex metaprograms can become extremely long. In fact mangled type names *are* metaprogram listings. Hence they can have the same length as program listings. In order to keep their lengths reasonable we need to compress them by having names of frequent sub-component templates as small as possible, ideally only one or two letters long. Thus compilers see `th`, whereas human program writers should use `type_hull` so that human readers see the full and desriptive tmeplate alias name. 

This hack of having short names for the compiler and longer template aliases is used virtually everywhere.


## Compile-Time Strings

The header `text.hpp` contains compile-time strings:






## Wrappers For Values, Class Templates etc...

The class templates discussed here appear in the header file `symbolic_expressions.hpp`.


### Values

We represent values as types with the help of the class template `lt::value` that is described below:

```
namespace lt
{
	template<  auto const x  >
	struct v
	{
		using  type = decltype(x);
	}
	
	template< auto const x >
	using value = v<x>;
	
	template< int const n >
	using integer = value<n>;
	
	
	// value_type removes const qualifiers
	template< auto const x >
	using value_type = typename value<x>::type;
}

```


### Class Templates

Class templates are wrapped in types with the help of `lt::class_template`:
```
naemsapce lt
{
	template<  template< typename... > class  >
	struct ct {};
	
	
	template<  tmeplate< typename... > class F  >
	using class_template = ct< F >;
}
```

We give a second wrapper for class templates that contains an integer `n > 0`, indicating the number of type parameters, with which the class template shall be instantiated at a later state:

```
namespace lt
{
	template<  unsigned n,  template< typename... > class  >
	requires( n != 0 )
	struct co {};
	
	
	template<  unsigned n,  template< typename... > class F  >
	using combinator = co< n, F >;
}

```


## Types And Tags For The Representation of Symbolic Expressions

The following classes and class templates play an important role in the representation of symbolic expressions by C++-types.

namespace lt
{
	template<  typename... >
	struct s {};
	
	template<  typename >
	struct quoted {};
	
	
	template< int n >  
	struct list {};
	
	struct S{};
	struct K{};
}

Detailed explanation of their purpose will be given later.

# Symbolic Expressions

Trivium-Lisp was designed and described with the help of combinatory logic, which is a theory that is closely related and intertwined with the better known λ-calculus. The λ-calculus is a theory of "unnamed functions". We shall gather a few facts from λ-calculus and combinatory logic by citing and paraphrasing the first two chapters of the book *Introduction to Combinators and λ-Calculus* by J. Rogoer Hindley and Jonathan P. Seldin. The reader who is already familiar with these topics can skip the current section.





## Trivium-λ-Εxpressions

A *Trivium variable* is a non-empty sequence of x ascii-letters, subject to the following conditions:

1) x contains no whitespace charaters;
2) x does not start with the symbol ' or @.
3) x does not contain any of the following symbols:  [ ] ( ) { } ;
4) There is no prefix p of x such that p is eiher a C++-floating point literal or a C++ integer literal. (For example "0xay" is not a variable, because 0xa is a hex-number in C++).

A *Trivium constant* is one of the following:

1. A C++-integer literal;
2. A C++type;

In the following, we keep constants and variables fixed, that is, when we speak of constants, then we mean Trivium constants, and when we speak of variables, then we mean Trivim variables.




λ-terms are models *unnamed functions*. They are studied in the theory of λ-calculus.









A Trivium-symbol is a sequence of ascii-characters that contains no white-space sign. A *atomic Trivium-λ-expression* is either a Trivium-symbol or a C++-type.
The set of  *Trivium-λ-expressions* is the set of all λ-expressions over the a



We inductively define the set of *extended s-expressions*, which constitute the building blocks of Trivium Lisp. We fix a set `Var` of variable symbols. An *atomic Trivium-λ-expression* is one of the following

1) A variable symbol in `Var`
2) An integer
3) A C++-type

We assume that these three clases be disjoint.


A general Trivium-λ-expression is a λ-expression, with atoms 

```
(0a) Every variable symbol is an s-expression.

(0b) Every integer is an s-expression.

(0c) Every C++-type is an s-expression.

(0d) The elements in (0a)-(0c) do not overlap. 
A symbol cannot appear in two of the categories above.

(2) If p and q are s-expressions, then the (p q) is an extended s-expression, too.

(3) If p is an s-expression and x is a variable symbol in V, then (λx.p) is an s-expression, too.
```

We agree on the following notations for the sake of readability:
```
(p1 ... pn )    := (...((p1 p2) p3 ) ...) pn)
λ(x1... xn).p   := (λx1.(λx2.(...(λxn.p))...)
λx.p            := (λx.p)
```
An (s-)expression that arises from the rules (0a)-(0c) is called an *atomic (s-)expression*.






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
apply    and    C++    cons    drop_first    eval    eq    evaluable
first    if    if_possible  irreducible    not    or    quote
requires    show_error    symbolic    xor    true    false    ==
!=    <    <=    >    >=    +    -    *    /    %
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



## evaluation_failure

When the evaluation of a symbolic expression `expr` is attempted, three scenarios are possible:

1. The evaluation of `expr` terminates with an evaluation result.
2. The evaluation never stops. (In reality, this triggers a compilation error when the maximal template depth of the compiler is exceeded).
3. The expression `expr` is not evaluable. This happens when neither (1) nor (2) applies. For instance it may happen if we attempt to evaluate an undefined symbol, or if a function is given improper arguments.


We call the third scenario an *evaluation failure*. The command
```
	(evaluation_failure expr)
```
evaluates to `true` when an attempt to evaluate `expr` would result in an evaluation failure. When an evaluation is possible (and terminates), then `false` is returned. 

(If the evaluation of `expr` does not terminate, or the evaluation exceeds the resources of the  C++-compiler (template-depth, memory, lookup tables etc), then a compilation error will arise).



## if_possible

```
	eval[ (if_possible p q) ] = eval[  ( if (evaluation_failure p) q  p) ]
```
If the evaluation of `p` fails, then we evaluate `q`. Otherwise we evaluate p.
Note that that `if_possible` is not implemented with an if-statement. 

(The `if`-statement is only given to specify the semantics of `if_possible`. It is not used to implement `if_possible`, because it has the disadvantage that `p` may be evaluated twice. The implementation of the Trivium Lisp interpreter ensures that `p` is evaluated only once).



## show_error
```
(show_error expr)
```
The command show_error is only used for debugging purposes. It makes use of the following class template to indicate errors:
```
namespace lt
{
	tmeplate<  typename...  >  struct evaluation_failure {};
}

```
The expression `(show_error expr)` evaluates to `lt::evaluation_failure<>`, when the evaluation of  `expr` would not result in an evaluation error. 

Otherwise, an instance of the form lt::evaluation_failure< Xs... > with a non-empty parameter pack `Xs...` is returned. 

The parameter gives hints to the question why the expression `expr` yields an evaluation failure.

**It is not admissible to make any assumptions on the exact nature of the pack `Xs...` except for whether it is empty (no evaluation error) or non-empty (when an evaluation error occurred).**

The parameters in the pack are merely meant to give debugging hints. They can be seen as a metaprogramming analogon for error messages from a C++-compiler.



Note that the user is allowed to treat instances of evaluation_failure as normal types that may occur in symbolic expressions as normal components. We consider two evaluations:

*Case 1: No error.*

 ```
 eval[  (show_error (quote lt::evaluation_failure<int>) ) ] =
    lt::evaluation_failure<>;
 ```

In the evaluation above no evaluation failure occurs. Thus the parameter pack of lt::evaluation_failure is empty.



*Case 2: Evaluation error occurs.*

```
eval[  (show_error  (first lt::evaluation_failure<int>))  ] =

lt::evaluation_failure< 
    lt::text<'f','i','r','s','t>,  
    lt::evaluation_failure< 	lt::evaluation_failure<int> >
 >
 ```


The parameter pack is not empty, because an evaluation error does occur. Observe the following parameter that is contained in the pack:

```
    lt::evaluation_failure< 	lt::evaluation_failure<int> >.
```

The outer instance indicates an error that arose during the attempt to evaluate the inner instance, whereas the inner instance is an ordinary parameter.

Confusion of this kind should be avoided. Thus:

*It is best to avoid using instances of lt::evaluation_failure as atomic symbolic expressions.*



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


## lt::s_expr

We have introduced the following three maps:
```
translate: C-strings ---> λ-Expr`
    Eval*: λ-Expr    ---> Expr
   S-Expr: Expr      ---> C++-Types
 ```
 
 
The framework contains a template alias that realises  the combination
`S_Expr o Eval* o translate` of these three maps:

```
lt::s_expr< str > :=: S-Expr(Eval*(translate(str)))
```

(Here we use the symbol `:=:` to denote equality between C++-types). We give some examples:

```
lt::s_expr<"3"> :=: lt::integer<3> :=: lt::value< int(3) >

lt::s_expr<"(first '(1 2 3)"> = lt::s_expr<"3">

lt::s_expr<"[x y z]x(yz)"> =  X  (explained below).
```

In the last line we have written 'X' to indicate that the exact form of the expression is not at all relevant. The only thing that matters is that for the expression `q := `[x y z]x(yz)` we witness the identity
```
(q r s t) = r (st)
```
for every choice of subexpressions `r, s, t`.

Not every s-expression can be encoded merely by C-string literals. For instance a list of C++types cannot be expressed in such a way. But it is possible to capture these types with the help of a second parameter given to the template lt::s_expr. This parameter is an instantiation of the type lt::map. For example the type

```
lt::s_expr<"'(a b a)",  lt::map{ lt::assign<"a", int>, lt::assign<"b", double> }>
```
evaluates to the list `lt::s< int, double, int >. We give the exact procedure:

[to be continued in May 2025]



## The Trivium Lisp Evaluator lt::eval
[to be written in May 2025]


# The Symbolic Representation of the C++ Type System

We shall call a C++ type `T` *reducible* when it satisfies at least one or more of the following conditions:

(a)  `T` differs from `std::decay_t<T>`.

(b)  `T` is a template instantiation of the form `T :=: F< X... >`, where F is a class template of the form `template<  typename... > class F`.

(c) T is of the type R(X...) cv noexcept(b)

(d) T is of the type R X::*

(e) T is of type T*



We call a C++ type *irreducible* when it is not reducible.

We give examples of a few reducible C++ types:

```
int

F<3, 4>                             // instantiation of a 
								   // template class with non-type parameters.

lt::class_template< std::vector >  // std::vector is a template template 
                                   // parameters

void

lt::text<'a,'b','c>
```


We give examples of irreducible C++ types

```
std::vector<int, std::allocator<int>>

int (double) &&

std::size_t (std::vector<int>::*)()  noexcept

lt::s< int, double, text<'a','b,'c'>  >
```


The symbolic representation of the C++ type system represents every reducible C++ type as a tree of representations of simpler C++ types. This tree is a symbolic expression that can be reasoned upon by Trivium Lisp. An irreducible type is represented as a trivial s-expression consistsing only of the type itself.

We observe: A C++ type is irreducible if and only if the s-expression representing it is irreducible too.

We give a pair of mappings `cpp-rep: C++-Types ---> S_Expr` and `sym-rep: S_Expr ---> `C++-Types`, which are inverses of each other.

For a C++ type `T` we call `sym-rep[T]` the *symbolic representation* of `T`, whereas, for a symbolic expression `expr` we call `cpp-rep[expr]` the *C++ type represented by `expr`*.


The function `sym-rep` is calculated for a type `T` by successively applying the following pattern substitution rules (0 - (-7)), ordered by descending priority until no substitution is possible any more:



(0) `sym-rep [ (x...)] = (x*...)` with  `x* = x` for `x` an irreducible C++-type
and `x* = sym-rep[x]` otherwise


(-1) Member pointers:
```
sym-rep[ Y X::*] = ( "::*  sym-rep[X]  sym-rep[Y] )
```



(-2) Functions:
```
sym-rep[ X(Xs...) cvr ne ] = 
( "function" ("signature" ( sym-rep[X] sym-rep[Xs]...) CVR[cvr]  NE[ne]  )

with... 

... cvr one of the values in

cvr_list:     ε / 
              const / volatile / const volatile / 
              const& / volatile& / const volatile& /
              const&& / volatile&& / const volatile&&


... CVR[cvr] the corresponding value in 

CVR_list:     ε /
              "const" / "volatile" / "const_volatile" / 
              "const&" / "volatile&" / const_volatile&" /
              "const&&" / "volatile&&" / const_volatile&&" ;


and with 

ne one of ε / noexcept  and NE[ε] = ε, NE[noexcept] = "noexcept".
```
By the "corresponding" value in the CVR_list we mean that when cvr is the n-th item in `cvr_list`, then `CVR[cvr]` is the n-th item in `CVR_list`.


(-3) References:
```
sym-rep[ X& ] = ( "&"  sym-rep[X] )
sym-rep[ X&& ] = ( "&&"  sym-rep[X] )
```

(-4) Arrays:
```
sym-rep[ X[]  ]             = ("array" sym-rep[X])
sym-rep[ X[n] ]             = ("array" sym-rep[X] lt::integer<n> )
```


(-5) Pointers:
```
sym-rep[ X* ]               = ("*" sym-rep[X])
sym-rep[ X *const ]         = ("*const" sym-rep[X])
sym-rep[ X *volatile]       = ("*volatile" sym-rep[X])
sym-rep[ X *const volatile] = ("*const volatile" sym-rep[X])
```

(-6) CV-qualified types:
```
sym-rep[ X const ]         = ( "const"  sym-rep[X] )
sym-rep[ X volatile]       = ("volatile" sym-rep[X])
sym-rep[ X const volatile] = ("const_volatile" sym-rep[X])
```


(-7) Class Templates:
``` 
sym-rep[ F<T...> ] = ( "class_template"  lt::class:template,  sym-rep<T>... )
```


We stated that the function `cpp-rep` shall be the inverse of `sym-rep`.

We give a few examples:

```
sym-rep[ int(double) noexcept] = 
("function" ("signature" int double) "noexcept")


sym-rep[ void ] = void


sym-rep[ std::size_t (std::vector<int>::*) noexcept ] =
("function" ("signature" std::size_t) noexcept )

```
Consider a struct `X` given by

```
struct X { int a;  int foo() const& { return a }; };
```

We witness   // Change this!!!
```
sym-rep[ &X::a ] =  ("::*" int  X)
sym-rep[ &X::foo ] = ("::*"  ("function" ( "signature" int ) "&" ) ).
```



# Appendix

## The Lambda Calculus And Combinatory Logic

The lambda-calculus is a collection of formal systems tha are used to describe functions and  the composition of functions. We deal with the simplest version of λ-calculus, which is called the *untyped λ-calculus*, to describe the semantics of functions in Trivium Lisp. Tightly connected to the untyped λ-calculus is the theory of combinatory logic. 

In this section we cite and paraphrase parts of the first two from *J. Roger Hindley, Jonathan P. Seldin, Introduction to Combinators and λ-calculus.* We abbreviate this source with [HINDLEY]
 (We only present the parts that are absolutely essential for the description of the semantics of Trivium Lisp).



The syntactic elements of untyped λ-calculus are the λ-terms. 

Assume that we are given an infinite set of symbols called variables, and another (empty, finite, or infinite) set of symbols called constants. The two sets shall be disjoint. The set of expressions called *λ-terms*  is defined inductively as follows:

(a) All variables and constants are called λ-terms (called atoms).

(b) If `M` and `N` are λ-terms, then `(MN)` is a λ-term (called *application*)

(c) If `M` is any λ-term and `x` is any variable, then `(λx.M)` is a λ-term (called an *abstraction*).


*Examples of lambda terms:
```
	(λx.(xy)),			(λy.y(λx.(xy)))
	(x(λx.(λx.x))), 		(λx.(yz))
```


Parenthesises will be moitted in such a way that, for example 'MNPQ' denotes `(((MN)P)Q`. (This convention is called association to the left). Other abbreviations will be 
```
	 λx.PQ         for   (λx.(PQ)),
	 λx1...xn.M    for   (λx1.(λx2.(...(λxn.M)...))).
```


### Informal Interpretation of λ-Terms
If `M` has been interpreted as a function or operator, then `(MN)` is interpreted as the result of applying `M` to an argument `N`, provided this result is defined. (The more usual notation for function application is M(N), but historically  `(MN)` has become the standard in λ-calculus and combinatory logic).

A term `(λx.M)` represents the operator whose value at an argument `N` is calculuated by substituting `N` for `x` in `M`.


### Free and Bound Variables

An occurrence of a variable `x` in a term `P` is bound if and only if it is in a part of `P` with the form `(λx.M)`, otherwise it is free. If `x` has at least one free occurrence in `P` it is called a *free variable* of `P`.



## Substitution
For any `M`, `N`, `x`, define `[N/x]M` to be the result of substituting `N` for every free occurrence of `x` in `M`, and changing bound variables to avoid clashes.  (Exact details in [HINDLEY]).


Note that the exact way of changing bound variables is not relevant. It only matters to choose new variables in such a way that name clashes are avoided.

We witness the same situation when we look at function declarations and definitions in C++. Consider a function with the signature `int f(int)`. It does not matter whether in the definition we write  `f(int x) { ... }`, or `f(int y){ ... }`, with the role of `x` taken by `y` in the second case, provided we do not have a name clash  with other named objects `x` or `y`.

We see that the two different namings of the formal function parameter in `f` do not change the semantics (and pragmatics) of `f`.

For a second example consider the two terms `λx.xx` and `λy.yy`. Both have the same computational meaning: "write out the input twice".


### Changes of Bound Variables

Let a term `P` contain an occurrence of `λx.M`, and let `y` not be free in `Μ`.
The act of replacing this occurrence of `λx.M` by 
```
	P' = λy.[y/x]M
``` 
is called a *change of a bound variable* in `P`. For two terms `M` and `N` we shall use the notation 
```
	M --α--> N
```
when `N` arises from `M` by changing a bound variable an occurrence of a subterm `P` in `M`.

to indicate the fact that `P'` has arisen from a change of a bound variable.


We have already discussed that changes of bound variables do not alter a λ-expression in a significant way.


### β-contractions
An abstraction Q := (λx.P) can be applied to an argument `R`. We can see the term `[R/x]Q`, which arises by replacing every instance of `x` in `Q` by `R` as an evaluation of `(QR)`. The procedure of replacing the term `(λx.P)R` by the term `[R/x]P` is called a β-contraction.

We shall use the notation
```
    M --β--> Ν
```
when a term `N` arises from `M` by applying a β-contraction to one occurrence of a subterm in `M`.


### η-contractions
If `x` is a variable that has no free occurrence in a λ-term `P`, then we can see the expressions `(λx.Px)` and `P` as essentially the same thing, because for every λ-term `Q` we witness
```
	((λx.Px)Q) --β--> [Q/x](Px) == (PQ)
```

We call the replacement of `(λx.Px)` by `P` an *η-contraction*.

For two terms `M` and `N` 


### λβη




Evaluating the term `QR`


This yields the term `(QR)`. The "computational result" `of applying R to Q is the term `[R/x]Q` that arises when every free occurrence of the variable `x` in `P` is replaced by `R` in `Q`. Thus the term `[N/x]M` can be seen as the evaluation result of the term `((λx.M)N)`. 


We call the process of subsituting `(λx.M)N` into [N/x]M






In the previous section on substitution we have seen that we can treat a term `P` and another term `P` that arises from `P` as a result of a change of a bound variable in `P` as essentially the same thing.

We call two λ-terms `P` and `P'` *α-equivalent* when one of the terms arises by the other from several (possibly zero) changes of bound variables. α-equivalence is, as its name suggests, an equivalence relation.






### λβη


We give an equivalence relation `β=` on the set `λ` of λ-terms. `P β= Q` shall mean that `P` and `Q` are "essentially the same thing". We have explained that a renaming of bound variables that does not clash with free variables does not alter the meaning of a λ-term `P`. Thus we require
```
(a)    P β= Q      when `P` and `Q` are α-equivalent.
```

We have also seen that a term of the form `M:=(λx.P)` can be seen as a function depending on one formal input parameter `x` and that a term of the form `(MN)` can be seen as the instruction "evaluate `M` with the input `N`. The result of the evaluation is the term `[N/x]P`. Thus we require
```
(b)    (λx.P)N  β=  [N/x]P
```

When a variable `x` is not free in a term `P`, then for the 

 as follows:

Let `x` be an arbitrary variable, and let `P`, `Q` be arbitrary terms. Then `β=`
be the smallest equivalence relation with the properties (1)-(3) below:

```
(a)	
(b)	((λx.P)Q)   β=  [P/x]Q;
(c)	(λx.Px)     β=  P      when x is not free in P.
```


Two terms `P`, `Q` with `P βη= Q` can be considered as "essentially the same thing". We informally check this claim by giving an example for each rule.

(a) The term `(λx.(x x))` means "duplicate the input given in the formal variable x". This is the same as taking `(λy.(y y))`. The choice fo the formal parameter is irrelevant. Thus: `(λx.(x x)) β= (λy.(y y))`.

(b) If `c` is a constant, then `(λx.(x x))c` can be understood as apply the function "duplicate the input" to the argument `c`. Thus: 

	(λx.(x x))c  β= [c/x](x x) == (c c)

(c) When `x` has no free occurrence in `P`, then the epxression `Q := (λx.Px)` has the following property

	φ: for every λ-term `R`, the expression `QR` β-reduces to `(PQ)`.

Note that `P` also has the property `φ`, hence: `Q β= P`.



### Combinatory Logic

Consider the closed expressions
```
	S = (λxyz.xz(yz))    and    K = (λxy.x).
```

A *compbinatory logic term* or *CL-term* is one of the following:

(a) A variable;
(b) a constant
(c) `S` or `K`
(d) `(MN)` whenever `M` and `N` are CL-terms.

We write `CL` for the set of CL-terms.

NOTATION: I := (S K K)

We  call `I` the identity combinator because it satisfies the identity `(Ix)β=x` for every λ-term.


We shall explain this statement by considering a map `*: λ-Calculus --> CL` that assigns a CL-term `P*` to every λ-term `P` such that both terms have the same "extensional behaviour".


### CL+ext: Equality for CL-Terms
We 




The construction of `*`:

```
(a) x*    := x                          for atoms x;
(b) (ΜΝ)* := (M* N*);
(c) (λx.x)* := I                        with I = (S K K);
(d) (λx.Ux)* := U                       if x is not free in U;
(e) (λx.UV)* := (S (λx.U)*  (λx.V)*)    if neither (c) nor (d) applies.
```


NOTATION: (λx*.P) := (λx.P)*.


EXAMPLES:

1. (λ*x.x)        ==  I
2. (λ*xyz. x(yz)) ==  S (KS) K
3. (λ*x.xx)       ==  S I I 

Here `==` means literal term equality. 



Combinatory logic can be understood as a "rephrasing of the λ-calculus that avoids explicit mention of variables and of abstraction". [https://ncatlab.org/nlab/show/combinatory+logic](cf ncatlab.org)













### Simulating the λ-calculus with CL

OBSERVATION: For every λ-term P the term P* contains no abstractions other than `S` and `K`.

CONSEQUENCE: If  `P` is a λ-term that contains no free variables, then `P*` is *variable-free* in the sense that is  built only from `S`, `K`, and constants.

When we want to describe a function with the help of a λ-term `P`, we are not interested in the form of `P`, but only in the action `X |--> (P X)` that is given by `P`. For example, the terms `P:= ((λx.M)Q)` and 


in the form of the function `X |--> (P X)`. In this sense it make no difference to 



EXEΤNSIONALITY: The terms `P` and `P*` have the same extensional behaviour. We cannot discuss this property with formal rigour here, but only sketch the meaning with a bit of hand-waving. The exact treatment can be found in HINDLEY, Theorem 9.14 (Equivalence theorem for βη)).


The central idea behind using combinatory logic instead of λ-expressions directly is that, when 

in the exact form of a λ-term `P` that describes a 


Two λ-expressions are said to be *extensionally equal*




Using `S` and `K` we resursively define a set *CL*, whose members we call *CL-terms*:

(a) All constants are members of *CL*

(b) `S` and `K` are members of *CL*

(c) For any two members `P`, `Q` of *CL*, the term `(PQ)` is contained in `CL`, too.
 
 
The study of `CL`-terms is called combinatory logic.
 


ADDENDUM: The exact construction of `*`:



 
 
The following theorem is critically important for the implementation and the understanding of Trivium Lisp:



PROPOSITION: There is a computable function `P |--> P*` that maps every λ-expression `P` into an element `P*` of `CL`, and is subject to the following conditions for every `P` and `Q`:


1) `x* = x`  for atoms x;
2) `S* = S`,  `K* = K`;
3) `(PQ)* = (P* Q*)`.
4) `P* = Q*` for α-equivalent λ-terms `P`, `Q` (literal identity);
5) `([N/x]M)* = ([N*/x]M)*`.
6) If `P` contains no free variables, then `P*` contains no variables, except for the ones bound in instances of  at all.


The exact form of `*` is not important, but we give it as an addendum.
The map `*` is defined inductively:


NOTATION: For a λ-term M we agree on the notation `(λ*x.M) := (λx.M)*`.

With this notation we could rewrite (c)-(e)  via
```
(c)' λ*x.x = I;
(d)' (λ*x.Ux) = U for x not free in U;
(e)' (λ*x.UV) = S (λ*x.U) (λ*x.V).
```

We give a few examples. 
1. Let I := `λ*x.x = (S K K).



### Closed λ-Terms
A *closed λ-term* is a λ-term that contains no free variables.

Examples:  Pick variables x, y, a constant c, and arbitary expressions 'M', 'N'

1) `P := λx.xy` is not a combinator, because `y` is free in `P`.

2) `λy.( λx.xy)` is a combinator.

3) `P := c` is a combinator

4) `P:=(λx.xy)(λy (λx.xy) )` is not a combinator, because in the first occurrence of 'λx.xy', the 'y' is not bound.

5) `P = MN` is a combinator if and only if both `M` and `N` are combinators

6) `P = λx.M` is a combinator if at most `x`, but no other variable is free in `M`.
