#include "BigInt.h"

#include <stdexcept>
#include <vector>

// Arithmetic operators and magnitude subtraction.

BigInt &BigInt::operator+=(const BigInt &other)
{
    if (this->negative != other.negative)
    {
        const BigInt::MagnitudeComparison leftComparison = this->CompareMagnitude(other);
        if (leftComparison == MagnitudeComparison::Lesser)
        {
            BigInt temp = other;
            temp.SubtractMagnitude(*this);
            *this = temp;
        }
        else if (leftComparison == MagnitudeComparison::Equal)
        {
            this->digits.clear();
            this->negative = false;
            this->digits.push_back(0);
        }
        else
        {
            this->SubtractMagnitude(other);
        }
    }
    else
    {
        int carry = 0;

        if (this->digits.size() < other.digits.size())
        {
            this->digits.resize(other.digits.size());
        }

        for (std::size_t i = 0; i < this->digits.size(); i++)
        {
            int otherDigit = 0;

            if (other.digits.size() > i)
            {
                otherDigit = other.digits.at(i);
            }

            int sum = this->digits.at(i) + otherDigit + carry;
            carry = 0;

            if (sum >= 10)
            {
                carry = sum / 10;
                sum = sum % 10;
            }

            this->digits.at(i) = sum;
        }

        if (carry != 0)
        {
            this->digits.push_back(carry);
        }
    }
    return *this;
}

BigInt BigInt::operator+(const BigInt &other) const
{
    BigInt result = *this;
    result += other;
    return result;
}

BigInt &BigInt::operator++()
{
    return *this += 1;
}

BigInt BigInt::operator++(int)
{
    BigInt result = *this;
    ++*this;
    return result;
}

BigInt &BigInt::operator-=(const BigInt &other)
{
    *this += -other;
    return *this;
}

BigInt BigInt::operator-(const BigInt &other) const
{
    BigInt result = *this;
    result -= other;
    return result;
}

BigInt BigInt::operator-() const
{
    BigInt result = *this;
    result.negative = !result.negative;
    result.Normalize();
    return result;
}

BigInt &BigInt::operator--()
{
    return *this -= 1;
}

BigInt BigInt::operator--(int)
{
    BigInt result = *this;
    --*this;
    return result;
}

BigInt &BigInt::operator*=(const BigInt &other)
{
    if (this->IsZero() || other.IsZero())
    {
        this->digits.clear();
        this->digits.push_back(0);
        this->negative = false;
        return *this;
    }

    BigInt result;
    result.digits.resize(this->digits.size() + other.digits.size());
    result.negative = this->negative != other.negative;

    for (std::size_t i = 0; i < this->digits.size(); i++)
    {
        int carry = 0;
        for (std::size_t j = 0; j < other.digits.size(); j++)
        {
            const int product = this->digits.at(i) * other.digits.at(j);
            const int sum = result.digits.at(i + j) + product + carry;

            result.digits.at(i + j) = sum % 10;
            carry = sum / 10;
        }
        result.digits.at(i + other.digits.size()) = carry;
    }
    result.Normalize();
    *this = result;
    return *this;
}

BigInt BigInt::operator*(const BigInt &other) const
{
    BigInt result = *this;
    result *= other;
    return result;
}

BigInt &BigInt::operator/=(const BigInt &other)
{
    if (other.IsZero())
    {
        throw std::invalid_argument("BigInt: input del divisore non valido");
    }

    const bool resultNegative = this->negative != other.negative;
    BigInt leftNumber = *this;
    BigInt rightNumber = other;
    leftNumber.negative = false;
    rightNumber.negative = false;

    BigInt remainder;
    std::vector<int> quotientDigits;

    for (std::size_t i = leftNumber.digits.size(); i > 0; --i)
    {
        const std::size_t index = i - 1;
        int quotientDigit = 0;

        remainder = remainder * 10 + leftNumber.digits.at(index);
        while (remainder >= rightNumber)
        {
            remainder -= rightNumber;
            ++quotientDigit;
        }
        quotientDigits.push_back(quotientDigit);
    }

    this->digits.clear();
    for (std::size_t i = quotientDigits.size(); i > 0; --i)
    {
        this->digits.push_back(quotientDigits.at(i - 1));
    }
    this->negative = resultNegative;
    this->Normalize();
    return *this;
}

BigInt BigInt::operator/(const BigInt &other) const
{
    BigInt result = *this;
    result /= other;
    return result;
}

BigInt &BigInt::operator%=(const BigInt &other)
{
    BigInt resultDivision = *this;
    resultDivision /= other;
    *this = *this - (resultDivision * other);
    return *this;
}

BigInt BigInt::operator%(const BigInt &other) const
{
    BigInt result = *this;
    result %= other;
    return result;
}

BigInt pow(const BigInt &base, int exponent)
{
    return pow(base, BigInt(exponent));
}

// Exponentiation by squaring avoids a linear number of multiplications.
BigInt pow(const BigInt &base, BigInt exponent)
{
    if (exponent < 0)
    {
        throw std::invalid_argument("BigInt: input all'esponente non valido");
    }

    BigInt result = 1;
    BigInt tempBase = base;

    while (exponent > 0)
    {
        if (exponent % 2 != 0)
        {
            result *= tempBase;
        }
        tempBase *= tempBase;
        exponent /= 2;
    }
    return result;
}

void BigInt::SubtractMagnitude(const BigInt &other)
{
    int borrow = 0;

    for (std::size_t i = 0; i < digits.size(); i++)
    {
        const int rightDigit = other.digits.size() > i ? other.digits.at(i) : 0;
        int leftDigit = this->digits.at(i) - borrow;

        if (leftDigit < rightDigit)
        {
            leftDigit += 10;
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }

        this->digits.at(i) = leftDigit - rightDigit;
    }
    this->Normalize();
}
