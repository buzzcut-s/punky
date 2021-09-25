#include <iostream>
#include <memory>
#include <string>

#include <punky/Environment.hpp>
#include <punky/Evaluator.hpp>
#include <punky/Lexer.hpp>
#include <punky/Parser.hpp>
#include <punky/readline.hpp>

static void repl()
{
    auto repl_env = punky::env::Environment();

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

        auto res = eval.interpret(repl_env);

        if (const auto out = punky::obj::inspect(res); !out.empty())
            std::cout << out << std::endl;
    }
}

int main()
{
    repl();

    return 0;
}
