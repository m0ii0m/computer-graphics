#version 120
attribute vec3 a_position;
attribute vec3 a_normal;
varying vec3 v_Normal;

uniform mat4 u_transform;
uniform mat4 u_normalMatrix;

void main(void) {
    gl_Position = u_transform * vec4(a_position, 1.0);
    v_Normal = mat3(u_normalMatrix) * a_normal;
}