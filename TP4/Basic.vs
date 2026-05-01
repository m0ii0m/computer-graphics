attribute vec3 a_position;
varying vec4 v_color;

uniform mat4 u_transform;

void main(void) {
    gl_Position = u_transform * vec4(a_position, 1.0);
    // Convert position from [-1, 1] to [0, 1] for colors
    v_color = vec4(a_position * 0.5 + 0.5, 1.0);
}