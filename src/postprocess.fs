#version 330 core
in vec2 v_UV;

uniform sampler2D u_screenTexture;
uniform int u_effect; // 0 = Normal, 1 = Sépia, 2 = Luminance, 3 = Inversion

out vec4 FragColor;

void main(void) {
    vec3 color = texture(u_screenTexture, v_UV).rgb;

    if (u_effect == 1) {
        // Sépia
        vec3 sepia = vec3(
            dot(color, vec3(0.393, 0.769, 0.189)),
            dot(color, vec3(0.349, 0.686, 0.168)),
            dot(color, vec3(0.272, 0.534, 0.131))
        );
        color = sepia;
    }
    else if (u_effect == 2) {
        // Luminance (grayscale perceptuel)
        const vec3 luminanceWeight = vec3(0.2125, 0.7154, 0.0721);
        float lum = dot(color, luminanceWeight);
        color = vec3(lum);
    }
    else if (u_effect == 3) {
        // Inversion des couleurs
        color = vec3(1.0) - color;
    }

    // Correction gamma manuelle
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
