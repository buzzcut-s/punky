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
```

To run :
``` 
./punky
```

# Usage  
punky provides a REPL environment to play around in. 
After executing, use the ```punky >>``` shell to provide input. 


# (extra)
You can pass in a second string argument to the ```readline::read(input)``` call at ```main.cpp:18:31```[ (here) ](https://github.com/buzzcut-s/punky/blob/main/src/main.cpp#L18) to change the shell prompt from ```punky >>``` to anything else that your heart desires :D

# Examples
There are only examples beyond this point, just a heads-up.

- Literals
```
punky >> 150
150
punky >> true
true
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
punky is licensed under the MIT License, © 2021 Piyush Kumar. See [LICENSE](https://github.com/buzzcut-s/punky/blob/nude/LICENSE) for more details.

punky uses the cpp-linenoise library. The file ```linenoise.hpp``` is taken from the [cpp-linenoise](https://github.com/yhirose/cpp-linenoise) library, © 2015 Yuji Hirose, licensed under the BSD license.

The Monkey spec is licensed under the MIT license, © 2020 Thorsten Ball.

