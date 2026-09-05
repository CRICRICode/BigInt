#include "BigInt.h"

// Comparisons use the sign first, then the magnitude when necessary.

bool BigInt::operator==(const BigInt &other) const
{
    return this->digits == other.digits &&
           this->negative == other.negative;
}

bool BigInt::operator!=(const BigInt &other) const
{
    return !(*this == other);
}

bool BigInt::operator>(const BigInt &other) const
{
    if (this->negative != other.negative)
    {
        return !this->negative;
    }

    if (!this->negative)
    {
        return this->CompareMagnitude(other) == MagnitudeComparison::Greater;
    }

    return this->CompareMagnitude(other) == MagnitudeComparison::Lesser;
}

bool BigInt::operator>=(const BigInt &other) const
{
    return !(other > *this);
}

bool BigInt::operator<(const BigInt &other) const
{
    return other > *this;
}

bool BigInt::operator<=(const BigInt &other) const
{
    return !(*this > other);
}
