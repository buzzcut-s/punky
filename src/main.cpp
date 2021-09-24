#include <iostream>
#include <memory>
#include <string>

#include "../include/Environment.hpp"
#include "../include/Evaluator.hpp"
#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"
#include "../include/readline.hpp"

static void repl(punky::env::Environment& env)
{
    std::string line;
    while (readline::read(line))
    {
        auto lex = punky::lex::Lexer{std::move(line)};
        auto par = punky::par::Parser{lex};

        auto prog = par.parse_program();

        auto errors = par.errors();
        if (!errors.empty())
        {
            std::cerr << "parser errors:\n";
            for (const auto& error : errors)
                std::cerr << "\t" + error + "\n";
            continue;
        }

        auto eval = punky::eval::Evaluator{std::move(prog)};

        auto res = eval.interpret(env);

        if (const auto out = punky::obj::inspect(res); !out.empty())
            std::cout << out << std::endl;
    }
}

int main()
{
    auto env = std::make_unique<punky::env::Environment>();

    repl(*env);

    return 0;
}
