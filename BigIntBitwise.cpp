#include "BigInt.h"

#include <stdexcept>
#include <vector>

// Shifts and bitwise operators use a temporary bit representation when needed.

BigInt &BigInt::operator<<=(int count)
{
    if (count < 0)
    {
        throw std::invalid_argument("BigInt: input inserito non valido");
    }

    for (int i = 0; i < count; i++)
    {
        *this *= 2;
    }
    return *this;
}

BigInt BigInt::operator<<(int count) const
{
    BigInt result = *this;
    result <<= count;
    return result;
}

BigInt &BigInt::operator>>=(int count)
{
    if (count < 0)
    {
        throw std::invalid_argument("BigInt: input inserito non valido");
    }

    for (int i = 0; i < count; i++)
    {
        // Arithmetic right shift rounds negative odd values toward negative infinity.
        const bool needsRoundDown = this->negative && ((*this % 2) != 0);
        *this /= 2;

        if (needsRoundDown)
        {
            *this -= 1;
        }
    }
    return *this;
}

BigInt BigInt::operator>>(int count) const
{
    BigInt result = *this;
    result >>= count;
    return result;
}

BigInt BigInt::operator~() const
{
    BigInt result = -*this;
    result -= 1;
    return result;
}

BigInt &BigInt::operator&=(const BigInt &other)
{
    return ApplyBitwise(other, BitwiseOperation::And);
}

BigInt BigInt::operator&(const BigInt &other) const
{
    BigInt result = *this;
    result &= other;
    return result;
}

BigInt &BigInt::operator|=(const BigInt &other)
{
    return ApplyBitwise(other, BitwiseOperation::Or);
}

BigInt BigInt::operator|(const BigInt &other) const
{
    BigInt result = *this;
    result |= other;
    return result;
}

BigInt &BigInt::operator^=(const BigInt &other)
{
    return ApplyBitwise(other, BitwiseOperation::Xor);
}

BigInt BigInt::operator^(const BigInt &other) const
{
    BigInt result = *this;
    result ^= other;
    return result;
}

BigInt &BigInt::ApplyBitwise(const BigInt &other, BitwiseOperation operation)
{
    BigInt leftMagnitude = *this;
    if (leftMagnitude.negative)
    {
        leftMagnitude = -leftMagnitude;
    }

    BigInt rightMagnitude = other;
    if (rightMagnitude.negative)
    {
        rightMagnitude = -rightMagnitude;
    }

    const std::size_t leftBits = leftMagnitude.ToBits().size();
    const std::size_t rightBits = rightMagnitude.ToBits().size();
    const std::size_t width = (leftBits > rightBits ? leftBits : rightBits) + 1;

    const std::vector<int> leftBitsValue = this->ToTwosComplement(width);
    const std::vector<int> rightBitsValue = other.ToTwosComplement(width);
    std::vector<int> resultBits(width);

    for (std::size_t i = 0; i < width; i++)
    {
        if (operation == BitwiseOperation::And)
        {
            resultBits.at(i) = leftBitsValue.at(i) & rightBitsValue.at(i);
        }
        else if (operation == BitwiseOperation::Or)
        {
            resultBits.at(i) = leftBitsValue.at(i) | rightBitsValue.at(i);
        }
        else
        {
            resultBits.at(i) = leftBitsValue.at(i) ^ rightBitsValue.at(i);
        }
    }

    *this = FromTwosComplement(resultBits);
    return *this;
}

BigInt BigInt::FromBits(const std::vector<int> &bits)
{
    BigInt result = 0;

    for (std::size_t i = bits.size(); i > 0; --i)
    {
        result *= 2;
        result += bits.at(i - 1);
    }
    return result;
}

std::vector<int> BigInt::ToBits() const
{
    BigInt working = *this;
    std::vector<int> bits;

    if (working.IsZero())
    {
        bits.push_back(0);
        return bits;
    }

    while (working > 0)
    {
        const BigInt remainder = working % 2;
        working /= 2;
        bits.push_back(remainder.digits.at(0));
    }
    return bits;
}

std::vector<int> BigInt::ToTwosComplement(std::size_t width) const
{
    BigInt magnitude = *this;
    if (magnitude.negative)
    {
        magnitude = -magnitude;
    }

    std::vector<int> bits = magnitude.ToBits();
    bits.resize(width, 0);

    if (!this->negative)
    {
        return bits;
    }

    for (int &bit : bits)
    {
        bit = bit == 0 ? 1 : 0;
    }
    AddOneToBits(bits);
    return bits;
}

BigInt BigInt::FromTwosComplement(const std::vector<int> &bits)
{
    if (bits.back() == 0)
    {
        return FromBits(bits);
    }

    std::vector<int> magnitudeBits = bits;
    for (int &bit : magnitudeBits)
    {
        bit = bit == 0 ? 1 : 0;
    }
    AddOneToBits(magnitudeBits);
    return -FromBits(magnitudeBits);
}

void BigInt::AddOneToBits(std::vector<int> &bits)
{
    int carry = 1;

    for (int &bit : bits)
    {
        if (bit + carry > 1)
        {
            bit = 0;
            continue;
        }

        bit += carry;
        return;
    }
}
