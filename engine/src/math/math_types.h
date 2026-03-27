#pragma once
#include "defines.h"

typedef union vec2_u
{
    f32 elements[2];
    struct
    {
        union
        {
            f32 x,r,u,s;
        };
        union
        {
            f32 y,g,v,t;
        };
    };
}vec2;

typedef union vec3_u
{
    f32 elements[3];
    struct
    {
        union
        {
            f32 x,r,u,s;
        };
        union
        {
            f32 y,g,v,t;
        };
        union
        {
            f32 z,b,w,p;
        };
    };
}vec3;

typedef union vec4_u
{
    #if defined(KUSE_SIMD)
    //use for simd operation
    alignas(16) __m128 data;
    #endif
    f32 elements[4];
    struct
    {
        union
        {
            f32 x,r,s;
        };
        union
        {
            f32 y,g,t;
        };
        union
        {
            f32 z,b,p;
        };
        union
        {
            f32 w,a,q;
        };
    };
    
}vec4;