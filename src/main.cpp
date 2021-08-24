#include <iostream>
#include <string>

#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"
#include "../include/Token.hpp"
#include "../include/ast.hpp"
#include "../include/readline.hpp"

int main()
{
    std::string line;

    while (readline::read(line))
    {
        auto lex = Lexer{line};

        /*
        auto token = lex.next_token();
        std::cout << "Printing lexer output :" << std::endl;
        while (token.m_type != TokenType::EOS)
        {
            std::cout << format_token(token) << std::endl;
            token = lex.next_token();
        }
        */
        auto par  = parser::Parser{lex};
        auto prog = par.parse_program();

        std::cout << prog->to_string();

        auto errs = par.errors();
        for (const auto& err : errs)
            std::cout << err;
    }

    return 0;
}
