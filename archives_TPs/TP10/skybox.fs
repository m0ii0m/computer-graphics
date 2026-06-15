#version 330 core
in vec3 v_TexCoords;

uniform samplerCube u_cubemap;

out vec4 FragColor;

void main(void) {
    FragColor = texture(u_cubemap, v_TexCoords);
}
