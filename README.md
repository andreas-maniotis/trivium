# TRIVIUM:  A LIBRARY FOR SYMBOLIC METAPROGRAMMING IN C++


_Metaprogrammed code in C++ can be as simple, clear, reusable, modular and configurable as code that is written in a functional language like Lisp or Haskell._ 

Trivium is a template metaprogrammaing framework that achieves these objectives through a strict separation of metaprograms and C++ types. It has three components:

1. *Trivium Lisp*, a domain-specific[^1] Lisp dialect that was designed for metaprogramming.
2. A symbolic representation of the C++ type system.
3. A structure called *navigator* that connects the interpreter of Trivium Lisp with  the symbolically represented type system.


In contrast to C++, which allows for metaprogramming by chance rather than by design, Trivium Lisp has been constructed with the explicit purpose of supporting it. Thus metaprograms become as easy to understand, write and maintain as ordinary programs written in a common high-level functional programming language such as Haskell or Lisp. 


Trivium is a header only library  written in C++20. Having no dependency on any other library, it can be used with freestanding and hosted C++ implementations. 


All the confusing syntactic and semantic peculiarities that make template metaprogramming difficult to use are not present in Trivium Lisp  metaprograms. Instead they are hidden in one "universal template metaprogram", the interpreter of the language. The metaprograms written by library users  are simple and short symbolic expressions. This fact has an important consequence:


Trivium can serve as a metaprogramming tool for everyone, not just the small group of template metaprogramming experts.


## Requirements
C++ 20, no dependencies on any other library. Currently the library compiles with Gcc >= 12.2 and Clang >= 16.0.6. There is ongoing work to support MSVC 2022. Other compilers have not been tested yet.


## Download
[https://www.github.com/andreas-maniotis/libtrivium](https://www.github.com/andreas-maniotis/trivium)


## Building Examples and Unit Tests
At the moment only Linux/Unix makefiles are provided.

1. Examples:   In the folder src/examples  type `make -j$nproc`.
2. Unit Tests: In the folder src/selftest  type `make -j$nproc`.


## A First Example

Imagine that we want to replace every occurrence of *std::allocator* by *my_alloc* in an arbitrary type *X*. The problem is solved in three steps.


*First step:  the algorithm.*   We replace *source* with *target* in a symbolic expression *term*.

```
constexpr auto code =

R"( def subst  [ source target term ]
        
    (   if  ( eq term source )

        target

        (   if  ( irreducible term ) 

            term

            ( cons  ( subst source target (first term) )
                    ( subst source target (drop_first term) )
            )
        )
    )
)"_s;  // _s is a literal operator that generates a template from this code.
```


*Second step: the navigator.* We collect everything we need in a navigator.


```
template<  typename Type
        ,  typename Old_Type
        ,  typename New_Type
        >
using nav = typename
lt::navigator<  lt::parameter<     "x"_s,  Type  >
             ,  lt::parameter<  "from"_s,  Old_Type  >
             ,  lt::parameter<    "to"_s,  New_Type  >
             ,  lt::parameter< "subst"_s,  decltype(code)  >
             >;
```

The details are not important here. We only need to observe that the navigator makes the types visible to the interpreter by creating symbolic representations of type  and binding them to name.

*Third step: Run the interpreter.* 

```
template<  typename Type
        ,  typename Old_Type
        ,  typename New_Type
        >
using replace =
lt::ts::cpp< 
    typename nav< Type,  Old_Type,  New_Type >
    template eval< " ((` 'subst)  (` 'from)  (` 'to) (` 'x)) "_s
>;
```

In the code above the interpreter returns a symbolic expression that denotes a C++ type. This C++ type is generated from the symbolic expression with the help of the class template lt::ts::cpp.



Now we can test the validity of the result:

```
#include <vector>
#include <type_traits>

using namespace std;

// alias to keep nested type names sufficiently short
template<  typename...  >
using v = vector< X... >; 


template<  typename X  >
struct my_alloc : allocator<X> { };


// A complex type:
using T =  v<int>  (foo::*)( v< v< int> > ) volatile&& noexcept

struct foo;


using result  =
replace<  T
       ,  lt::class_template< std::allocator >
       ,  lt::class_template< my_alloc >
       >;


using expected  =

v<int, my_allocator<int>> 
(foo::*)( v< v<int,  my_alloc<int>>
           , my_alloc<v< int, my_alloc<int>>>> ) volatile&& noexcept;


static_assert(  std::is_same_v<  result, expected >  );

int main() {}  ;
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
constexpr int f(int n) { 
    if constexpr (n == 0)  return 1;  
    else return n * f(n-1);
}
```
This approach, taken for instance by Boost Hana, is unproblematic, when small inputs are processed by compilers. But for contemporary C++ compilers processing values is far more expensive than processing types. The template class instantiations that arise under the hood, for instance when recursive Trivium Lisp functions  with several variables are processed, can be so large that the value based approach becomes too expensive or even unfeasible because of a massive slowdown and huge memory consumption. Trivium keeps dealing with values to an absolute minimum.



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
                   ,  template_<f>
                    , tmeplate_<g>  >::template apply_to<X>;
```


In the source code above we neither see  `g<X>` nor `f<X>` appearing directly, because the following code would not work:
```
template<  typename X  > 
using y = 
std::conditional_t<  std::is_integral_v<X>, f<X>, g<X>  >;
```

If we attempt to instantiate y<double>, then logically we want to discard f<X> and to pick g<X>, but we end up with a compilation-error because of a failed attempt to instantiate g<X>. The indirect construction with apply_to is harder to read, especially when the metaprogramming code is long.

In Trivium-Lisp the metaprogram-code would look as follows:

```
// Wrapper for std::is_integral

template< typename T >
using Is_Integral = 
typename std::conditional_t<  std::is_integral_v<T>, decltype("true"_s),  decltype("false"_s)  >;


// The navigator

using nav =
lt::navigator< lt::parameter< "is_integral"_s,  lt::combinator<1, Is_Integral > >
             , lt::parameter< "f"_s          ,  lt::class_template<f> >
             , lt::parameter< "g"_s          ,  lt::class_template<g> >
             , lt::parameter< "x"_s          ,  x  >
             >;


// run interpreter

template<  typename X  >
using result =  lt::ts::cpp< // template to transform a symbolic result to a C++ type

    // The metaprogram

    typename nav::template 
    template eval<"(  if ( (` 'is_integral) (` 'x) )  ; test with is_integral  
                         ('class_template (` 'f) (` 'x))    ; if yes return f<x> (in symbolic form)
                         ('class_template (` 'g) (` 'x))    ; if no return g<x>  (in symbolic form)
                   )"_s >
>;
```

We see that no indirection is needed. The symbolic representations of `f<x>` and `g<x>` are present, and the one not chosen is being discarded before an instantiation takes place. The Trivium-Lisp if statement is, like all of its other functions, as lazy as possible.


Another important point that makes template metaprogramming difficult is the complexity of the substitution scheme for template specialisations. Unexpected ambiguities or undesired substitutions may arise, whereas the evaluation scheme of Trivium Lisp is extremely simple and easily predictable.

*A note on the length of the example:* A reader may object that the length of the Trivium example does not compare favourably to the length of the solution that does not use the library. But we should keep in mind that Trivium was designed for large and complex metaprograms, not for small examples. 
The effort to write a navigator, which is mostly 4-10 lines long, has no weight when the metaprogramming problem to solve is complex. 
If however a solution to a problem is only two 2-5 lines long, then the effort of writing up to 10 lines for a navigator is of course significant. The example given here only aims to show that the interaction between data and algorithms is cleaner in Trivium than it is in direct C++. Only the metaprogramming code matters in this context.



# Achievements


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





# Trivium-Lisp

## Homoiconic Symbolic Programming Languages

A _symbolic expression_ with _symbolic atoms_ in a set S is a finite tree, whose leaf nodes are labelled with elements from S. Symbolic expressions are usually written as nested lists. For example when x y z are elements of S, then the expression `T:  (x (y z (a b) ) x)` denotes the following labelled tree:

```
        .
      / | \
T': x   .   x
      / | \
     y  z  .
          / \
         a   b
```
Let S-Expr be the set of all symbolic expressions over symbolic atoms in S. We write S-Expr<sup>n</sup> for the set of n-tuples (x<sub>1</sub> ... x<sub>n</sub>) of expressions in S-Expr.  Furthermore, we write S-Expr<sup>* </sup> for the set of arbitrary finite tuples of symbolic expressions.


A *symbolic programming language* is a programming language, whose programs denote transformations on symbolic expressions. A *homoiconic symbolic programming language* is a symbolic programming language whose programs and data are symbolic expressions over the same set of symbolic atoms, and which have the property that its semantics [[ ]]: S-Expr* --->  S-Expr  is given by a a partial map Eval:  S-Expr --->   S-Expr via the equivalence 

[[ p ]](q<sub>1</sub> ... q<sub>n</sub>) = Eval(  (p q<sub>1</sub> ... <sub>n</sub> )  ).

The identity is meant to hold in the symmetric sense that either both sides are defined and equal, or neither side is defined.[^2]

Trivium Lisp, like ordinary Lisp, is a homoiconic symbolic programming language.




## The Semantics of Trivium Lisp

The set of symbolic atoms of Trivium Lisp consists of the following items:

1. The empty list ();
2. ascii-strings;
3. C++ types.

***Convention: We write C++ types in italics to distinguish them from strings.*** For instance the symbolic expression (*double* double) is a list of the C++ type double followed by the string "double".




In the following description of the semantics "undefined behaviour" of a metaprogram means compilation failure. The behaviour of commands is sometimes given by equivalence relation of the form
`equation 1 <=> equation 2` between two equations. Then, if neither equation 1 nor equation 2 holds, then the behaviour is undefined and thus a compilation failure is the intended behaviour. 

On other occasions the behaviour is given by a single equation. Then the equality is meant symmetrically in the sense that the left-hand side exists if and only if the right-hand side exists.

###  Constants:


### true / false:
```
Eval( true ) = true
Eval( false) = false
```

Text constants that indicate truth and falseness.


### The Empty List ():

`Eval( () ) = ()`.


### Elementary Operations:

### quote (and '):
`Eval( (quote x) ) = x.`  We write 'x as an abbreviation for (quote x).


### list:
Create a list. 
```Eval(  (list xs... )  ) = ( Eval(xs)...)```

Example:  (list 'x 'y 'z) evaluates to (x y z).



### first:
Return the first element of a list.
```Eval(  (first z )  ) = x   <=>   Eval(z) = (x xs...)  for some xs... .```

Example:  (first '(1 2 3)) evaluates to 1.



###  drop_first:
Drop the first element of a list.
```Eval(  (drop_first z  )  = (xs...)  <=>  Eval(z) = (x xs...) for some x.```

Example:  (drop_first '(x y z)) evaluates to (y z)


### cons:
Add an element at the front of a list.
```Eval(  (cons  z  w) )  =  (x  xs...)   <=>  Eval(z) = x and Eval(w) = (xs...)  for some x xs... ```.

Example:  (cons 'x '(y z)) evaluates to (x y z).



### eval:
Evaluate a string as a lisp program.
```Eval(  (eval x)  ) =  Eval(Eval(x))```

Example: 
```
Eval(  '(first (list 'x 'y 'z))  ) =  (first (list 'x 'y 'z ) )
Eval( ( eval 'first (list 'x 'y 'z ) ) ) = Eval( first (list 'x 'y 'z) ) = x.
```


### apply:
Apply a function to arguments given in a list.
```Eval(  (apply p z )  ) = Eval(  (p xs...)  )  <=>  Eval(z) = (xs...).```

Example:  (apply cons (list ( 'a '(b c))) evaluates the expression (cons 'a '(b c)). Thus the result is the list (a b c).



### not:
```
Eval( (not x) ) = true   <=>  Eval(x) = false
Eval( (not x) ) = false  <=>  Eval(x) = true.
```
If x evaluates neither to true nor to false, then the result is undefined.



### and / or / xor:
```(and x y) / (or x y) / (xor x y)```

Undefined behaviour if either x or y does not evaluate to one of the symbolic constants true/false.
Otherwise  Eval(  ( *op* x y )  ) =  Eval(x) *op*  Eval(y).


Example:  
```
(and true false) evaluates to false
(and 0     1)    yields a compilation error.
```



### requires:
`(requires cond x)` evaluates to `Eval(x)` whenever `Eval(cond) = true`. Otherwise undefined behaviour.



### eq:
`(eq x y)` evaluates to the symbolic constant "true" when  `Eval(x)` and `Eval(y)` are identical; 
otherwise it returns to the symbolic constant "false".


### if:
```
Eval( (if cond x y) )  =  Eval(x) when cond evaluates to the symbolic constant "true",
Eval( (if cond x y) )  =  Eval(y) when cond evaluates to the symbolic constant "false"
Eval( (if ocnd x y) )  =  undefined otherwise.
```

Note that always at only one of the branches x y is evaluated.



### lt::combinator<n, F>:

Pick a list of n expressions xs... : If these expressions evaluate to C++ types *Xs*... and if the template instantiation *F<Xs...>* exists, then  ( _combinator<n, F>_ xs... ) evaluates to *F<Xs...>*. Otherwise undefined behaviour form the input xs... .


### lt::navigator< Xs...  >
Trivium ocntains the following class template that plays a special role:
```
namespace lt { template<  typename...  > class navigator; }
```

Semantics of navigators:

Eval(  ( *lt::navigator< Xs... >*,  q  )  ) = lt::navigator::lookup<  *Q*{}  >   <=>   Eval(q) = *Q*    for expressions q and a types *Q*.


This is merely a formal definition. A thorough discussion of its meaning will be given in the section on navigators.


### == / != / < / <= / > / >=:

(  **op**  x  y ) with **op** in (== / != / < / <= / > / >=). 


Trivium contains the following class template that plays a special role::
```
namespace lt {  template< auto > struct value { } };
```

If x and y are expressions that evaluate to *lt::value<v_x>*  and *lt::value<v_y>*, then ( __op__ x y )
evaluates to the symbolic string true / false if and only if the C++ expression *val_x* __op__  *val_y*  evaluates to the boolean value true / false. Otherwise the behaviour is undefined.


### + / - / * / / / %:

( **op**  x  y ) with _op_ in ( + / - / * / / / % ). 

The expression is evaluable if x and y are expressions that evaluate to *value<v_x>*  and *value<v_y>*.
Then the expression evaluates to the C++ type *lt::value< val_x __op__ val_y >*. Otherwise the behaviour is undefined.



### Anonymous Functions:
A  string x shall be called a *formal variable name* if it does not contain any of the letters (,),[,],{,}, and if it does not start with any of the letters ' or 0-9.

For Trivium Lisp Expressions p, q, and a formal variable name x we define the expression p[q/x] as the expression that arises when every occurrence of x in p is replaced by q.
Example:  (* 2 x)[3/x] is the expression (* 2 3).

For every Trivium Lisp expression p and every formal variable name x, the language selects an expression [x]p with the property that for every Trivium Lisp expression q the expressions 
 `([x]p q)` and   `p[q/x]` evaluate to the same result.

For a non-empty list of variables names xs... and a variable x such that the variables in the list `xs..., x ` are pairwise different, an expression  [xs... x]p is chosen inductively via the identity
`[xs... x]p = [xs...][x]p`.


Examples:  
```
1. [x]x is the identity map.
2. `[x y] ( (+ (*x x) y)). This expression encodes the function which maps (x, y) into x^2 + y.
```


### Definitions: (def f p q)

The parameter f needs to be a formal variable name. `(def f p q )` has the same result as an evaluation of `(p [q/f])`


Example:  (def  f  [x y](+ (* *value<2>* x) y)  (f *value<3>*  *value<4>* )  )  results in *value<10>*.

**Recursive definitions** are admissible: For instance the following code computes the factorial 3! = 6:

(def f [x] (if (eq *value<0>* x ) *value<1>*  (* x (- x *value<1>*))) (f *value<3>*))


*Nested definitions* are also possible: For instance the inner f in `p: (def f [x](* 2 x)(  f (def f 3 3) ))`  does not collide with the outer f. 
But nested definitions are subject to a restriction that is explained in the section on Currying that follows.



### Currying 
We assign an arity to every elementary operation that we have described except for the operation "list":

```
Arity 1:  quote, first, drop_first, eval, not;
Arity 2:  cons, apply, and / or / xor, requires, eq, + / - / * / / / %, navigator<Xs...>;
Arity 3:  if, def
```

```
Operations of varying arity n:  combinator<n, F>   with a fixed template class template< typename... > class F.
```


If an elementary operation op has the arity n, then for any non-empty list of expressions xs...  with `< n` elements we let

```Eval( ( op xs... ) ) = (op xs...).```


If an elementary operation op has the arity n, and we are given a list xs... with n elements and another nonempty list ys...  of arguments,
then we let 

```Eval( op xs... ys...) =  Eval(  Eval(op xs...) ys... )```

*Currying in definitions*:  [ Missing ]


### Nested Expressions

For symbolic expressions x and non-empty lists of symbolic expressions xs..., ys...  the evaluation of the term ((x xs...) ys... ) is realised via the equation
``` 
Eval(  (( x xs...) ys... )  )  =  Eval( x xs... ys...)
```


## The C++ Type Associated With a Symbolic Expression

There are three class templates that are used to represent symbolic expressions as C++ types:
```
namespace lt {

    template<int n>          struct list;

    template< auto >         struct value;

    template< typename... >  struct s_expr;

    template< char... >      struct text;
};
```
The instantiations of these class templates are literal types. Thus objects whose types instantiate them can be non-type template parameters. For example `lt::value< lt::s_expr<double, lt::text<'a'>>{} >` is a valid instantiation of  lt::value.

1. An ascii string  of the form x<sub>1</sub>...x<sub>n</sub>  represents the type lt::text< x<sub>1</sub>, ..., x<sub>n</sub> >, unless it is an integer literal;
2. An ascii string that is an integer literal  (either binary, octal, decimal, or hexadecimal) denoting an integer n is represents the type lt::value<(int)n>
2. The expression (xs...) represents the type s_expr< Xs... >, with Xs the C++ types represented by xs.
3. The expression (list xs...) represents the type s_expr< list< sizeof...(xs) >,  Xs... >, with Xs... the C++ types represented by xs... .
4. A C++ type *T* represents itself.

A few examples:

1. hello represents lt::text<'h','e,'l','l','o'>
2. -0xA  represents lt::value< (int)-15 >
3. *double* is represented by the type *double*
3. ()       is represented by the type lt::s_expr<>
4. (first (list a b c)) represents the type  lt::s_expr<  text<'f', 'i', 'r', 's', 't'>,  lt::s_expr< list<3>, text<'a'>,  text<'b'>, text<'c'>  > >.



It is not relevant whether we see a symbolic expression as a string (with letters in the ascii-alphabet and  C++ type system) or as a C++ type, because the correspondence between them is unique. It is merely a matter of notation. 


**From now we consider symbolic expressions to be mere notations that specify C++ types in accordance with the rules we have just given.**



### lt::operator""_t,  lt::operator""_s
The operator ""_t transforms a C-string str of length n to the type lt::text< str[0], str[1], ... , str[n-1] >, with the \0-terminator excluded.


The operator ""_s generates a symbolic expression from a C-string at compile-time.

Examples:  

1.  `"(head (list 1 2 3))"_s` generates the C++ type lt::s_expr< list<3>,  lt::value<1>,  lt::value<2>,  lt::value<3>  >.
2.  `"[x](* 2 x)"_s` generates an expression q with `Eval(q[w/x]) = Eval((* 2 w)) = Eval(lt::s_expr< text<'*'>,  lt::value<(int)2>, W >)` for a suitable type *W*. 

In the last example we have used the convention that symbolic expressions be notations that specify C++ types. The exact types of w and W are not important. Only the fact that the identity  Eval(q[w/x]) = Eval((* 2 w)) holds is of importance.


# Navigators

The Trivium Lisp interpreter is called by instantiating the class template ```template< auto const > eval```. 

Example:  using result = eval<"([x](* 2 x) 3)"s>; The type result is lt::value<(int)6>. 

Trivium metaprograms can either be specified as the C++ types that represent the symbolic expressions with the source code, or they can be specified as compile-time strings with the ""_s-operator. The compiler sees the same thing, but the human developer deals far better with strings.

Thus we should (almost) always specify metaprograms with strings. For this to happen we need a method to symbolise C++-types in strings. This can be done with the help of navigators. A symbol is attached to a type name with the help of the following class template:
```
namespace lt {  template<  auto const symbol,  typename T  >  struct parameter;  }
```
Such attachements are gathered and made accessible to interpreters in a class template called navigator:
```
namespace lt {  
    template<  typename...  >  struct navigator;

    template<  auto const...  symbols
            ,  typename...    Xs
            >
    struct navigator<  parameter< synmbols, Xs >...  >;
}

```
Note that the symbols in the parameter pack *symbols...* are unique.  If not, the compilation will fail.


Navigators contain a type alias `template< auto const s > using lookup = ... `.

The type 
```
    typename navigator< parameter<symbols, Xs... >...  >::template lookup< s >
```
is the unique type X, if it exists, with the property that parameter<s, X> is contained in the parameter pack  *parameter< symbols, Xs >...* . If such a type cannot be found in the pack, then the compilation fails.



The Trivium Lisp interpreter can be called from a navigator:

```
template<  typename program  >
using nav_eval =

template<  auto const... s
        ,  typename...   X
        >
struct navigator<  parameter< s, X >...  >::template eval<program>;
```


The interpreter call nav_eval<"\`"_s>  results in the type navigator< lt::parameter<s, X>... >.

Applying the semantics of navigator instructions in Trivium Lisp, we can access a symbol z with the expression nav_eval<(\` z)>, which is identical with the unique *Z* such that *parameter<z, Z>* is contained in *parameter<s, X>...* .



Example:

```
template<  typename X  >
using sizeof_t = lt::value<  sizeof(X)  >;

using result = 
    lt::navigator<  lt::parameter< "size",     lt::combinator< 1, sizeof_t >  >
                 ,  lt::parameter< "integer",  int  >
                 ,  lt::parameter< "double",   double  >
                 >::
    template eval< "(if (eq ((` 'size) (` 'integer)) 8)  (` 'double) (` 'int) )"_s >;
```
The code above associates the result type *double* to *result* if the type *int* has the size 8. Otherwise it associates the type *int* to *result*.

# Libraries written in Trivium-Lisp
[ coming soon ]


## The Linker
[ coming soon ]


# The Symbolic Representation of the C++ Type System
[ coming soon ]

### Footnotes

[^1]: Normally being domain-specific and being Turing-complete are mutually exclusive properties, but here the situation is different: Trivium-Lisp is specific to the domain of computing at compile-time in contrast to C++, which is general in the sense that it supports computing both at compile-time and at runtime.


[^2]: The notion of homoiconicity is used very informally. It is always required for a symbolic language to be called homoiconic that programs have the same representation as symbolic expressions as data and that the language shall treat code as data, making it naturally apt for metaprogramming. The definition of homoiconicity given here is very exact, but the reader be warned that the gaps left by the informal use of the notion have been filled with an exact formula on the semantics that is not generally agreed.

