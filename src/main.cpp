#include <iostream>
#include <string>

#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"
#include "../include/readline.hpp"

int main()
{
    std::string line;
    while (readline::read(line))
    {
        auto lex = Lexer{std::move(line)};
        auto par = parser::Parser{lex};

        auto prog = par.parse_program();

        auto errs = par.errors();
        if (!errs.empty())
        {
            std::cerr << "parser errors:\n";
            for (const auto& err : errs)
                std::cerr << "\t" + err + "\n";
            continue;
        }

        if (prog)
            std::cout << prog->to_string();
    }

    return 0;
}
