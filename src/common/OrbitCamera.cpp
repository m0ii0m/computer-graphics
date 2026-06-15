#include "OrbitCamera.h"
#include <cmath>

void OrbitCamera::RegisterCallbacks(GLFWwindow* window) {
    glfwSetWindowUserPointer(window, this);

    glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
        auto* cam = static_cast<OrbitCamera*>(glfwGetWindowUserPointer(win));
        if (cam) cam->OnMouseButton(button, action, mods);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* win, double xpos, double ypos) {
        auto* cam = static_cast<OrbitCamera*>(glfwGetWindowUserPointer(win));
        if (cam) cam->OnCursorPos(xpos, ypos);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* win, double xoffset, double yoffset) {
        auto* cam = static_cast<OrbitCamera*>(glfwGetWindowUserPointer(win));
        if (cam) cam->OnScroll(xoffset, yoffset);
    });
}

void OrbitCamera::Update(GLFWwindow* window, float deltaTime) {
    (void)deltaTime;
    const float keySpeed = 0.03f;
    const float PI = 3.14159265f;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) m_Phi += keySpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) m_Phi -= keySpeed;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) m_Theta += keySpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) m_Theta -= keySpeed;
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) m_Radius -= 0.5f;
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) m_Radius += 0.5f;

    // Limiter les angles et le rayon
    if (m_Phi > PI) m_Phi -= 2.0f * PI;
    if (m_Phi < -PI) m_Phi += 2.0f * PI;
    if (m_Theta > PI / 2.0f - 0.01f) m_Theta = PI / 2.0f - 0.01f;
    if (m_Theta < -PI / 2.0f + 0.01f) m_Theta = -PI / 2.0f + 0.01f;
    if (m_Radius < 1.0f) m_Radius = 1.0f;
    if (m_Radius > 500.0f) m_Radius = 500.0f;

    // Sphériques -> Cartésiennes
    m_Position.y = m_Radius * std::sin(m_Theta);
    m_Position.x = m_Radius * std::cos(m_Theta) * std::cos(m_Phi);
    m_Position.z = m_Radius * std::cos(m_Theta) * std::sin(m_Phi);
}

Matrix4 OrbitCamera::GetViewMatrix() const {
    return LookAt(m_Position, m_Target, m_Up);
}

void OrbitCamera::OnMouseButton(int button, int action, int mods) {
    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        m_MousePressed = (action == GLFW_PRESS);
    }
}

void OrbitCamera::OnCursorPos(double xpos, double ypos) {
    if (m_MousePressed) {
        double dx = xpos - m_LastMouseX;
        double dy = ypos - m_LastMouseY;

        const float sensitivity = 0.005f;
        const float PI = 3.14159265f;
        m_Phi -= (float)dx * sensitivity;
        m_Theta += (float)dy * sensitivity;

        // Limiter les angles
        if (m_Phi > PI) m_Phi -= 2.0f * PI;
        if (m_Phi < -PI) m_Phi += 2.0f * PI;
        if (m_Theta > PI / 2.0f - 0.01f) m_Theta = PI / 2.0f - 0.01f;
        if (m_Theta < -PI / 2.0f + 0.01f) m_Theta = -PI / 2.0f + 0.01f;
    }
    m_LastMouseX = xpos;
    m_LastMouseY = ypos;
}

void OrbitCamera::OnScroll(double xoffset, double yoffset) {
    (void)xoffset;
    m_Radius -= (float)yoffset * 2.0f;
    if (m_Radius < 1.0f) m_Radius = 1.0f;
    if (m_Radius > 500.0f) m_Radius = 500.0f;
}
