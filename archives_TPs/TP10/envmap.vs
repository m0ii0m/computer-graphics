#version 330 core
in vec3 a_position;
in vec3 a_normal;

out vec3 v_Normal;
out vec3 v_FragPos;

uniform mat4 u_worldMatrix;

layout(std140) uniform ViewProj {
    mat4 u_viewMatrix;
    mat4 u_projectionMatrix;
};

void main(void) {
    gl_Position = u_projectionMatrix * u_viewMatrix * u_worldMatrix * vec4(a_position, 1.0);
    v_Normal = mat3(u_worldMatrix) * a_normal;
    v_FragPos = vec3(u_worldMatrix * vec4(a_position, 1.0));
}
