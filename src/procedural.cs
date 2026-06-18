#version 430 core
layout(local_size_x = 16, local_size_y = 16) in;

// Texture de sortie (image 2D RGBA)
layout(rgba8, binding = 0) uniform image2D u_outputImage;

uniform float u_time;

// Bruit pseudo-aléatoire
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

// Bruit de valeur avec interpolation
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f); // smoothstep

    float a = hash(i + vec2(0.0, 0.0));
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// FBM effet de marbre
float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 5; i++) {
        value += amplitude * noise(p);
        p *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

void main(void) {
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 imgSize = imageSize(u_outputImage);

    if (texelCoord.x >= imgSize.x || texelCoord.y >= imgSize.y) return;

    vec2 uv = vec2(texelCoord) / vec2(imgSize);

    // Effet marbre
    float n = fbm(uv * 8.0);
    float marble = sin((uv.x + n) * 12.0) * 0.5 + 0.5;

    // couleurs type marbre
    vec3 color1 = vec3(0.95, 0.90, 0.82); // beige clair
    vec3 color2 = vec3(0.55, 0.35, 0.20); // brun
    vec3 color = mix(color1, color2, marble);

    // Veines plus sombres
    float vein = smoothstep(0.45, 0.5, marble) * smoothstep(0.55, 0.5, marble);
    color = mix(color, vec3(0.3, 0.2, 0.1), vein * 0.5);

    imageStore(u_outputImage, texelCoord, vec4(color, 1.0));
}
