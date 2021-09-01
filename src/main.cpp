#include <iostream>
#include <string>

#include "../include/Evaluator.hpp"
#include "../include/Lexer.hpp"
#include "../include/Object.hpp"
#include "../include/Parser.hpp"
#include "../include/ast.hpp"
#include "../include/readline.hpp"

int main()
{
    std::string line;
    while (readline::read(line))
    {
        auto lex = punky::lex::Lexer{std::move(line)};
        auto par = punky::par::Parser{lex};

        auto prog = par.parse_program();

        auto errs = par.errors();
        if (!errs.empty())
        {
            std::cerr << "parser errors:\n";
            for (const auto& err : errs)
                std::cerr << "\t" + err + "\n";
            continue;
        }

        std::cout << "ast print:\n";
        if (prog)
            std::cout << prog->to_string();

        auto eval = punky::eval::Evaluator{std::move(prog)};

        auto res = eval.interpret();

        std::cout << "result inspect:\n";
        std::cout << punky::obj::inspect(res) << std::endl;
    }

    return 0;
}
