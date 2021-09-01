#include <iostream>
#include <memory>
#include <string>

#include "../include/Environment.hpp"
#include "../include/Evaluator.hpp"
#include "../include/Lexer.hpp"
#include "../include/Object.hpp"
#include "../include/Parser.hpp"
#include "../include/ast.hpp"
#include "../include/readline.hpp"

int main()
{
    auto env = std::make_unique<punky::env::Environment>();

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

        auto eval = punky::eval::Evaluator{std::move(prog)};

        auto res = eval.interpret(*env);

        if (const auto out = punky::obj::inspect(res); !out.empty())
            std::cout << out << std::endl;
    }

    return 0;
}
