#include "BigInt.h"

#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>

// Stream operators keep formatting and parsing outside the core arithmetic code.

std::ostream &operator<<(std::ostream &out, const BigInt &value)
{
    if (value.negative)
    {
        out << '-';
    }

    for (std::size_t i = value.digits.size(); i > 0; --i)
    {
        out << value.digits.at(i - 1);
    }
    return out;
}

std::istream &operator>>(std::istream &in, BigInt &value)
{
    std::string text;
    if (!(in >> text))
    {
        return in;
    }

    try
    {
        BigInt parsed(text);
        value = parsed;
    }
    catch (const std::invalid_argument &)
    {
        // Preserve value and report invalid formatted input through the stream.
        in.setstate(std::ios::failbit);
    }

    return in;
}
