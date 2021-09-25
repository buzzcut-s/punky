#ifndef FOBJECT_HPP
#define FOBJECT_HPP

#include "ast.hpp"

namespace punky::env
{
class Environment;
}

namespace punky::obj
{

class FunctionObject
{
public:
    FunctionObject(const ast::FunctionLiteral* fn, env::Environment* fn_env) :
      m_fn{fn},
      m_fn_env{fn_env}
    {}

    [[nodiscard]] auto fn() const { return m_fn; }
    [[nodiscard]] auto env() const { return m_fn_env; }

private:
    // This should be a unique_ptr -> which takes ownership of a FnLit from the AST
    const ast::FunctionLiteral* m_fn;

    // Not sure if a raw ptr here would suffice.
    // It would suffice for storing out otermost env from main
    // Apart from that, I'm not sure...
    env::Environment* m_fn_env;
};

}  // namespace punky::obj

#endif  // FOBJECT_HPP
