#include "BigInt.h"
#include <iostream>
#include <stdexcept>

BigInt::BigInt()
{
    this->digits.push_back(0);
    this->negative = false;
}

BigInt::BigInt(int value)
{
    this->negative = value < 0;

    do
    {
        int singleValue = value % 10;

        if (singleValue < 0)
        {
            singleValue = -singleValue;
        }

        this->digits.push_back(singleValue);
        value = value / 10;
    } while (value != 0);
}

BigInt::BigInt(const std::string &text)
{
    this->negative = false;
    char firstDigit = 0;

    if (text.empty() || text == "-" || text == "+")
    {
        throw std::invalid_argument("BigInt: input non valido");
    }

    if (text[0] == '-')
    {
        this->negative = true;
        firstDigit = 1;
    }
    else if (text[0] == '+')
    {
        firstDigit = 1;
    }

    for (int i = text.length() - 1; i >= firstDigit; i--)
    {
        char value = text[i];

        if (value >= '0' && value <= '9')
        {
            int number = value - '0';
            this->digits.push_back(number);
        }
        else
        {
            throw std::invalid_argument("BigInt: input non valido");
        }
    }
    this->Normalize();
}

BigInt &BigInt::operator+=(const BigInt &other)
{
    if (this->negative != other.negative)
    {
        const BigInt::MagnitudeComparison leftComparison = this->CompareMagnitude(other);
        if (leftComparison == MagnitudeComparison::LESSER)
        {
            BigInt temp = other;
            temp.SubtractMagnitude(*this);
            *this = temp;
        }
        else if (leftComparison == MagnitudeComparison::EQUAL)
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

            int sum = this->digits.at(i);
            sum += otherDigit;

            if (carry > 0)
            {
                sum = sum + carry;
                carry = 0;
            }

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
    BigInt digit = *this;
    if (digit.negative)
    {
        digit.negative = false;
    }
    else
    {
        digit.negative = true;
    }
    digit.Normalize();
    return digit;
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

    for (std::size_t i = 0; i < this->digits.size(); i++) // Cicle for left number
    {
        int carry = 0;
        for (std::size_t j = 0; j < other.digits.size(); j++) // Cicle for right number
        {
            int leftDigit = this->digits.at(i);
            int rightDigit = other.digits.at(j);

            int sum = result.digits.at(i + j) + leftDigit * rightDigit + carry;
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
    bool negResult = this->negative != other.negative;
    BigInt leftNumber = *this, rightNumber = other;

    if (other.IsZero())
    {
        throw std::invalid_argument("BigInt: input del divisore non valido");
    }

    leftNumber.negative = false;
    rightNumber.negative = false;

    BigInt remainder;
    std::vector<int> temp;

    for (std::size_t i = leftNumber.digits.size(); i > 0; --i)
    {
        std::size_t index = i - 1;
        int quotientDigit = 0;
        int singleDigit = leftNumber.digits.at(index);
        remainder = remainder * 10 + singleDigit;
        while (remainder >= rightNumber)
        {
            remainder -= rightNumber;
            quotientDigit++;
        }
        temp.push_back(quotientDigit);
    }

    this->digits.clear();

    for (std::size_t i = temp.size(); i > 0; --i)
    {
        std::size_t index = i - 1;
        this->digits.push_back(temp.at(index));
    }
    this->negative = negResult;
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

BigInt pow(const BigInt &base, BigInt exponent)
{
    if (exponent < 0)
    {
        throw std::invalid_argument("BigInt: input all'esponente non valido");
    }
    BigInt result = 1;
    while (exponent > 0)
    {
        result *= base;
        exponent--;
    }
    return result;
}

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
    else
    {
        if (this->negative == false && this->CompareMagnitude(other) == MagnitudeComparison::GREATER)
        {
            return true;
        }
        if (this->negative == true && this->CompareMagnitude(other) == MagnitudeComparison::LESSER)
        {
            return true;
        }
    }
    return false;
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

BigInt &BigInt::operator<<=(int count)
{
    if (count < 0)
    {
        throw std::invalid_argument("BigInt: input inserito non valido");
    }
    if (count == 0)
    {
        return *this;
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

std::ostream &operator<<(std::ostream &out, const BigInt &value)
{
    if (value.negative)
    {
        out << "-";
    }
    for (std::size_t i = value.digits.size(); i > 0; --i)
    {
        out << value.digits.at(i - 1);
    }
    return out;
}

BigInt &BigInt::operator&=(const BigInt &other)
{
    BigInt leftPositive = *this;
    if (leftPositive.negative)
    {
        leftPositive = -leftPositive;
    }

    BigInt rightPositive = other;
    if (rightPositive.negative)
    {
        rightPositive = -rightPositive;
    }

    std::size_t leftBits = leftPositive.ToBits().size();
    std::size_t rightBits = rightPositive.ToBits().size();
    std::size_t maxSize = (leftBits > rightBits ? leftBits : rightBits) + 1;

    std::vector<int> leftNumber = this->ToTwosComplement(maxSize);
    std::vector<int> rightNumber = other.ToTwosComplement(maxSize);
    std::vector<int> result(maxSize);


    for (std::size_t i = 0; i < maxSize; i++)
    {
        result.at(i) = leftNumber.at(i) & rightNumber.at(i);
    }

    *this = FromTwosComplement(result);
    return *this;
}

BigInt BigInt::operator&(const BigInt &other) const
{
    BigInt result = *this;
    result &= other;
    return result;
}

void BigInt::Print() const
{
    if (negative)
    {
        std::cout << "-";
    }

    for (int i = digits.size() - 1; i > -1; i--)
    {
        std::cout << this->digits.at(i);
    }
    std::cout << std::endl;
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

// Private

void BigInt::Normalize()
{
    if (digits.empty())
    {
        this->digits.push_back(0);
    }

    while (digits.size() > 1 && digits.back() == 0)
    {
        this->digits.pop_back();
    }

    if (digits.front() == 0 && digits.size() == 1)
    {
        this->negative = false;
    }
}

bool BigInt::IsZero() const
{
    return this->digits.size() == 1 && this->digits.at(0) == 0;
}

void BigInt::SubtractMagnitude(const BigInt &other)
{
    int borrow = 0;

    for (std::size_t i = 0; i < digits.size(); i++)
    {

        int rightDigit = 0;
        int leftDigit = this->digits.at(i) - borrow;

        if (other.digits.size() > i)
        {
            rightDigit = other.digits.at(i);
        }

        if (leftDigit < rightDigit)
        {
            borrow = 1;
            leftDigit = (10 + leftDigit) - rightDigit;
            this->digits.at(i) = leftDigit;
        }
        else
        {
            borrow = 0;
            leftDigit = leftDigit - rightDigit;
            this->digits.at(i) = leftDigit;
        }
    }
    this->Normalize();
}

BigInt::MagnitudeComparison BigInt::CompareMagnitude(const BigInt &other) const
{
    if (this->digits.size() < other.digits.size())
    {
        return MagnitudeComparison::LESSER;
    }
    else if (this->digits.size() > other.digits.size())
    {
        return MagnitudeComparison::GREATER;
    }

    for (std::size_t i = this->digits.size(); i > 0; --i)
    {
        std::size_t index = i - 1;
        if (this->digits.at(index) > other.digits.at(index))
        {
            return MagnitudeComparison::GREATER;
        }
        else if (this->digits.at(index) < other.digits.at(index))
        {
            return MagnitudeComparison::LESSER;
        }
    }
    return MagnitudeComparison::EQUAL;
}

std::vector<int> BigInt::ToBits() const
{
    BigInt working = *this; // 13
    std::vector<int> result;
    BigInt remainder = working % 2; // Estraggo già il primo bit == resto //1

    if (working.IsZero())
    {
        result.push_back(0);
        return result;
    }

    while (working > 0)
    {
        remainder = working % 2;
        working /= 2;
        result.push_back(remainder.digits.at(0)); // Prendo quello in posizione 0 tanto varrà sempre e solo 0 o 1
    }

    return result;
}

std::vector<int> BigInt::ToTwosComplement(std::size_t width) const
{
    BigInt temp = *this;

    if (temp.negative)
    {
        temp = -temp;
    }

    std::vector<int> result = temp.ToBits();
    result.resize(width, 0);

    if (!this->negative)
    {
        return result;
    }

    for (std::size_t i = 0; i < width; i++)
    {
        if (result[i] == 0)
        {
            result[i] = 1;
        }
        else
        {
            result[i] = 0;
        }
    }

    this->AddOneToBits(result);
    return result;
}

BigInt BigInt::FromTwosComplement(const std::vector<int> &bits)
{
    if (bits.back() == 0)
    {
        return FromBits(bits);
    }

    std::vector<int> magnitudeBits = bits;

    for (std::size_t i = magnitudeBits.size(); i > 0; --i)
    {
        std::size_t index = i - 1;
        if (magnitudeBits.at(index) == 0)
        {
            magnitudeBits.at(index) = 1;
        }
        else
        {
            magnitudeBits.at(index) = 0;
        }
    }
    AddOneToBits(magnitudeBits);
    return -FromBits(magnitudeBits);
}

void BigInt::AddOneToBits(std::vector<int> &bits)
{
    int one = 1;

    for (std::size_t i = 0; i < bits.size(); i++)
    {
        if (bits.at(i) + one > 1)
        {
            bits.at(i) = 0;
            one = 1;
        }
        else
        {
            bits.at(i) = bits.at(i) + one;
            return;
        }
    }
}