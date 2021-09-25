These are some of the review notes from the Royal Code Roast session, organized by the CppIndia community.

- Forward declarations. Remove them. Just makes headers horrible to open.
- Lexer members should be named better.
- Keyword map, prefix parse function map, infix parse function map and the precedence map can all just be static const global variables in the respective impl files. In fact, they should be. This cleans up a lot of the constructors and removes their virtually useless functions.
- Parser::expect_peek() is a bad name. It does not specify that it's not a pure function and we care about the side effect (the consume() call) in it. 
- The evaluator is a classic visitor problem. Can look into using that instead. This would remove the huge switch case in eva::eval() function.
- The AST and how the base class ASTNode returns a casted pointer to a derived class is related to the Curiously recurring template pattern (CRTP). Can look into that as well.
- static_cast on pointers is assuming a lot on the programmer. Let's not do that and do dynamic_casts and check for runtime cast bugs that way.
- Make parse error checking a strong guarantee. Let's throw an error at the end of parse_program() which we must catch in repl(), or die.
- Alternative : Can return a full stream of tokens instead. Makes independant testing of the Lexer and Parser classes easier.
- Alternative : Have TokenType be a class with a std::string member. Create static const globals for each type with the string member as the format string. This would replace all tok::type_to_string(m_token.m_type) calls and the huge switch case in the function to simply m_token.m_type.m_string.
- Alternative : - The parser already stores a Lexer member in it. Instead of passing a Lexer instance in the constructor, let's just pass the input line and we can construct the Lexer member in Parser.
- Solution : Just store the function state in the Environment. We look up the function identifier in the env anyways, let's also store the function literal members there as well. Pretty cool.
- Solution : Instead of creating the AST for each input line, keep adding on to the program AST vector. Return this vector which gets passed on to the Evaluator. This way the AST is alive when changing lines as well. 
