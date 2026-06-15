#pragma once

#include <GLFW/glfw3.h>

// Caméra orbitale (TP à rendre)
extern float cam_phi;
extern float cam_theta;
extern float cam_radius;

void mouse_button_callback(GLFWwindow* win, int button, int action, int mods);
void cursor_pos_callback(GLFWwindow* win, double xpos, double ypos);
void scroll_callback(GLFWwindow* win, double xoffset, double yoffset);
