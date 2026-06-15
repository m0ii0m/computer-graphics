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

Vector3D Subtract(const Vector3D& a, const Vector3D& b);
Vector3D Add(const Vector3D& a, const Vector3D& b);
bool ArePositionsEqual(const Vector3D& a, const Vector3D& b);
Vector3D CrossProduct(const Vector3D& a, const Vector3D& b);
Vector3D Normalize(const Vector3D& v);
float DotProduct(const Vector3D& a, const Vector3D& b);

struct Matrix4 {
    float m[16];

    Matrix4() {
        for (int i = 0; i < 16; ++i) m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f; // Identity
    }
};

Matrix4 Translate(float x, float y, float z);
Matrix4 RotateX(float angle);
Matrix4 RotateY(float angle);
Matrix4 RotateZ(float angle);
Matrix4 Scale(float sx, float sy, float sz);
Matrix4 Multiply(const Matrix4& a, const Matrix4& b);
Matrix4 Perspective(float fovY, float aspect, float near, float far);

// Ex2.1
Matrix4 LookAt(const Vector3D& position, const Vector3D& target, const Vector3D& up);
