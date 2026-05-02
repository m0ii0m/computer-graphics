#version 120
attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_uv;

varying vec3 v_Normal;
varying vec2 v_UV;

uniform mat4 u_transform;
uniform mat4 u_normalMatrix;

void main(void) {
    gl_Position = u_transform * vec4(a_position, 1.0);
    v_Normal = mat3(u_normalMatrix) * a_normal;
    v_UV = a_uv;
}