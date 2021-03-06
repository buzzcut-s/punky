# Overview
Monkey is a programing language designed by Thorsten Ball for the books Writing An Interpreter In Go and Writing A Compiler In Go. You may refer to the project website to learn more about Monkey.

punky is tree-walking interpreter implementation for Monkey written in C++.

# Release Build
punky is built using CMake and requires at least C++17. clang-tidy and clang-format config files are provided.

punky requires no external dependencies.

To build:

```
git clone https://github.com/buzzcut-s/punky.git
cd punky/
mkdir build
cd build/
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
cd src/
```

The binary is built in the ```punky/build/src/``` directory

To run :
``` 
./punky
```

# Usage  
punky provides a REPL environment to play around in. 
After executing, use the ```punky >>``` shell to provide input. 


# (extra)
You can pass in a second string argument to the ```readline::read(input)``` call at ```main.cpp:18:31```[ (here) ](https://github.com/buzzcut-s/punky/blob/main/src/main.cpp#L18) to change the shell prompt from ```punky >>``` to anything else that your heart desires :D


# Implementation Details
An interpreter is a program that executes a source program written in a particular language. For the interpreter to work, it needs to take a string of text, understand its meaning and do the task that the programmer intends. 
- punky has three phases - lexer, parser, and evaluator. A data structure joins each pair of phases. Tokens flow from lexer to parser, and an abstract syntax tree (AST) from parser to evaluator. The evaluator, then, runs the program.
    - Lexing / lexical analysis / tokenization : Converting a stream of characters (say, the source program) to a stream of Tokens, by identifying keywords, punctuations etc. This phase does a lot of string analysis so that the other two phases deal with individual Tokens instead of linear sequences of text. A token is the smallest individual part of the source program. Language keywords like ```class``` and ```for```, variable names, function names etc. are all Tokens.
    - Parsing : Converting a stream of symbols (in our case, Tokens) and produces a grammar conforming parse tree (AST).
    - Evaluating : Takes a well formed AST and evaluates it, by walking the tree. Hence, the term, tree walking interpreter. The evaluator understands simple primitive operations. For example, it knows how to add numbers or how to concatenate strings.
- A recursive Pratt parser is implemented for parsing. Pratt parsing was described by Vaughan R. Pratt in his paper ["Top Down Operator Precedence"](https://dl.acm.org/doi/10.1145/512927.512931), in 1973. This is used to handle operator precedence and infix expressions during the parsing phase. (see [```parse_expression()```](https://github.com/buzzcut-s/punky/blob/10d17ac00d0f2a277a04a8b7e522b32da6309373/src/Parser.cpp#L161)).
- Error messages are produced in the parsing phase. In case of a parse error, evaluation does not occur. 

# Issue(s) and TODOs
- With the way the AST (as std::unique_ptrs) and runtime Objects (as a std::variant of values) are handled internally right now, all AST state is deleted after execution finishes for a line. This works fine for Integer and Boolean literals since we always store them by value in our runtime Objects. However this does not work for Function literals as we currently only store an instance of a class which wraps around a pointer (see [here](https://github.com/buzzcut-s/punky/blob/f1be9faf2fd505566c7af5b707e2b70db0999e9a/include/FObject.hpp#L32)) in our runtime Function Object (extracted from the AST node - see [here](https://github.com/buzzcut-s/punky/blob/449c474ccb1d3692ac278312779ab88ac3fce394/src/Evaluator.cpp#L126)). This means all function state is deleted as soon as the line on which it was defined on finishes execution. In other words, functions must be defined and used on the same line - or else we access deleted memory. This also makes closures not work right now. 

- We can observe this very trivially. Compare the following:
    ```
    punky >> let add = fn(x, y) { x + y; }; add(5,15);
    20
    ```
    and 
    ```
    punky >> let add = fn(x,y) { x + y; };
    punky >> add(5, 15);
    terminate called after throwing an instance of 'std::bad_alloc'
    what():  std::bad_alloc
    Aborted (core dumped)
    ```
    This happens [here](https://github.com/buzzcut-s/punky/blob/449c474ccb1d3692ac278312779ab88ac3fce394/src/Evaluator.cpp#L329) when accessing params.name(). It's accessing members from the AST node FunctionLiteral, defined [here](https://github.com/buzzcut-s/punky/blob/99668957bab874918cd1e0ca85478edfbcebe1d0/include/ast.hpp#L531), which had already been deleted once the first line finished executing. 
    - The easiest solution I can think of right now, which I've implemented elsewhere in another project as well, is to capture the state of these FunctionLiterals and store a copy (the pointer to which can then be stored in whichever environment necessary) elsewhere. This solves ownership and deletion issues, as global functions should stay around for the entire execution, always. 
    - To also note is that we can also steal the function state from the AST (by moving it) and store that in the runtime Function Object. This would mean having a unique_ptr member. We use value semantics to pass Objects during runtime. So that's a no go.
- Add more in-built data types : Strings, Arrays and Hashmaps.
- Implement some built-in language functions.
- See [REVIEW_NOTES](https://github.com/buzzcut-s/punky/blob/main/REVIEW_NOTES.md) for more.

# Examples
There are only examples beyond this point, just a heads-up.

An example script can also be found in the examples directory.

- Literals
```
punky >> 150
150
punky >> true
true
```

- Bindings
```
punky >> let a = 5; let b = a; let c = a + b + 5; c;
15
punky >> let a = 5;
punky >> let b = a > 3;
punky >> let c = a * 99;
punky >> if (b) { 10 } else { 1 };
10
punky >> let d = if (c > a) { 99 } else { 100 };
punky >> d
99
punky >> d * c * a;
245025
```

- Prefix operators
```
punky >> !true
false
punky >> !5
false
punky >> -5
-5
punky >> !!false
false
punky >> !!!!-5
true
```

- Infix operators
```
punky >> 2 * (5 + 10)
30
punky >> (5 + 10 * 2 + 15 / 3) * 2 + -10
50
punky >> 1 < 1
false
punky >> 1 != 2
true
punky >>  3 + 4 * 5 == 3 * 1 + 4 * 5
true
punky >> (5 > 5 == true) != false
false
```

- Conditional statements
```
punky >> if (false) { 10 }
null
punky >> if (1) { 25 }
25
punky >> if ((1000 / 2) + 250 * 2 == 1000) { 9999 }
9999
````

- Error handling in the interpreter
```
punky >> 5 + true;
type mismatch: int + boolean
punky >> -true
unknown operator: -boolean
punky >> if (10 > 1) { true + false; }
unknown operator: boolean + boolean
```

- Parsing errors
```
punky >> let x = { 1, 2 }
parser errors:
        No prefix parse function found for token 'LEFT_BRACE'
        No prefix parse function found for token 'COMMA'
        No prefix parse function found for token 'RIGHT_BRACE'
```

- Function and Function Calls
```
punky >>  let double = fn(x) { x * 2; }; double(20);
40
punky >>  let factorial = fn(n) { if (n == 0) { 1 } else { n * factorial(n - 1) } }; factorial(5);
120
punky >> let max = fn(x, y) { if (x > y) { x } else { y } }; max(5, 10);
10
punky >> let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));
20
```


# License
punky is licensed under the MIT License, ?? 2021 Piyush Kumar. See [LICENSE](https://github.com/buzzcut-s/punky/blob/nude/LICENSE) for more details.

punky uses the cpp-linenoise library. The file ```linenoise.hpp``` is taken from the [cpp-linenoise](https://github.com/yhirose/cpp-linenoise) library, ?? 2015 Yuji Hirose, licensed under the BSD license.

The Monkey spec is licensed under the MIT license, ?? 2020 Thorsten Ball.

