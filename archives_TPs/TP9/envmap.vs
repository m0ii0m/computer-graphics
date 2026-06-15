#version 120
attribute vec3 a_position;
attribute vec3 a_normal;

varying vec3 v_Normal;
varying vec3 v_FragPos;

uniform mat4 u_worldMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

void main(void) {
    gl_Position = u_projectionMatrix * u_viewMatrix * u_worldMatrix * vec4(a_position, 1.0);
    v_Normal = mat3(u_worldMatrix) * a_normal;
    v_FragPos = vec3(u_worldMatrix * vec4(a_position, 1.0));
}
