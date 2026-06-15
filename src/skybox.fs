#version 120
varying vec3 v_TexCoords;

uniform samplerCube u_cubemap;

void main(void) {
    gl_FragColor = textureCube(u_cubemap, v_TexCoords);
}
