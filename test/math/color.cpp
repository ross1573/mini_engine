#include "test_macro.h"

import mini.test;

using mini::Color;

constexpr int testCtor()
{
    Color c;
    TEST_ENSURE(c.r == 0.f && c.g == 0.f && c.b == 0.f && c.a == 0.f);

    Color rgba(0.1f, 0.2f, 0.3f, 0.4f);
    TEST_ENSURE(rgba.r == 0.1f && rgba.g == 0.2f && rgba.b == 0.3f && rgba.a == 0.4f);

    Color scalar(0.5f);
    TEST_ENSURE(scalar.r == 0.5f && scalar.g == 0.5f && scalar.b == 0.5f && scalar.a == 0.5f);

    Color fromInt(1);
    TEST_ENSURE(fromInt == Color(1.f, 1.f, 1.f, 1.f));

    return 0;
}

constexpr int testConstant()
{
    TEST_ENSURE(Color::white() == Color(1.f, 1.f, 1.f, 1.f));
    TEST_ENSURE(Color::black() == Color(0.f, 0.f, 0.f, 1.f));
    TEST_ENSURE(Color::clear() == Color(0.f, 0.f, 0.f, 0.f));
    TEST_ENSURE(Color::red() == Color(1.f, 0.f, 0.f, 1.f));
    TEST_ENSURE(Color::green() == Color(0.f, 1.f, 0.f, 1.f));
    TEST_ENSURE(Color::blue() == Color(0.f, 0.f, 1.f, 1.f));

    return 0;
}

constexpr int testEqual()
{
    TEST_ENSURE(Color(0.1f, 0.2f, 0.3f, 0.4f) == Color(0.1f, 0.2f, 0.3f, 0.4f));
    TEST_ENSURE(Color(0.1f, 0.2f, 0.3f, 0.4f) != Color(0.f, 0.2f, 0.3f, 0.4f));
    TEST_ENSURE(Color(0.1f, 0.2f, 0.3f, 0.4f) != Color(0.1f, 0.f, 0.3f, 0.4f));
    TEST_ENSURE(Color(0.1f, 0.2f, 0.3f, 0.4f) != Color(0.1f, 0.2f, 0.f, 0.4f));
    TEST_ENSURE(Color(0.1f, 0.2f, 0.3f, 0.4f) != Color(0.1f, 0.2f, 0.3f, 0.f));

    return 0;
}

constexpr int testUnary()
{
    Color c(0.25f, -0.5f, 0.75f, 1.f);

    TEST_ENSURE(+c == c);
    TEST_ENSURE(-c == Color(-0.25f, 0.5f, -0.75f, -1.f));

    return 0;
}

constexpr int testAdd()
{
    TEST_ENSURE(Color::red() + Color::green() == Color(1.f, 1.f, 0.f, 2.f));

    Color c = Color::clear();
    TEST_ENSURE(&(c += Color::blue()) == &c);
    TEST_ENSURE(c == Color::blue());

    return 0;
}

constexpr int testSubtract()
{
    TEST_ENSURE(Color::white() - Color::red() == Color(0.f, 1.f, 1.f, 0.f));

    Color c = Color::white();
    TEST_ENSURE(&(c -= Color::black()) == &c);
    TEST_ENSURE(c == Color(1.f, 1.f, 1.f, 0.f));

    return 0;
}

constexpr int testMultiply()
{
    Color c(0.25f, 0.5f, 0.75f, 1.f);

    TEST_ENSURE(c * 2.f == Color(0.5f, 1.f, 1.5f, 2.f));
    TEST_ENSURE(2.f * c == Color(0.5f, 1.f, 1.5f, 2.f));
    TEST_ENSURE(c * 0 == Color::clear());

    Color m = c;
    TEST_ENSURE(&(m *= 4) == &m);
    TEST_ENSURE(m == Color(1.f, 2.f, 3.f, 4.f));

    return 0;
}

constexpr int testDivide()
{
    Color c(0.5f, 1.f, 1.5f, 2.f);

    TEST_ENSURE(c / 2.f == Color(0.25f, 0.5f, 0.75f, 1.f));
    TEST_ENSURE(c / 2 == Color(0.25f, 0.5f, 0.75f, 1.f));

    Color d = c;
    TEST_ENSURE(&(d /= 4) == &d);
    TEST_ENSURE(d == Color(0.125f, 0.25f, 0.375f, 0.5f));

    return 0;
}

int testData()
{
    Color c(0.1f, 0.2f, 0.3f, 0.4f);
    TEST_ENSURE(c.data[0] == 0.1f && c.data[1] == 0.2f && c.data[2] == 0.3f && c.data[3] == 0.4f);

    c.data[3] = 1.f;
    TEST_ENSURE(c.a == 1.f);

    return 0;
}

int main()
{
    TEST_CONSTEXPR(testCtor);
    TEST_CONSTEXPR(testConstant);
    TEST_CONSTEXPR(testEqual);
    TEST_CONSTEXPR(testUnary);
    TEST_CONSTEXPR(testAdd);
    TEST_CONSTEXPR(testSubtract);
    TEST_CONSTEXPR(testMultiply);
    TEST_CONSTEXPR(testDivide);
    TEST_FUNCTION(testData);

    return 0;
}
