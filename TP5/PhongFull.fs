#version 120
varying vec3 v_Normal;
varying vec2 v_UV;
varying vec3 v_FragPos;

struct Light {
    vec3 direction;
    vec3 ambientColor;  // Ia
    vec3 diffuseColor;  // Id
    vec3 specularColor; // Is
};
uniform Light u_light;

struct Material {
    vec3 emissiveColor; // Ke
    vec3 ambientColor;  // Ka
    vec3 diffuseColor;  // Kd
    vec3 specularColor; // Ks
    float shininess;
};
uniform Material u_material;

uniform vec3 u_viewPos;
uniform sampler2D u_diffuseTexture;

void main(void) {
    vec3 N = normalize(v_Normal); 
    vec3 L = normalize(u_light.direction);
    vec3 V = normalize(u_viewPos - v_FragPos);
    vec3 R = reflect(-L, N);

    vec3 emissive = u_material.emissiveColor;

    vec3 ambient = u_light.ambientColor * u_material.ambientColor;

    float nDotL = max(dot(N, L), 0.0);
    vec3 diffuse = nDotL * u_light.diffuseColor * u_material.diffuseColor;

    vec3 specular = vec3(0.0);
    if (nDotL > 0.0) {
        float specFactor = pow(max(dot(R, V), 0.0), u_material.shininess);
        specular = specFactor * u_light.specularColor * u_material.specularColor;
    }
    
    vec4 texColor = texture2D(u_diffuseTexture, v_UV);
    
    vec3 result = emissive + (ambient + diffuse + specular) * texColor.rgb;
    
    gl_FragColor = vec4(result, texColor.a);
}
