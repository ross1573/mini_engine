#include "test_macro.h"

import mini.test;

using mini::Vector2;
using mini::Vector2Int;
using mini::Vector3;
using mini::Vector3Int;

constexpr int testCtor()
{
    Vector3 v;
    TEST_ENSURE(v.x == 0.f && v.y == 0.f && v.z == 0.f);

    Vector3 xyz(1.5f, -2.5f, 3.f);
    TEST_ENSURE(xyz.x == 1.5f && xyz.y == -2.5f && xyz.z == 3.f);

    Vector3 scalar(3.f);
    TEST_ENSURE(scalar.x == 3.f && scalar.y == 3.f && scalar.z == 3.f);

    Vector3 fromInt(4);
    TEST_ENSURE(fromInt == Vector3(4.f, 4.f, 4.f));

    return 0;
}

constexpr int testConstant()
{
    TEST_ENSURE(Vector3::zero() == Vector3(0.f, 0.f, 0.f));
    TEST_ENSURE(Vector3::one() == Vector3(1.f, 1.f, 1.f));
    TEST_ENSURE(Vector3::unitX() == Vector3(1.f, 0.f, 0.f));
    TEST_ENSURE(Vector3::unitY() == Vector3(0.f, 1.f, 0.f));
    TEST_ENSURE(Vector3::unitZ() == Vector3(0.f, 0.f, 1.f));
    TEST_ENSURE(Vector3::up() == Vector3(0.f, 1.f, 0.f));
    TEST_ENSURE(Vector3::down() == Vector3(0.f, -1.f, 0.f));
    TEST_ENSURE(Vector3::left() == Vector3(-1.f, 0.f, 0.f));
    TEST_ENSURE(Vector3::right() == Vector3(1.f, 0.f, 0.f));
    TEST_ENSURE(Vector3::forward() == Vector3(0.f, 0.f, 1.f));
    TEST_ENSURE(Vector3::backward() == Vector3(0.f, 0.f, -1.f));

    return 0;
}

constexpr int testEqual()
{
    TEST_ENSURE(Vector3(1.f, 2.f, 3.f) == Vector3(1.f, 2.f, 3.f));
    TEST_ENSURE(Vector3(1.f, 2.f, 3.f) != Vector3(0.f, 2.f, 3.f));
    TEST_ENSURE(Vector3(1.f, 2.f, 3.f) != Vector3(1.f, 0.f, 3.f));
    TEST_ENSURE(Vector3(1.f, 2.f, 3.f) != Vector3(1.f, 2.f, 0.f));

    return 0;
}

constexpr int testArithmetic()
{
    TEST_ENSURE(+Vector3(1.f, -2.f, 3.f) == Vector3(1.f, -2.f, 3.f));
    TEST_ENSURE(-Vector3(1.f, -2.f, 3.f) == Vector3(-1.f, 2.f, -3.f));

    TEST_ENSURE(Vector3(1.f, 2.f, 3.f) + Vector3(3.f, -4.f, 0.5f) == Vector3(4.f, -2.f, 3.5f));
    TEST_ENSURE(Vector3(1.f, 2.f, 3.f) - Vector3(3.f, -4.f, 0.5f) == Vector3(-2.f, 6.f, 2.5f));
    TEST_ENSURE(Vector3(1.f, -2.f, 3.f) * 2.f == Vector3(2.f, -4.f, 6.f));
    TEST_ENSURE(2.f * Vector3(1.f, -2.f, 3.f) == Vector3(2.f, -4.f, 6.f));
    TEST_ENSURE(Vector3(1.f, -2.f, 3.f) / 2 == Vector3(0.5f, -1.f, 1.5f));

    Vector3 v(1.f, 2.f, 3.f);
    TEST_ENSURE(&(v += Vector3(1.f, 1.f, 1.f)) == &v);
    TEST_ENSURE(v == Vector3(2.f, 3.f, 4.f));
    TEST_ENSURE(&(v -= Vector3(1.f, 2.f, 3.f)) == &v);
    TEST_ENSURE(v == Vector3(1.f, 1.f, 1.f));
    TEST_ENSURE(&(v *= 4) == &v);
    TEST_ENSURE(v == Vector3(4.f, 4.f, 4.f));
    TEST_ENSURE(&(v /= 8.f) == &v);
    TEST_ENSURE(v == Vector3(0.5f, 0.5f, 0.5f));

    return 0;
}

constexpr int testDot()
{
    Vector3 a(1.f, 2.f, 3.f);
    Vector3 b(4.f, -5.f, 6.f);

    TEST_ENSURE(a.dot(b) == 12.f);
    TEST_ENSURE(Vector3::dot(a, b) == 12.f);
    TEST_ENSURE(Vector3::unitX().dot(Vector3::unitZ()) == 0.f);
    TEST_ENSURE(a.dot(a) == 14.f);

    return 0;
}

constexpr int testCross()
{
    TEST_ENSURE(Vector3::unitX().cross(Vector3::unitY()) == Vector3::unitZ());
    TEST_ENSURE(Vector3::unitY().cross(Vector3::unitZ()) == Vector3::unitX());
    TEST_ENSURE(Vector3::unitZ().cross(Vector3::unitX()) == Vector3::unitY());

    Vector3 a(1.f, 2.f, 3.f);
    Vector3 b(4.f, 5.f, 6.f);
    Vector3 c = Vector3::cross(a, b);

    TEST_ENSURE(c == Vector3(-3.f, 6.f, -3.f));
    TEST_ENSURE(a.cross(b) == c);
    TEST_ENSURE(Vector3::cross(b, a) == -c);
    TEST_ENSURE(a.cross(a) == Vector3::zero());

    return 0;
}

constexpr int testClamp()
{
    Vector3 min(-1.f, 0.f, 1.f);
    Vector3 max(1.f, 2.f, 3.f);

    Vector3 inside(0.f, 1.f, 2.f);
    inside.clamp(min, max);
    TEST_ENSURE(inside == Vector3(0.f, 1.f, 2.f));

    Vector3 below(-5.f, -5.f, -5.f);
    below.clamp(min, max);
    TEST_ENSURE(below == min);

    Vector3 above(5.f, 5.f, 5.f);
    above.clamp(min, max);
    TEST_ENSURE(above == max);

    Vector3 mixed(-5.f, 1.f, 5.f);
    mixed.clamp(min, max);
    TEST_ENSURE(mixed == Vector3(-1.f, 1.f, 3.f));

    return 0;
}

constexpr int testLerp()
{
    Vector3 a(0.f, 2.f, -4.f);
    Vector3 b(10.f, -2.f, 4.f);

    TEST_ENSURE(Vector3::lerp(a, b, 0.f) == a);
    TEST_ENSURE(Vector3::lerp(a, b, 1.f) == b);
    TEST_ENSURE(Vector3::lerp(a, b, 0.5f) == Vector3(5.f, 0.f, 0.f));
    TEST_ENSURE(Vector3::lerp(a, b, 2.f) == Vector3(20.f, -6.f, 12.f));

    return 0;
}

constexpr int testConvert()
{
    Vector2 v = static_cast<Vector2>(Vector3(1.f, 2.f, 3.f));
    TEST_ENSURE(v == Vector2(1.f, 2.f));

    return 0;
}

int testLength()
{
    TEST_ENSURE(Vector3(2.f, 3.f, 6.f).lengthSquared() == 49.f);
    TEST_ENSURE(Vector3(2.f, 3.f, 6.f).length() == 7.f);
    TEST_ENSURE(Vector3(-2.f, -3.f, -6.f).length() == 7.f);
    TEST_ENSURE(Vector3::zero().length() == 0.f);

    return 0;
}

int testNormalize()
{
    Vector3 v(0.f, 3.f, 4.f);
    v.normalize();
    TEST_ENSURE(v == Vector3(0.f, 0.6f, 0.8f));

    TEST_ENSURE(Vector3::normalize(Vector3(0.f, 0.f, -2.f)) == Vector3(0.f, 0.f, -1.f));

    return 0;
}

int testDistance()
{
    Vector3 a(1.f, 1.f, 1.f);
    Vector3 b(3.f, 4.f, 7.f);

    TEST_ENSURE(Vector3::distanceSquared(a, b) == 49.f);
    TEST_ENSURE(Vector3::distance(a, b) == 7.f);
    TEST_ENSURE(Vector3::distance(a, a) == 0.f);

    return 0;
}

int testData()
{
    Vector3 v(1.f, 2.f, 3.f);
    TEST_ENSURE(v.data[0] == 1.f && v.data[1] == 2.f && v.data[2] == 3.f);

    v.data[2] = 5.f;
    TEST_ENSURE(v.z == 5.f);

    Vector3Int iv(3, 4, 5);
    TEST_ENSURE(iv.data[0] == 3 && iv.data[1] == 4 && iv.data[2] == 5);

    iv.data[2] = -1;
    TEST_ENSURE(iv.z == -1);

    return 0;
}

constexpr int testIntCtor()
{
    Vector3Int v;
    TEST_ENSURE(v.x == 0 && v.y == 0 && v.z == 0);

    Vector3Int xyz(1, -2, 3);
    TEST_ENSURE(xyz.x == 1 && xyz.y == -2 && xyz.z == 3);

    Vector3Int unsignedXYZ(3u, 4u, 5u);
    TEST_ENSURE(unsignedXYZ.x == 3 && unsignedXYZ.y == 4 && unsignedXYZ.z == 5);

    Vector3Int scalar(7);
    TEST_ENSURE(scalar.x == 7 && scalar.y == 7 && scalar.z == 7);

    Vector3Int fromVector(Vector3(1.9f, -1.9f, 0.5f));
    TEST_ENSURE(fromVector == Vector3Int(1, -1, 0));

    return 0;
}

constexpr int testIntConstant()
{
    TEST_ENSURE(Vector3Int::zero() == Vector3Int(0, 0, 0));
    TEST_ENSURE(Vector3Int::one() == Vector3Int(1, 1, 1));
    TEST_ENSURE(Vector3Int::unitX() == Vector3Int(1, 0, 0));
    TEST_ENSURE(Vector3Int::unitY() == Vector3Int(0, 1, 0));
    TEST_ENSURE(Vector3Int::unitZ() == Vector3Int(0, 0, 1));
    TEST_ENSURE(Vector3Int::up() == Vector3Int(0, 1, 0));
    TEST_ENSURE(Vector3Int::down() == Vector3Int(0, -1, 0));
    TEST_ENSURE(Vector3Int::left() == Vector3Int(-1, 0, 0));
    TEST_ENSURE(Vector3Int::right() == Vector3Int(1, 0, 0));
    TEST_ENSURE(Vector3Int::forward() == Vector3Int(0, 0, 1));
    TEST_ENSURE(Vector3Int::backward() == Vector3Int(0, 0, -1));

    return 0;
}

constexpr int testIntConvert()
{
    Vector3Int v(3, -4, 5);

    TEST_ENSURE(static_cast<Vector2Int>(v) == Vector2Int(3, -4));
    TEST_ENSURE(static_cast<Vector2>(v) == Vector2(3.f, -4.f));
    TEST_ENSURE(static_cast<Vector3>(v) == Vector3(3.f, -4.f, 5.f));

    return 0;
}

constexpr int testIntOperator()
{
    TEST_ENSURE(Vector3Int(1, 2, 3) == Vector3Int(1, 2, 3));
    TEST_ENSURE(Vector3Int(1, 2, 3) != Vector3Int(1, 2, 4));

    TEST_ENSURE(+Vector3Int(1, -2, 3) == Vector3Int(1, -2, 3));
    TEST_ENSURE(-Vector3Int(1, -2, 3) == Vector3Int(-1, 2, -3));

    TEST_ENSURE(Vector3Int(1, 2, 3) + Vector3Int(3, -4, 5) == Vector3Int(4, -2, 8));
    TEST_ENSURE(Vector3Int(1, 2, 3) - Vector3Int(3, -4, 5) == Vector3Int(-2, 6, -2));
    TEST_ENSURE(Vector3Int(1, -2, 3) * 3 == Vector3Int(3, -6, 9));
    TEST_ENSURE(3 * Vector3Int(1, -2, 3) == Vector3Int(3, -6, 9));
    TEST_ENSURE(Vector3Int(7, -7, 8) / 2 == Vector3Int(3, -3, 4));

    Vector3Int v(1, 2, 3);
    TEST_ENSURE(&(v += Vector3Int(2, 2, 2)) == &v);
    TEST_ENSURE(v == Vector3Int(3, 4, 5));
    TEST_ENSURE(&(v -= Vector3Int(1, 1, 1)) == &v);
    TEST_ENSURE(v == Vector3Int(2, 3, 4));
    TEST_ENSURE(&(v *= 3) == &v);
    TEST_ENSURE(v == Vector3Int(6, 9, 12));
    TEST_ENSURE(&(v /= 3) == &v);
    TEST_ENSURE(v == Vector3Int(2, 3, 4));

    return 0;
}

constexpr int testIntClamp()
{
    Vector3Int min(-1, 0, 1);
    Vector3Int max(1, 2, 3);

    Vector3Int mixed(-5, 1, 5);
    mixed.clamp(min, max);
    TEST_ENSURE(mixed == Vector3Int(-1, 1, 3));

    Vector3Int below(-5, -5, -5);
    below.clamp(min, max);
    TEST_ENSURE(below == min);

    Vector3Int above(5, 5, 5);
    above.clamp(min, max);
    TEST_ENSURE(above == max);

    return 0;
}

constexpr int testIntLengthSquared()
{
    TEST_ENSURE(Vector3Int(2, 3, 6).lengthSquared() == 49);
    TEST_ENSURE(Vector3Int(-2, -3, -6).lengthSquared() == 49);
    TEST_ENSURE(Vector3Int::zero().lengthSquared() == 0);

    TEST_ENSURE(Vector3Int::distanceSquared(Vector3Int(1, 1, 1), Vector3Int(3, 4, 7)) == 49);

    return 0;
}

int testIntLength()
{
    TEST_ENSURE(Vector3Int(2, 3, 6).length() == 7.f);
    TEST_ENSURE(Vector3Int(-2, 3, -6).length() == 7.f);
    TEST_ENSURE(Vector3Int::zero().length() == 0.f);

    TEST_ENSURE(Vector3Int::distance(Vector3Int(1, 1, 1), Vector3Int(3, 4, 7)) == 7.f);

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

    TEST_CONSTEXPR(testIntCtor);
    TEST_CONSTEXPR(testIntConstant);
    TEST_CONSTEXPR(testIntConvert);
    TEST_CONSTEXPR(testIntOperator);
    TEST_CONSTEXPR(testIntClamp);
    TEST_CONSTEXPR(testIntLengthSquared);
    TEST_FUNCTION(testIntLength);

    return 0;
}
