#include "BigInt.h"
#include <stdexcept>
#include <string>
#include <utility>

// Core representation, construction and invariant maintenance.

BigInt::BigInt()
{
    this->digits.push_back(0);
    this->negative = false;
}

BigInt::BigInt(int value) : BigInt(static_cast<long long>(value)) {}

BigInt::BigInt(long value) : BigInt(static_cast<long long>(value)) {}

BigInt::BigInt(long long value)
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

BigInt::BigInt(unsigned int value) : BigInt(static_cast<unsigned long long>(value)) {}

BigInt::BigInt(unsigned long value) : BigInt(static_cast<unsigned long long>(value)) {}

BigInt::BigInt(unsigned long long value)
{
    this->negative = false;

    do
    {
        this->digits.push_back(static_cast<int>(value % 10));
        value /= 10;
    } while (value != 0);
}

BigInt::BigInt(BigInt &&other) : BigInt()
{
    this->digits.swap(other.digits);
    this->negative = other.negative;
    other.negative = false;
}

BigInt &BigInt::operator=(BigInt &&other)
{
    if (this != &other)
    {
        BigInt moved(std::move(other));
        this->digits.swap(moved.digits);
        this->negative = moved.negative;
    }
    return *this;
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

BigInt::MagnitudeComparison BigInt::CompareMagnitude(const BigInt &other) const
{
    if (this->digits.size() < other.digits.size())
    {
        return MagnitudeComparison::Lesser;
    }
    else if (this->digits.size() > other.digits.size())
    {
        return MagnitudeComparison::Greater;
    }

    for (std::size_t i = this->digits.size(); i > 0; --i)
    {
        std::size_t index = i - 1;
        if (this->digits.at(index) > other.digits.at(index))
        {
            return MagnitudeComparison::Greater;
        }
        else if (this->digits.at(index) < other.digits.at(index))
        {
            return MagnitudeComparison::Lesser;
        }
    }
    return MagnitudeComparison::Equal;
}
