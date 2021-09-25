#include <iostream>
#include <memory>
#include <string>

#include <punky/Environment.hpp>
#include <punky/Evaluator.hpp>
#include <punky/Lexer.hpp>
#include <punky/Parser.hpp>
#include <punky/ast.hpp>
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
        if (std::holds_alternative<bool>(prog))  // TODO(piyush): Catch instead
            continue;

        auto eval = punky::eval::Evaluator{std::move(
          std::get<std::unique_ptr<punky::ast::Program>>(prog))};

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
