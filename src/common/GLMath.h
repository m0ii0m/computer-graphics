#pragma once

#include <cmath>

struct Vector2D {
    float u, v;
};

struct Vector3D {
    float x, y, z;
};

struct Vertex {
    Vector3D position;
    Vector3D normal;
    Vector2D uv;
};

inline Vector3D Subtract(const Vector3D& a, const Vector3D& b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

inline Vector3D Add(const Vector3D& a, const Vector3D& b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

inline bool ArePositionsEqual(const Vector3D& a, const Vector3D& b) {
    const float epsilon = 0.001f;
    return std::abs(a.x - b.x) < epsilon &&
           std::abs(a.y - b.y) < epsilon &&
           std::abs(a.z - b.z) < epsilon;
}

inline Vector3D CrossProduct(const Vector3D& a, const Vector3D& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

inline Vector3D Normalize(const Vector3D& v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length > 0.0f) {
        return { v.x / length, v.y / length, v.z / length };
    }
    return v;
}

inline float DotProduct(const Vector3D& a, const Vector3D& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

struct Matrix4 {
    float m[16];

    Matrix4() {
        for (int i = 0; i < 16; ++i) m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f; // Identity
    }
};

inline Matrix4 Translate(float x, float y, float z) {
    Matrix4 mat;
    mat.m[12] = x;
    mat.m[13] = y;
    mat.m[14] = z;
    return mat;
}

inline Matrix4 RotateX(float angle) {
    Matrix4 mat;
    float c = std::cos(angle);
    float s = std::sin(angle);
    mat.m[5] = c;
    mat.m[6] = s;
    mat.m[9] = -s;
    mat.m[10] = c;
    return mat;
}

inline Matrix4 RotateY(float angle) {
    Matrix4 mat;
    float c = std::cos(angle);
    float s = std::sin(angle);
    mat.m[0] = c;
    mat.m[2] = -s;
    mat.m[8] = s;
    mat.m[10] = c;
    return mat;
}

inline Matrix4 RotateZ(float angle) {
    Matrix4 mat;
    float c = std::cos(angle);
    float s = std::sin(angle);
    mat.m[0] = c;
    mat.m[1] = s;
    mat.m[4] = -s;
    mat.m[5] = c;
    return mat;
}

inline Matrix4 Scale(float sx, float sy, float sz) {
    Matrix4 mat;
    mat.m[0] = sx;
    mat.m[5] = sy;
    mat.m[10] = sz;
    return mat;
}

inline Matrix4 Multiply(const Matrix4& a, const Matrix4& b) {
    Matrix4 res;
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            res.m[i * 4 + j] = a.m[0 * 4 + j] * b.m[i * 4 + 0] +
                               a.m[1 * 4 + j] * b.m[i * 4 + 1] +
                               a.m[2 * 4 + j] * b.m[i * 4 + 2] +
                               a.m[3 * 4 + j] * b.m[i * 4 + 3];
        }
    }
    return res;
}

inline Matrix4 Perspective(float fovY, float aspect, float near, float far) {
    Matrix4 mat;
    for (int i = 0; i < 16; ++i) mat.m[i] = 0.0f;
    float cotan = 1.0f / std::tan(fovY / 2.0f);
    mat.m[0] = cotan / aspect;
    mat.m[5] = cotan;
    mat.m[10] = -(far + near) / (far - near);
    mat.m[11] = -1.0f;
    mat.m[14] = -(2.0f * far * near) / (far - near);
    return mat;
}

inline Matrix4 LookAt(const Vector3D& position, const Vector3D& target, const Vector3D& up) {
    // 1. Forward = -(target - position) normalisé
    Vector3D forward = Normalize(Subtract(position, target));

    // 2. Right = cross(up, forward)
    Vector3D right = Normalize(CrossProduct(up, forward));

    // 3. Up corrigé = cross(forward, right)
    Vector3D correctedUp = CrossProduct(forward, right);

    // 4. Produits scalaires pour la translation inverse
    float tx = -DotProduct(right, position);
    float ty = -DotProduct(correctedUp, position);
    float tz = -DotProduct(forward, position);

    // 5. Assemblage de la matrice
    Matrix4 mat;
    mat.m[0] = right.x;       mat.m[4] = right.y;       mat.m[8]  = right.z;       mat.m[12] = tx;
    mat.m[1] = correctedUp.x; mat.m[5] = correctedUp.y; mat.m[9]  = correctedUp.z; mat.m[13] = ty;
    mat.m[2] = forward.x;     mat.m[6] = forward.y;     mat.m[10] = forward.z;     mat.m[14] = tz;
    mat.m[3] = 0.0f;          mat.m[7] = 0.0f;          mat.m[11] = 0.0f;          mat.m[15] = 1.0f;
    return mat;
}
