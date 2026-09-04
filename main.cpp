#include "BigInt.h"

#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace
{
    int failures = 0;

    void Check(bool condition, const char *testName)
    {
        if (!condition)
        {
            ++failures;
            std::cerr << "[FAIL] " << testName << '\n';
        }
    }

    void ExpectEqual(const BigInt &actual,
                     const BigInt &expected,
                     const char *testName)
    {
        Check(actual == expected, testName);
    }

    template <typename Function>
    void ExpectInvalidArgument(Function action, const char *testName)
    {
        try
        {
            action();
            Check(false, testName);
        }
        catch (const std::invalid_argument &)
        {
            Check(true, testName);
        }
        catch (...)
        {
            Check(false, testName);
        }
    }
}

int main()
{
    // Constructors and normalization
    ExpectEqual(BigInt(), BigInt(0), "default constructor creates zero");
    ExpectEqual(BigInt(-123), BigInt("-123"), "integer negative constructor");
    ExpectEqual(BigInt("-0000"), BigInt(0), "negative zero normalizes");
    ExpectEqual(BigInt("00023"), BigInt(23), "leading zeroes normalize");
    ExpectEqual(BigInt("-00023"), BigInt(-23), "negative leading zeroes normalize");
    ExpectEqual(BigInt("+23"), BigInt(23), "plus sign parsing");
    ExpectEqual(
        BigInt(std::numeric_limits<int>::min()),
        BigInt(std::to_string(std::numeric_limits<int>::min())),
        "INT_MIN constructor");

    ExpectInvalidArgument([]
                          { BigInt(""); }, "empty string throws");
    ExpectInvalidArgument([]
                          { BigInt("+"); }, "sign only plus throws");
    ExpectInvalidArgument([]
                          { BigInt("-"); }, "sign only minus throws");
    ExpectInvalidArgument([]
                          { BigInt("12a3"); }, "non-digit throws");

    // Addition and subtraction
    ExpectEqual(BigInt(999) + BigInt(1), BigInt(1000), "addition carry");
    ExpectEqual(BigInt(23) + BigInt(-50), BigInt(-27), "addition different signs");
    ExpectEqual(BigInt(-23) + BigInt(-5), BigInt(-28), "addition both negative");
    ExpectEqual(BigInt(23) - BigInt(-50), BigInt(73), "subtract negative");
    ExpectEqual(BigInt(-23) - BigInt(-5), BigInt(-18), "negative subtraction");
    ExpectEqual(BigInt(23) - BigInt(23), BigInt(0), "subtraction produces canonical zero");

    BigInt addSelf = 17;
    addSelf += addSelf;
    ExpectEqual(addSelf, BigInt(34), "self addition");

    // Multiplication
    ExpectEqual(BigInt(385) * BigInt(892), BigInt(343420), "multi-digit multiplication");
    ExpectEqual(BigInt(-23) * BigInt(45), BigInt(-1035), "negative multiplication");
    ExpectEqual(BigInt(0) * BigInt(-999), BigInt(0), "multiplication by zero");

    // Division and modulo: division truncates toward zero
    ExpectEqual(BigInt(123) / BigInt(10), BigInt(12), "positive division");
    ExpectEqual(BigInt(-23) / BigInt(5), BigInt(-4), "negative dividend division");
    ExpectEqual(BigInt(23) / BigInt(-5), BigInt(-4), "negative divisor division");
    ExpectEqual(BigInt(-23) / BigInt(-5), BigInt(4), "both negative division");

    ExpectEqual(BigInt(23) % BigInt(5), BigInt(3), "positive modulo");
    ExpectEqual(BigInt(-23) % BigInt(5), BigInt(-3), "negative dividend modulo");
    ExpectEqual(BigInt(23) % BigInt(-5), BigInt(3), "negative divisor modulo");
    ExpectEqual(BigInt(-23) % BigInt(-5), BigInt(-3), "both negative modulo");

    ExpectInvalidArgument(
        []
        {
            BigInt value = 10;
            value /= 0;
        },
        "division by zero throws");

    ExpectInvalidArgument(
        []
        {
            BigInt value = 10;
            value %= 0;
        },
        "modulo by zero throws");

    // Unary operators and increment/decrement
    ExpectEqual(-BigInt(23), BigInt(-23), "unary minus");
    ExpectEqual(-BigInt(0), BigInt(0), "unary minus preserves canonical zero");
    ExpectEqual(~BigInt(5), BigInt(-6), "bitwise not positive");
    ExpectEqual(~BigInt(-5), BigInt(4), "bitwise not negative");

    BigInt increment = 9;
    ExpectEqual(++increment, BigInt(10), "pre-increment");
    ExpectEqual(increment, BigInt(10), "pre-increment mutates");

    BigInt postIncrement = 9;
    BigInt oldIncrement = postIncrement++;
    ExpectEqual(oldIncrement, BigInt(9), "post-increment returns old value");
    ExpectEqual(postIncrement, BigInt(10), "post-increment mutates");

    BigInt decrement = 0;
    ExpectEqual(--decrement, BigInt(-1), "pre-decrement");
    ExpectEqual(decrement, BigInt(-1), "pre-decrement mutates");

    BigInt postDecrement = 0;
    BigInt oldDecrement = postDecrement--;

    ExpectEqual(oldDecrement, BigInt(0),
                "post-decrement returns old value");
    ExpectEqual(postDecrement, BigInt(-1),
                "post-decrement mutates left operand");

    // Comparisons
    Check(BigInt(5) == BigInt(5), "equality");
    Check(BigInt(5) != BigInt(-5), "inequality");
    Check(BigInt(-3) < BigInt(2), "negative is less than positive");
    Check(BigInt(-3) > BigInt(-5), "negative comparison reverses magnitude");
    Check(BigInt(5) >= BigInt(5), "greater or equal equality");
    Check(BigInt(-5) <= BigInt(-5), "less or equal equality");

    // Shifts
    ExpectEqual(BigInt(5) << 3, BigInt(40), "left shift");
    ExpectEqual(BigInt(-3) << 1, BigInt(-6), "negative left shift");
    ExpectEqual(BigInt(23) >> 1, BigInt(11), "positive right shift");
    ExpectEqual(BigInt(-3) >> 1, BigInt(-2), "arithmetic right shift");
    ExpectEqual(BigInt(-13) >> 2, BigInt(-4), "repeated arithmetic right shift");
    ExpectEqual(BigInt(-1) >> 100, BigInt(-1), "negative one right shift");
    ExpectEqual(BigInt(0) >> 100, BigInt(0), "zero right shift");

    ExpectInvalidArgument(
        []
        {
            BigInt value = 1;
            value <<= -1;
        },
        "negative left shift count throws");

    ExpectInvalidArgument(
        []
        {
            BigInt value = 1;
            value >>= -1;
        },
        "negative right shift count throws");

    // Bitwise operations, including signed values
    ExpectEqual(BigInt(13) & BigInt(6), BigInt(4), "bitwise and");
    ExpectEqual(BigInt(13) | BigInt(6), BigInt(15), "bitwise or");
    ExpectEqual(BigInt(13) ^ BigInt(6), BigInt(11), "bitwise xor");
    ExpectEqual(BigInt(-3) & BigInt(-5), BigInt(-7), "signed bitwise and");
    ExpectEqual(BigInt(-3) | BigInt(-5), BigInt(-1), "signed bitwise or");
    ExpectEqual(BigInt(-3) ^ BigInt(-5), BigInt(6), "signed bitwise xor");

    // Value semantics: binary operators must not mutate their left operand
    BigInt original = 13;
    BigInt result = original | BigInt(6);
    ExpectEqual(original, BigInt(13), "binary or preserves left operand");
    ExpectEqual(result, BigInt(15), "binary or result");

    BigInt copySource = 123;
    BigInt copy = copySource;
    copy += 1;
    ExpectEqual(copySource, BigInt(123), "copy is independent");
    ExpectEqual(copy, BigInt(124), "copy can mutate independently");

    BigInt selfAssignment = 42;
    BigInt &alias = selfAssignment;
    selfAssignment = alias;
    ExpectEqual(selfAssignment, BigInt(42), "self assignment");

    BigInt moveSource = 456;
    BigInt moved = std::move(moveSource);
    ExpectEqual(moved, BigInt(456), "move destination preserves value");

    BigInt notOriginal = 5;
    BigInt notResult = ~notOriginal;

    ExpectEqual(notOriginal, BigInt(5), "bitwise not preserves operand");
    ExpectEqual(notResult, BigInt(-6), "bitwise not result");

    // pow
    ExpectEqual(pow(BigInt(2), 10), BigInt(1024), "pow int exponent");
    ExpectEqual(pow(BigInt(-2), BigInt(3)), BigInt(-8), "pow BigInt exponent");
    ExpectEqual(pow(BigInt(123), 0), BigInt(1), "pow zero exponent");

    ExpectInvalidArgument(
        []
        {
            (void)pow(BigInt(2), -1);
        },
        "negative int exponent throws");

    ExpectInvalidArgument(
        []
        {
            (void)pow(BigInt(2), BigInt(-1));
        },
        "negative BigInt exponent throws");

    // Stream output
    std::ostringstream output;
    output << BigInt(-1000);
    Check(output.str() == "-1000", "stream output");

    // Compound operators mutate the left operand
    BigInt addAssign = 23;
    addAssign += BigInt(-50);
    ExpectEqual(addAssign, BigInt(-27), "operator+= mutates left operand");

    BigInt subtractAssign = 50;
    subtractAssign -= BigInt(73);
    ExpectEqual(subtractAssign, BigInt(-23), "operator-= mutates left operand");

    BigInt multiplyAssign = -23;
    multiplyAssign *= BigInt(45);
    ExpectEqual(multiplyAssign, BigInt(-1035), "operator*= mutates left operand");

    BigInt divideAssign = 123;
    divideAssign /= BigInt(10);
    ExpectEqual(divideAssign, BigInt(12), "operator/= mutates left operand");

    BigInt moduloAssign = -23;
    moduloAssign %= BigInt(5);
    ExpectEqual(moduloAssign, BigInt(-3), "operator%= mutates left operand");

    BigInt leftShiftAssign = 5;
    leftShiftAssign <<= 3;
    ExpectEqual(leftShiftAssign, BigInt(40), "operator<<= mutates left operand");

    BigInt rightShiftAssign = -13;
    rightShiftAssign >>= 2;
    ExpectEqual(rightShiftAssign, BigInt(-4), "operator>>= mutates left operand");

    BigInt andAssign = 13;
    andAssign &= BigInt(6);
    ExpectEqual(andAssign, BigInt(4), "operator&= mutates left operand");

    BigInt orAssign = 13;
    orAssign |= BigInt(6);
    ExpectEqual(orAssign, BigInt(15), "operator|= mutates left operand");

    BigInt xorAssign = 13;
    xorAssign ^= BigInt(6);
    ExpectEqual(xorAssign, BigInt(11), "operator^= mutates left operand");

    //Istream
    std::istringstream valid("-00023");
    BigInt value = 99;
    valid >> value;

    std::istringstream invalid("12a3");
    BigInt unchanged = 99;
    invalid >> unchanged;

    if (failures == 0)
    {
        std::cout << "All BigInt tests passed.\n";
        return 0;
    }

    std::cerr << failures << " test(s) failed.\n";
    return 1;
}