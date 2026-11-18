#include "test_macro.h"

import mini.test;

using mini::Rect;
using mini::RectInt;
using mini::Vector2;
using mini::Vector2Int;
using mini::Vector3;
using mini::Vector3Int;

constexpr int testCtor()
{
    Rect r;
    TEST_ENSURE(r.x == 0.f && r.y == 0.f && r.width == 0.f && r.height == 0.f);
    TEST_ENSURE(r == Rect::zero());

    Rect xywh(1.f, 2.f, 3.f, 4.f);
    TEST_ENSURE(xywh.x == 1.f && xywh.y == 2.f && xywh.width == 3.f && xywh.height == 4.f);

    Rect fromVector(Vector2(1.f, 2.f), Vector2(3.f, 4.f));
    TEST_ENSURE(fromVector == xywh);

    Rect fromInt(RectInt(-1, 2, 3, 4));
    TEST_ENSURE(fromInt == Rect(-1.f, 2.f, 3.f, 4.f));

    return 0;
}

constexpr int testEqual()
{
    TEST_ENSURE(Rect(1.f, 2.f, 3.f, 4.f) == Rect(1.f, 2.f, 3.f, 4.f));
    TEST_ENSURE(Rect(1.f, 2.f, 3.f, 4.f) != Rect(0.f, 2.f, 3.f, 4.f));
    TEST_ENSURE(Rect(1.f, 2.f, 3.f, 4.f) != Rect(1.f, 0.f, 3.f, 4.f));
    TEST_ENSURE(Rect(1.f, 2.f, 3.f, 4.f) != Rect(1.f, 2.f, 0.f, 4.f));
    TEST_ENSURE(Rect(1.f, 2.f, 3.f, 4.f) != Rect(1.f, 2.f, 3.f, 0.f));

    return 0;
}

constexpr int testAccessor()
{
    Rect r(1.f, 2.f, 3.f, 4.f);

    TEST_ENSURE(r.position() == Vector2(1.f, 2.f));
    TEST_ENSURE(r.size() == Vector2(3.f, 4.f));
    TEST_ENSURE(r.min() == Vector2(1.f, 2.f));
    TEST_ENSURE(r.max() == Vector2(4.f, 6.f));
    TEST_ENSURE(r.center() == Vector2(2.5f, 4.f));

    TEST_ENSURE(Rect(-2.f, -2.f, 4.f, 4.f).center() == Vector2::zero());

    return 0;
}

constexpr int testContains()
{
    Rect r(0.f, 0.f, 10.f, 5.f);

    TEST_ENSURE(r.contains(Vector2(5.f, 2.5f)));
    TEST_ENSURE(r.contains(Vector2(0.f, 0.f)));
    TEST_ENSURE(r.contains(Vector2(10.f, 5.f)));
    TEST_ENSURE(!r.contains(Vector2(-0.5f, 2.f)));
    TEST_ENSURE(!r.contains(Vector2(10.5f, 2.f)));
    TEST_ENSURE(!r.contains(Vector2(5.f, -0.5f)));
    TEST_ENSURE(!r.contains(Vector2(5.f, 5.5f)));

    TEST_ENSURE(r.contains(Vector3(5.f, 2.5f, 100.f)));
    TEST_ENSURE(!r.contains(Vector3(11.f, 2.5f, 0.f)));

    return 0;
}

constexpr int testIntersect()
{
    Rect r(0.f, 0.f, 10.f, 10.f);

    TEST_ENSURE(r.intersects(Rect(5.f, 5.f, 10.f, 10.f)));
    TEST_ENSURE(r.intersects(Rect(-5.f, -5.f, 10.f, 10.f)));
    TEST_ENSURE(r.intersects(Rect(2.f, 2.f, 2.f, 2.f)));
    TEST_ENSURE(r.intersects(Rect(-5.f, -5.f, 20.f, 20.f)));
    TEST_ENSURE(r.intersects(r));
    TEST_ENSURE(!r.intersects(Rect(10.f, 0.f, 5.f, 5.f)));
    TEST_ENSURE(!r.intersects(Rect(0.f, 10.f, 5.f, 5.f)));
    TEST_ENSURE(!r.intersects(Rect(20.f, 20.f, 5.f, 5.f)));
    TEST_ENSURE(!r.intersects(Rect(-5.f, 0.f, 5.f, 5.f)));
    TEST_ENSURE(!r.intersects(Rect(0.f, -5.f, 5.f, 5.f)));
    TEST_ENSURE(r.intersects(Rect(9.5f, 9.5f, 1.f, 1.f)));

    TEST_ENSURE(Rect::intersect(r, Rect(5.f, 5.f, 10.f, 10.f)));
    TEST_ENSURE(!Rect::intersect(r, Rect(20.f, 20.f, 5.f, 5.f)));

    return 0;
}

constexpr int testOverlap()
{
    Rect r(0.f, 0.f, 10.f, 10.f);

    TEST_ENSURE(r.overlaps(Rect(2.f, 2.f, 2.f, 2.f)));
    TEST_ENSURE(!r.overlaps(Rect(20.f, 20.f, 5.f, 5.f)));
    TEST_ENSURE(!r.overlaps(Rect(-5.f, -5.f, 20.f, 20.f)));
    TEST_ENSURE(!Rect(2.f, 2.f, 2.f, 2.f).overlaps(r));
    TEST_ENSURE(!r.overlaps(Rect(8.f, 8.f, 5.f, 5.f)));
    TEST_ENSURE(!r.overlaps(Rect(-1.f, 2.f, 2.f, 2.f)));

    TEST_ENSURE(Rect::overlap(r, Rect(2.f, 2.f, 2.f, 2.f)));

    return 0;
}

constexpr int testIntCtor()
{
    RectInt r;
    TEST_ENSURE(r.x == 0 && r.y == 0 && r.width == 0 && r.height == 0);
    TEST_ENSURE(r == RectInt::zero());

    RectInt xywh(1, 2, 3, 4);
    TEST_ENSURE(xywh.x == 1 && xywh.y == 2 && xywh.width == 3 && xywh.height == 4);

    RectInt fromVector(Vector2Int(1, 2), Vector2Int(3, 4));
    TEST_ENSURE(fromVector == xywh);

    return 0;
}

constexpr int testIntEqual()
{
    TEST_ENSURE(RectInt(1, 2, 3, 4) == RectInt(1, 2, 3, 4));
    TEST_ENSURE(RectInt(1, 2, 3, 4) != RectInt(0, 2, 3, 4));
    TEST_ENSURE(RectInt(1, 2, 3, 4) != RectInt(1, 0, 3, 4));
    TEST_ENSURE(RectInt(1, 2, 3, 4) != RectInt(1, 2, 0, 4));
    TEST_ENSURE(RectInt(1, 2, 3, 4) != RectInt(1, 2, 3, 0));

    return 0;
}

constexpr int testIntAccessor()
{
    RectInt r(1, 2, 3, 4);

    TEST_ENSURE(r.position() == Vector2Int(1, 2));
    TEST_ENSURE(r.size() == Vector2Int(3, 4));
    TEST_ENSURE(r.min() == Vector2Int(1, 2));
    TEST_ENSURE(r.max() == Vector2Int(4, 6));
    TEST_ENSURE(r.center() == Vector2(2.5f, 4.f));

    TEST_ENSURE(RectInt(-2, -2, 4, 4).center() == Vector2::zero());

    return 0;
}

constexpr int testIntContains()
{
    RectInt r(0, 0, 10, 5);

    TEST_ENSURE(r.contains(Vector2Int(5, 2)));
    TEST_ENSURE(r.contains(Vector2Int(0, 0)));
    TEST_ENSURE(r.contains(Vector2Int(10, 5)));
    TEST_ENSURE(!r.contains(Vector2Int(-1, 2)));
    TEST_ENSURE(!r.contains(Vector2Int(11, 2)));
    TEST_ENSURE(!r.contains(Vector2Int(5, -1)));
    TEST_ENSURE(!r.contains(Vector2Int(5, 6)));

    TEST_ENSURE(r.contains(Vector3Int(5, 2, 100)));
    TEST_ENSURE(!r.contains(Vector3Int(11, 2, 0)));

    return 0;
}

constexpr int testIntIntersect()
{
    RectInt r(0, 0, 10, 10);

    TEST_ENSURE(r.intersects(RectInt(5, 5, 10, 10)));
    TEST_ENSURE(r.intersects(RectInt(-5, -5, 10, 10)));
    TEST_ENSURE(r.intersects(RectInt(2, 2, 2, 2)));
    TEST_ENSURE(r.intersects(r));
    TEST_ENSURE(!r.intersects(RectInt(10, 0, 5, 5)));
    TEST_ENSURE(!r.intersects(RectInt(0, 10, 5, 5)));
    TEST_ENSURE(!r.intersects(RectInt(20, 20, 5, 5)));
    TEST_ENSURE(!r.intersects(RectInt(-5, 0, 5, 5)));
    TEST_ENSURE(!r.intersects(RectInt(0, -5, 5, 5)));
    TEST_ENSURE(r.intersects(RectInt(-5, -5, 20, 20)));

    TEST_ENSURE(RectInt::intersect(r, RectInt(5, 5, 10, 10)));
    TEST_ENSURE(!RectInt::intersect(r, RectInt(20, 20, 5, 5)));

    return 0;
}

constexpr int testIntOverlap()
{
    RectInt r(0, 0, 10, 10);

    TEST_ENSURE(r.overlaps(RectInt(2, 2, 2, 2)));
    TEST_ENSURE(!r.overlaps(RectInt(20, 20, 5, 5)));
    TEST_ENSURE(!r.overlaps(RectInt(-5, -5, 20, 20)));
    TEST_ENSURE(!RectInt(2, 2, 2, 2).overlaps(r));
    TEST_ENSURE(!r.overlaps(RectInt(8, 8, 5, 5)));
    TEST_ENSURE(!r.overlaps(RectInt(-1, 2, 2, 2)));

    TEST_ENSURE(RectInt::overlap(r, RectInt(2, 2, 2, 2)));

    return 0;
}

int main()
{
    TEST_CONSTEXPR(testCtor);
    TEST_CONSTEXPR(testEqual);
    TEST_CONSTEXPR(testAccessor);
    TEST_CONSTEXPR(testContains);
    TEST_CONSTEXPR(testIntersect);
    TEST_CONSTEXPR(testOverlap);

    TEST_CONSTEXPR(testIntCtor);
    TEST_CONSTEXPR(testIntEqual);
    TEST_CONSTEXPR(testIntAccessor);
    TEST_CONSTEXPR(testIntContains);
    TEST_CONSTEXPR(testIntIntersect);
    TEST_CONSTEXPR(testIntOverlap);

    return 0;
}
