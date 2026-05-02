#version 120
varying vec3 v_Normal;
varying vec2 v_UV;

struct Light {
    vec3 direction;
    vec3 diffuseColor;
};
uniform Light u_light;
uniform sampler2D u_diffuseTexture;

vec3 diffuse(vec3 N, vec3 L) {
    float nDotL = max(dot(N, L), 0.0);
    return nDotL * u_light.diffuseColor;
}

void main(void) {
    vec3 N = normalize(v_Normal); 
    vec3 L = normalize(u_light.direction);
    
    vec3 diffuseRes = diffuse(N, L);
    vec4 texColor = texture2D(u_diffuseTexture, v_UV);
    
    gl_FragColor = vec4(diffuseRes * texColor.rgb, texColor.a);
}