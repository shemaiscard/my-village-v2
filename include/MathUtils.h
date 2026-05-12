#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <cmath>
#include <vector>

namespace Math {
    struct Vec2 { float x, y; };
    struct Vec3 { float x, y, z; };
    
    // Simple 4x4 matrix for 2D ortho projection and transformations
    struct Mat4 {
        float m[16] = {0};
        
        static Mat4 Identity() {
            Mat4 res;
            res.m[0] = res.m[5] = res.m[10] = res.m[15] = 1.0f;
            return res;
        }
        
        static Mat4 Ortho(float left, float right, float bottom, float top, float near, float far) {
            Mat4 res = Identity();
            res.m[0] = 2.0f / (right - left);
            res.m[5] = 2.0f / (top - bottom);
            res.m[10] = -2.0f / (far - near);
            res.m[12] = -(right + left) / (right - left);
            res.m[13] = -(top + bottom) / (top - bottom);
            res.m[14] = -(far + near) / (far - near);
            return res;
        }
        
        static Mat4 Translation(float x, float y) {
            Mat4 res = Identity();
            res.m[12] = x;
            res.m[13] = y;
            return res;
        }
    };
}

#endif
