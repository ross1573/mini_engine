module;

#include "mini_engine.h"
#include "core/define.h"
#include "core/type.h"

export module mini.math:vertex;

import :vector3;
import :vector4;

export namespace mini
{

struct Vertex
{
    Vector3 position;
    Vector4 color;
};

} // namespace mini