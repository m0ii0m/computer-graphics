#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GLMath.h"

class OrbitCamera {
private:
    float m_Phi{ 0.0f };       // azimut (rotation horizontale)
    float m_Theta{ 0.3f };     // élévation (rotation verticale)
    float m_Radius{ 20.0f };   // distance à la cible
    double m_LastMouseX{ 0.0 };
    double m_LastMouseY{ 0.0 };
    bool m_MousePressed{ false };

    Vector3D m_Target{ 0.0f, 0.0f, 0.0f };
    Vector3D m_Up{ 0.0f, 1.0f, 0.0f };
    Vector3D m_Position{ 0.0f, 0.0f, 0.0f };

public:
    OrbitCamera() = default;

    void RegisterCallbacks(GLFWwindow* window);
    void Update(GLFWwindow* window, float deltaTime = 0.0f);

    Matrix4 GetViewMatrix() const;
    Vector3D GetPosition() const { return m_Position; }

    void SetRadius(float r) { m_Radius = r; }
    float GetRadius() const { return m_Radius; }

    // Internal input callback handlers
    void OnMouseButton(int button, int action, int mods);
    void OnCursorPos(double xpos, double ypos);
    void OnScroll(double xoffset, double yoffset);
};
