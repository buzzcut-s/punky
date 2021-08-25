#include <cctype>

namespace punky::utils
{

bool is_letter(char ch)
{
    return (std::isalpha(ch) != 0) || ch == '_';
}

bool is_digit(char ch)
{
    return std::isdigit(ch) != 0;
}

bool is_whitespace(char ch)
{
    return std::isspace(ch) != 0;
}

}  // namespace punky::utils
