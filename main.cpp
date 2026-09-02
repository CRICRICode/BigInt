#include "BigInt.h"
#include <limits>
#include <iostream>
#include <stdexcept>

int main()
{
    BigInt original = -3;
    BigInt shifted = original << 2;

    std::cout << original << '\n';
    std::cout << shifted << '\n';

    try
    {
        std::cout << (original << -1) << '\n';
    }
    catch (const std::invalid_argument &error)
    {
        std::cout << error.what() << '\n';
    }
}