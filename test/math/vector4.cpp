#include "test_macro.h"

import mini.test;

using mini::Vector3;
using mini::Vector4;

constexpr int testCtor()
{
    Vector4 v;
    TEST_ENSURE(v.x == 0.f && v.y == 0.f && v.z == 0.f && v.w == 0.f);

    Vector4 xyzw(1.5f, -2.5f, 3.f, -4.f);
    TEST_ENSURE(xyzw.x == 1.5f && xyzw.y == -2.5f && xyzw.z == 3.f && xyzw.w == -4.f);

    Vector4 scalar(3.f);
    TEST_ENSURE(scalar.x == 3.f && scalar.y == 3.f && scalar.z == 3.f && scalar.w == 3.f);

    Vector4 fromInt(4);
    TEST_ENSURE(fromInt == Vector4(4.f, 4.f, 4.f, 4.f));

    return 0;
}

constexpr int testConstant()
{
    TEST_ENSURE(Vector4::zero() == Vector4(0.f, 0.f, 0.f, 0.f));
    TEST_ENSURE(Vector4::one() == Vector4(1.f, 1.f, 1.f, 1.f));
    TEST_ENSURE(Vector4::unitX() == Vector4(1.f, 0.f, 0.f, 0.f));
    TEST_ENSURE(Vector4::unitY() == Vector4(0.f, 1.f, 0.f, 0.f));
    TEST_ENSURE(Vector4::unitZ() == Vector4(0.f, 0.f, 1.f, 0.f));
    TEST_ENSURE(Vector4::unitW() == Vector4(0.f, 0.f, 0.f, 1.f));

    return 0;
}

constexpr int testEqual()
{
    TEST_ENSURE(Vector4(1.f, 2.f, 3.f, 4.f) == Vector4(1.f, 2.f, 3.f, 4.f));
    TEST_ENSURE(Vector4(1.f, 2.f, 3.f, 4.f) != Vector4(0.f, 2.f, 3.f, 4.f));
    TEST_ENSURE(Vector4(1.f, 2.f, 3.f, 4.f) != Vector4(1.f, 0.f, 3.f, 4.f));
    TEST_ENSURE(Vector4(1.f, 2.f, 3.f, 4.f) != Vector4(1.f, 2.f, 0.f, 4.f));
    TEST_ENSURE(Vector4(1.f, 2.f, 3.f, 4.f) != Vector4(1.f, 2.f, 3.f, 0.f));

    return 0;
}

constexpr int testArithmetic()
{
    TEST_ENSURE(+Vector4(1.f, -2.f, 3.f, -4.f) == Vector4(1.f, -2.f, 3.f, -4.f));
    TEST_ENSURE(-Vector4(1.f, -2.f, 3.f, -4.f) == Vector4(-1.f, 2.f, -3.f, 4.f));

    TEST_ENSURE(Vector4(1.f, 2.f, 3.f, 4.f) + Vector4(1.f, -1.f, 0.5f, 2.f) == Vector4(2.f, 1.f, 3.5f, 6.f));
    TEST_ENSURE(Vector4(1.f, 2.f, 3.f, 4.f) - Vector4(1.f, -1.f, 0.5f, 2.f) == Vector4(0.f, 3.f, 2.5f, 2.f));
    TEST_ENSURE(Vector4(1.f, -2.f, 3.f, -4.f) * 2.f == Vector4(2.f, -4.f, 6.f, -8.f));
    TEST_ENSURE(2.f * Vector4(1.f, -2.f, 3.f, -4.f) == Vector4(2.f, -4.f, 6.f, -8.f));
    TEST_ENSURE(Vector4(1.f, -2.f, 3.f, -4.f) / 2 == Vector4(0.5f, -1.f, 1.5f, -2.f));

    Vector4 v(1.f, 2.f, 3.f, 4.f);
    TEST_ENSURE(&(v += Vector4(1.f)) == &v);
    TEST_ENSURE(v == Vector4(2.f, 3.f, 4.f, 5.f));
    TEST_ENSURE(&(v -= Vector4(1.f, 2.f, 3.f, 4.f)) == &v);
    TEST_ENSURE(v == Vector4(1.f, 1.f, 1.f, 1.f));
    TEST_ENSURE(&(v *= 4) == &v);
    TEST_ENSURE(v == Vector4(4.f));
    TEST_ENSURE(&(v /= 8.f) == &v);
    TEST_ENSURE(v == Vector4(0.5f));

    return 0;
}

constexpr int testDot()
{
    Vector4 a(1.f, 2.f, 3.f, 4.f);
    Vector4 b(4.f, -5.f, 6.f, -1.f);

    TEST_ENSURE(a.dot(b) == 8.f);
    TEST_ENSURE(Vector4::dot(a, b) == 8.f);
    TEST_ENSURE(Vector4::unitX().dot(Vector4::unitW()) == 0.f);
    TEST_ENSURE(a.dot(a) == 30.f);

    return 0;
}

constexpr int testCross()
{
    Vector4 a(1.f, 2.f, 3.f, 0.f);
    Vector4 b(4.f, 5.f, 6.f, 0.f);

    TEST_ENSURE(a.cross(b) == Vector4(-3.f, 6.f, -3.f, 0.f));
    TEST_ENSURE(Vector4::cross(a, b) == Vector4(-3.f, 6.f, -3.f, 0.f));
    TEST_ENSURE(Vector4::cross(Vector4::unitX(), Vector4::unitY()) == Vector4::unitZ());

    Vector4 c(1.f, 2.f, 3.f, 2.f);
    Vector4 d(4.f, 5.f, 6.f, 3.f);

    TEST_ENSURE(Vector4::cross(c, d) == -Vector4::cross(d, c));
    TEST_ENSURE(Vector4::cross(c, c) == Vector4::zero());
    TEST_ENSURE(Vector4::cross(c, d).w == 0.f);

    return 0;
}

constexpr int testClamp()
{
    Vector4 min(-1.f, 0.f, 1.f, 2.f);
    Vector4 max(1.f, 2.f, 3.f, 4.f);

    Vector4 mixed(-5.f, 1.f, 5.f, 3.f);
    mixed.clamp(min, max);
    TEST_ENSURE(mixed == Vector4(-1.f, 1.f, 3.f, 3.f));

    Vector4 below(-5.f);
    below.clamp(min, max);
    TEST_ENSURE(below == min);

    Vector4 above(5.f);
    above.clamp(min, max);
    TEST_ENSURE(above == max);

    return 0;
}

constexpr int testLerp()
{
    Vector4 a(0.f, 2.f, -4.f, 1.f);
    Vector4 b(10.f, -2.f, 4.f, 3.f);

    TEST_ENSURE(Vector4::lerp(a, b, 0.f) == a);
    TEST_ENSURE(Vector4::lerp(a, b, 1.f) == b);
    TEST_ENSURE(Vector4::lerp(a, b, 0.5f) == Vector4(5.f, 0.f, 0.f, 2.f));
    TEST_ENSURE(Vector4::lerp(a, b, 2.f) == Vector4(20.f, -6.f, 12.f, 5.f));

    return 0;
}

constexpr int testConvert()
{
    Vector3 v = static_cast<Vector3>(Vector4(1.f, 2.f, 3.f, 4.f));
    TEST_ENSURE(v == Vector3(1.f, 2.f, 3.f));

    return 0;
}

int testLength()
{
    TEST_ENSURE(Vector4(1.f, 2.f, 2.f, 4.f).lengthSquared() == 25.f);
    TEST_ENSURE(Vector4(1.f, 2.f, 2.f, 4.f).length() == 5.f);
    TEST_ENSURE(Vector4(-1.f, -2.f, -2.f, -4.f).length() == 5.f);
    TEST_ENSURE(Vector4::zero().length() == 0.f);

    return 0;
}

int testNormalize()
{
    Vector4 v(0.f, 3.f, 0.f, 4.f);
    v.normalize();
    TEST_ENSURE(v == Vector4(0.f, 0.6f, 0.f, 0.8f));

    TEST_ENSURE(Vector4::normalize(Vector4(0.f, 0.f, 0.f, -2.f)) == Vector4(0.f, 0.f, 0.f, -1.f));

    return 0;
}

int testDistance()
{
    Vector4 a(1.f, 1.f, 1.f, 1.f);
    Vector4 b(2.f, 3.f, 3.f, 5.f);

    TEST_ENSURE(Vector4::distanceSquared(a, b) == 25.f);
    TEST_ENSURE(Vector4::distance(a, b) == 5.f);
    TEST_ENSURE(Vector4::distance(a, a) == 0.f);

    return 0;
}

int testData()
{
    Vector4 v(1.f, 2.f, 3.f, 4.f);
    TEST_ENSURE(v.data[0] == 1.f && v.data[1] == 2.f && v.data[2] == 3.f && v.data[3] == 4.f);

    v.data[3] = 5.f;
    TEST_ENSURE(v.w == 5.f);

    return 0;
}

int main()
{
    TEST_CONSTEXPR(testCtor);
    TEST_CONSTEXPR(testConstant);
    TEST_CONSTEXPR(testEqual);
    TEST_CONSTEXPR(testArithmetic);
    TEST_CONSTEXPR(testDot);
    TEST_CONSTEXPR(testCross);
    TEST_CONSTEXPR(testClamp);
    TEST_CONSTEXPR(testLerp);
    TEST_CONSTEXPR(testConvert);
    TEST_FUNCTION(testLength);
    TEST_FUNCTION(testNormalize);
    TEST_FUNCTION(testDistance);
    TEST_FUNCTION(testData);

    return 0;
}
