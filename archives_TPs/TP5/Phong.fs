#version 120
varying vec3 v_Normal;
varying vec2 v_UV;
varying vec3 v_FragPos;

struct Light {
    vec3 direction;
    vec3 diffuseColor;
    vec3 specularColor;
};
uniform Light u_light;

struct Material {
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
};
uniform Material u_material;

uniform vec3 u_viewPos;
uniform sampler2D u_diffuseTexture;

vec3 diffuse(vec3 N, vec3 L) {
    float nDotL = max(dot(N, L), 0.0);
    return nDotL * u_light.diffuseColor * u_material.diffuseColor;
}

vec3 specular(vec3 N, vec3 L, vec3 V) {
    vec3 R = reflect(-L, N);
    float specFactor = pow(max(dot(R, V), 0.0), u_material.shininess);
    return specFactor * u_light.specularColor * u_material.specularColor;
}

void main(void) {
    vec3 N = normalize(v_Normal); 
    vec3 L = normalize(u_light.direction);
    vec3 V = normalize(u_viewPos - v_FragPos);
    
    vec3 diff = diffuse(N, L);
    
    vec3 spec = vec3(0.0);
    if (dot(N, L) > 0.0) {
        spec = specular(N, L, V);
    }
    
    vec4 texColor = texture2D(u_diffuseTexture, v_UV);
    
    // Combinaison : (Diffuse + Specular) * Texture
    gl_FragColor = vec4((diff + spec) * texColor.rgb, texColor.a);
}
