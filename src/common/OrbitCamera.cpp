#include "OrbitCamera.h"

// Caméra orbitale (TP à rendre)
float cam_phi = 0.0f;       // azimut (rotation horizontale)
float cam_theta = 0.3f;     // élévation (rotation verticale)
float cam_radius = 20.0f;   // distance à la cible
static double lastMouseX = 0.0, lastMouseY = 0.0;
static bool mousePressed = false;

void mouse_button_callback(GLFWwindow* /*win*/, int button, int action, int /*mods*/) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mousePressed = (action == GLFW_PRESS);
    }
}

void cursor_pos_callback(GLFWwindow* /*win*/, double xpos, double ypos) {
    if (mousePressed) {
        double dx = xpos - lastMouseX;
        double dy = ypos - lastMouseY;

        const float sensitivity = 0.005f;
        const float PI = 3.14159265f;
        cam_phi -= (float)dx * sensitivity;
        cam_theta += (float)dy * sensitivity;

        // Limiter les angles
        if (cam_phi > PI) cam_phi -= 2.0f * PI;
        if (cam_phi < -PI) cam_phi += 2.0f * PI;
        if (cam_theta > PI / 2.0f - 0.01f) cam_theta = PI / 2.0f - 0.01f;
        if (cam_theta < -PI / 2.0f + 0.01f) cam_theta = -PI / 2.0f + 0.01f;
    }
    lastMouseX = xpos;
    lastMouseY = ypos;
}

void scroll_callback(GLFWwindow* /*win*/, double /*xoffset*/, double yoffset) {
    cam_radius -= (float)yoffset * 2.0f;
    if (cam_radius < 1.0f) cam_radius = 1.0f;
    if (cam_radius > 500.0f) cam_radius = 500.0f;
}
