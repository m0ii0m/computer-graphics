#version 120
attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_uv;

varying vec3 v_Normal;
varying vec2 v_UV;
varying vec3 v_FragPos;

uniform mat4 u_worldMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

void main(void) {
    gl_Position = u_projectionMatrix * u_viewMatrix * u_worldMatrix * vec4(a_position, 1.0);

    // Normal Matrix = mat3(WorldMatrix) (valide quand pas de scale non-uniforme)
    v_Normal = mat3(u_worldMatrix) * a_normal;

    v_UV = a_uv;
    v_FragPos = vec3(u_worldMatrix * vec4(a_position, 1.0));
}
