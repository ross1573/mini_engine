#include <metal_stdlib>

using namespace metal;

constant float4 positions[3] = {
    {  0.0,  0.5, 0.0, 1.0 },
    {  0.5, -0.5, 0.0, 1.0 },
    { -0.5, -0.5, 0.0, 1.0 }
};

constant float4 colors[3] = {
    { 1, 0, 0, 1 },
    { 0, 1, 0, 1 },
    { 0, 0, 1, 1 }
};

struct VertexOut {
    float4 position [[position]];
    float4 color;
};

struct FragmentOut {
    float4 color [[color(0)]];
};

vertex VertexOut VertexMain(uint id [[vertex_id]])
{
    return { .position = positions[id], .color = colors[id] };
}

fragment FragmentOut FragmentMain(const VertexOut in [[stage_in]])
{
    return { .color = in.color };
}