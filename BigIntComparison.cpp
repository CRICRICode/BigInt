#include "BigInt.h"

// Comparisons use the sign first, then the magnitude when necessary.

bool operator==(const BigInt &left, const BigInt &right)
{
    return left.digits == right.digits &&
           left.negative == right.negative;
}

bool operator!=(const BigInt &left, const BigInt &right)
{
    return !(left == right);
}

bool operator>(const BigInt &left, const BigInt &right)
{
    if (left.negative != right.negative)
    {
        return !left.negative;
    }

    if (!left.negative)
    {
        return left.CompareMagnitude(right) == BigInt::MagnitudeComparison::Greater;
    }

    return left.CompareMagnitude(right) == BigInt::MagnitudeComparison::Lesser;
}

bool operator>=(const BigInt &left, const BigInt &right)
{
    return !(right > left);
}

bool operator<(const BigInt &left, const BigInt &right)
{
    return right > left;
}

bool operator<=(const BigInt &left, const BigInt &right)
{
    return !(left > right);
}
