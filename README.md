# punky
punky is an interpreter for the Monkey programming language written in C++.

# Overview
Monkey is a programing language designed by Thorsten Ball for the books Writing An Interpreter In Go and Writing A Compiler In Go. You may refer to the project website to learn more about Monkey.

punky is tree-walking interpreter for Monkey written in C++.

Here's how you would define a recursive function ```fib``` taking a single argument ```N``` for the fibonacci sequence in mal : 

```
let fibonacci = fn(x) {
  if (x == 0) {
    0
  } else {
    if (x == 1) {
      return 1;
    } else {
      fibonacci(x - 1) + fibonacci(x - 2);
    }
  }
};
```

and call it as :

```
fibonacci(10);
```

# Implemented

# Build
punky is built using CMake and requires at least C++17. clang-tidy and clang-format config files are provided.

MalPP requires no external dependencies.

```
git clone https://github.com/buzzcut-s/punky.git
cd punky/
mkdir build
cd build/
cmake ..
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

# What's missing 

# Implementation Details

# Examples
There are only examples beyond this point, just a heads-up.

# License
punky is licensed under the MIT License. See [LICENSE](https://github.com/buzzcut-s/punky/blob/nude/LICENSE) for more details.

punky uses the cpp-linenoise library. The file ```linenoise.hpp``` is taken from the [cpp-linenoise](https://github.com/yhirose/cpp-linenoise) library, © 2015 Yuji Hirose, licensed under the BSD license.

Monkey is licensed under the MIT license, © 2020 Thorsten Ball.

