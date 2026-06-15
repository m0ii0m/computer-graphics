#version 330 core
in vec3 a_position;

out vec3 v_TexCoords;

layout(std140) uniform ViewProj {
    mat4 u_viewMatrix;
    mat4 u_projectionMatrix;
};

void main(void) {
    v_TexCoords = a_position;
    // Supprimer la translation de la view matrix
    mat4 viewNoTranslation = mat4(mat3(u_viewMatrix));
    vec4 pos = u_projectionMatrix * viewNoTranslation * vec4(a_position, 1.0);
    // z = w pour que la skybox soit toujours au fond (depth = 1.0)
    gl_Position = pos.xyww;
}
