#version 120
varying vec3 v_Normal;

struct Light {
    vec3 direction;
    vec3 diffuseColor;
};
uniform Light u_light;

vec3 diffuse(vec3 N, vec3 L) {
    float nDotL = max(dot(N, L), 0.0);
    return nDotL * u_light.diffuseColor;
}

void main(void) {
    vec3 N = normalize(v_Normal); 
    vec3 L = normalize(u_light.direction);
    
    vec3 diffuseRes = diffuse(N, L);
    gl_FragColor = vec4(diffuseRes, 1.0);
}