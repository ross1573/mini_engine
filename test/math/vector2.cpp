#include "test_macro.h"

import mini.test;

using mini::Vector2;
using mini::Vector2Int;

constexpr int testCtor()
{
    Vector2 v;
    TEST_ENSURE(v.x == 0.f && v.y == 0.f);

    Vector2 xy(1.5f, -2.5f);
    TEST_ENSURE(xy.x == 1.5f && xy.y == -2.5f);

    Vector2 scalar(3.f);
    TEST_ENSURE(scalar.x == 3.f && scalar.y == 3.f);

    Vector2 fromInt(4);
    TEST_ENSURE(fromInt.x == 4.f && fromInt.y == 4.f);

    return 0;
}

constexpr int testConstant()
{
    TEST_ENSURE(Vector2::zero() == Vector2(0.f, 0.f));
    TEST_ENSURE(Vector2::one() == Vector2(1.f, 1.f));
    TEST_ENSURE(Vector2::unitX() == Vector2(1.f, 0.f));
    TEST_ENSURE(Vector2::unitY() == Vector2(0.f, 1.f));

    return 0;
}

constexpr int testEqual()
{
    TEST_ENSURE(Vector2(1.f, 2.f) == Vector2(1.f, 2.f));
    TEST_ENSURE(!(Vector2(1.f, 2.f) == Vector2(2.f, 1.f)));
    TEST_ENSURE(Vector2(1.f, 2.f) != Vector2(1.f, 3.f));
    TEST_ENSURE(Vector2(1.f, 2.f) != Vector2(0.f, 2.f));

    return 0;
}

constexpr int testUnary()
{
    Vector2 v(1.5f, -2.f);

    TEST_ENSURE(+v == v);
    TEST_ENSURE(-v == Vector2(-1.5f, 2.f));

    return 0;
}

constexpr int testAdd()
{
    TEST_ENSURE(Vector2(1.f, 2.f) + Vector2(3.f, -4.f) == Vector2(4.f, -2.f));
    TEST_ENSURE(Vector2(1.f, 2.f) + Vector2::zero() == Vector2(1.f, 2.f));

    Vector2 v(1.f, 2.f);
    Vector2& ref = (v += Vector2(0.5f, 0.5f));
    TEST_ENSURE(&ref == &v);
    TEST_ENSURE(v == Vector2(1.5f, 2.5f));

    return 0;
}

constexpr int testSubtract()
{
    TEST_ENSURE(Vector2(1.f, 2.f) - Vector2(3.f, -4.f) == Vector2(-2.f, 6.f));
    TEST_ENSURE(Vector2(1.f, 2.f) - Vector2(1.f, 2.f) == Vector2::zero());

    Vector2 v(1.f, 2.f);
    Vector2& ref = (v -= Vector2(0.5f, 4.f));
    TEST_ENSURE(&ref == &v);
    TEST_ENSURE(v == Vector2(0.5f, -2.f));

    return 0;
}

constexpr int testMultiply()
{
    TEST_ENSURE(Vector2(1.f, -2.f) * 3.f == Vector2(3.f, -6.f));
    TEST_ENSURE(Vector2(1.f, -2.f) * 2 == Vector2(2.f, -4.f));
    TEST_ENSURE(3.f * Vector2(1.f, -2.f) == Vector2(3.f, -6.f));

    Vector2 v(1.f, 2.f);
    Vector2& ref = (v *= 4);
    TEST_ENSURE(&ref == &v);
    TEST_ENSURE(v == Vector2(4.f, 8.f));

    return 0;
}

constexpr int testDivide()
{
    TEST_ENSURE(Vector2(3.f, -6.f) / 3.f == Vector2(1.f, -2.f));
    TEST_ENSURE(Vector2(3.f, -6.f) / 2 == Vector2(1.5f, -3.f));

    Vector2 v(4.f, 8.f);
    Vector2& ref = (v /= 4);
    TEST_ENSURE(&ref == &v);
    TEST_ENSURE(v == Vector2(1.f, 2.f));

    return 0;
}

constexpr int testDot()
{
    Vector2 a(1.f, 2.f);
    Vector2 b(3.f, -4.f);

    TEST_ENSURE(a.dot(b) == -5.f);
    TEST_ENSURE(Vector2::dot(a, b) == -5.f);
    TEST_ENSURE(Vector2::unitX().dot(Vector2::unitY()) == 0.f);
    TEST_ENSURE(a.dot(a) == 5.f);

    return 0;
}

constexpr int testCross()
{
    Vector2 a(1.f, 2.f);
    Vector2 b(3.f, -4.f);

    TEST_ENSURE(a.cross(b) == -10.f);
    TEST_ENSURE(Vector2::cross(a, b) == -10.f);
    TEST_ENSURE(Vector2::cross(b, a) == 10.f);
    TEST_ENSURE(Vector2::unitX().cross(Vector2::unitY()) == 1.f);
    TEST_ENSURE(a.cross(a) == 0.f);

    return 0;
}

constexpr int testClamp()
{
    Vector2 min(-1.f, 0.f);
    Vector2 max(1.f, 2.f);

    Vector2 inside(0.5f, 1.f);
    inside.clamp(min, max);
    TEST_ENSURE(inside == Vector2(0.5f, 1.f));

    Vector2 below(-5.f, -5.f);
    below.clamp(min, max);
    TEST_ENSURE(below == min);

    Vector2 above(5.f, 5.f);
    above.clamp(min, max);
    TEST_ENSURE(above == max);

    Vector2 mixed(-5.f, 5.f);
    mixed.clamp(min, max);
    TEST_ENSURE(mixed == Vector2(-1.f, 2.f));

    return 0;
}

constexpr int testLerp()
{
    Vector2 a(0.f, 2.f);
    Vector2 b(10.f, -2.f);

    TEST_ENSURE(Vector2::lerp(a, b, 0.f) == a);
    TEST_ENSURE(Vector2::lerp(a, b, 1.f) == b);
    TEST_ENSURE(Vector2::lerp(a, b, 0.5f) == Vector2(5.f, 0.f));
    TEST_ENSURE(Vector2::lerp(a, b, 2.f) == Vector2(20.f, -6.f));

    return 0;
}

int testLength()
{
    TEST_ENSURE(Vector2(3.f, 4.f).lengthSquared() == 25.f);
    TEST_ENSURE(Vector2(3.f, 4.f).length() == 5.f);
    TEST_ENSURE(Vector2(-3.f, -4.f).length() == 5.f);
    TEST_ENSURE(Vector2::zero().length() == 0.f);

    return 0;
}

int testNormalize()
{
    Vector2 v(3.f, 4.f);
    v.normalize();
    TEST_ENSURE(v == Vector2(0.6f, 0.8f));

    TEST_ENSURE(Vector2::normalize(Vector2(0.f, -2.f)) == Vector2(0.f, -1.f));

    Vector2 unit = Vector2::unitX();
    unit.normalize();
    TEST_ENSURE(unit == Vector2::unitX());

    return 0;
}

int testDistance()
{
    Vector2 a(1.f, 1.f);
    Vector2 b(4.f, 5.f);

    TEST_ENSURE(Vector2::distanceSquared(a, b) == 25.f);
    TEST_ENSURE(Vector2::distance(a, b) == 5.f);
    TEST_ENSURE(Vector2::distance(a, a) == 0.f);

    return 0;
}

int testData()
{
    Vector2 v(1.f, 2.f);
    TEST_ENSURE(v.data[0] == 1.f);
    TEST_ENSURE(v.data[1] == 2.f);

    v.data[1] = 5.f;
    TEST_ENSURE(v.y == 5.f);

    Vector2Int iv(3, 4);
    TEST_ENSURE(iv.data[0] == 3);
    TEST_ENSURE(iv.data[1] == 4);

    iv.data[0] = -1;
    TEST_ENSURE(iv.x == -1);

    return 0;
}

constexpr int testIntCtor()
{
    Vector2Int v;
    TEST_ENSURE(v.x == 0 && v.y == 0);

    Vector2Int xy(1, -2);
    TEST_ENSURE(xy.x == 1 && xy.y == -2);

    Vector2Int unsignedXY(3u, 4u);
    TEST_ENSURE(unsignedXY.x == 3 && unsignedXY.y == 4);

    Vector2Int scalar(7);
    TEST_ENSURE(scalar.x == 7 && scalar.y == 7);

    Vector2Int fromVector(Vector2(1.9f, -1.9f));
    TEST_ENSURE(fromVector.x == 1 && fromVector.y == -1);

    return 0;
}

constexpr int testIntConstant()
{
    TEST_ENSURE(Vector2Int::zero() == Vector2Int(0, 0));
    TEST_ENSURE(Vector2Int::one() == Vector2Int(1, 1));
    TEST_ENSURE(Vector2Int::unitX() == Vector2Int(1, 0));
    TEST_ENSURE(Vector2Int::unitY() == Vector2Int(0, 1));

    return 0;
}

constexpr int testIntConvert()
{
    Vector2 v = static_cast<Vector2>(Vector2Int(3, -4));
    TEST_ENSURE(v == Vector2(3.f, -4.f));

    return 0;
}

constexpr int testIntOperator()
{
    TEST_ENSURE(Vector2Int(1, 2) == Vector2Int(1, 2));
    TEST_ENSURE(Vector2Int(1, 2) != Vector2Int(2, 1));

    TEST_ENSURE(+Vector2Int(1, -2) == Vector2Int(1, -2));
    TEST_ENSURE(-Vector2Int(1, -2) == Vector2Int(-1, 2));

    TEST_ENSURE(Vector2Int(1, 2) + Vector2Int(3, -4) == Vector2Int(4, -2));
    TEST_ENSURE(Vector2Int(1, 2) - Vector2Int(3, -4) == Vector2Int(-2, 6));
    TEST_ENSURE(Vector2Int(1, -2) * 3 == Vector2Int(3, -6));
    TEST_ENSURE(3 * Vector2Int(1, -2) == Vector2Int(3, -6));
    TEST_ENSURE(Vector2Int(7, -7) / 2 == Vector2Int(3, -3));

    Vector2Int v(1, 2);
    TEST_ENSURE(&(v += Vector2Int(2, 2)) == &v);
    TEST_ENSURE(v == Vector2Int(3, 4));
    TEST_ENSURE(&(v -= Vector2Int(1, 1)) == &v);
    TEST_ENSURE(v == Vector2Int(2, 3));
    TEST_ENSURE(&(v *= 4) == &v);
    TEST_ENSURE(v == Vector2Int(8, 12));
    TEST_ENSURE(&(v /= 3) == &v);
    TEST_ENSURE(v == Vector2Int(2, 4));

    return 0;
}

constexpr int testIntClamp()
{
    Vector2Int min(-1, 0);
    Vector2Int max(1, 2);

    Vector2Int inside(0, 1);
    inside.clamp(min, max);
    TEST_ENSURE(inside == Vector2Int(0, 1));

    Vector2Int below(-5, -5);
    below.clamp(min, max);
    TEST_ENSURE(below == min);

    Vector2Int above(5, 5);
    above.clamp(min, max);
    TEST_ENSURE(above == max);

    return 0;
}

constexpr int testIntLengthSquared()
{
    TEST_ENSURE(Vector2Int(3, 4).lengthSquared() == 25);
    TEST_ENSURE(Vector2Int(-3, -4).lengthSquared() == 25);
    TEST_ENSURE(Vector2Int::zero().lengthSquared() == 0);

    TEST_ENSURE(Vector2Int::distanceSquared(Vector2Int(1, 1), Vector2Int(4, 5)) == 25);
    TEST_ENSURE(Vector2Int::distanceSquared(Vector2Int(-1, -1), Vector2Int(-1, -1)) == 0);

    return 0;
}

int testIntLength()
{
    TEST_ENSURE(Vector2Int(3, 4).length() == 5.f);
    TEST_ENSURE(Vector2Int(-3, 4).length() == 5.f);
    TEST_ENSURE(Vector2Int::zero().length() == 0.f);

    TEST_ENSURE(Vector2Int::distance(Vector2Int(1, 1), Vector2Int(4, 5)) == 5.f);

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
    TEST_CONSTEXPR(testDot);
    TEST_CONSTEXPR(testCross);
    TEST_CONSTEXPR(testClamp);
    TEST_CONSTEXPR(testLerp);
    TEST_FUNCTION(testLength);
    TEST_FUNCTION(testNormalize);
    TEST_FUNCTION(testDistance);
    TEST_FUNCTION(testData);

    TEST_CONSTEXPR(testIntCtor);
    TEST_CONSTEXPR(testIntConstant);
    TEST_CONSTEXPR(testIntConvert);
    TEST_CONSTEXPR(testIntOperator);
    TEST_CONSTEXPR(testIntClamp);
    TEST_CONSTEXPR(testIntLengthSquared);
    TEST_FUNCTION(testIntLength);

    return 0;
}
