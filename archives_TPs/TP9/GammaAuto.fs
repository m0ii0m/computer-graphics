#version 120
varying vec3 v_Normal;
varying vec2 v_UV;
varying vec3 v_FragPos;

struct Light {
    vec3 position;
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    float constant;
    float linear;
    float quadratic;
};
uniform Light u_light;

struct Material {
    vec3 emissiveColor;
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
};
uniform Material u_material;

uniform vec3 u_viewPos;
uniform sampler2D u_diffuseTexture;

uniform vec3 u_skyColor;
uniform vec3 u_groundColor;

void main(void) {
    vec3 N = normalize(v_Normal); 
    vec3 lightVec = u_light.position - v_FragPos;
    float distance = length(lightVec);
    vec3 L = normalize(lightVec);
    vec3 V = normalize(u_viewPos - v_FragPos);
    vec3 H = normalize(L + V);

    float attenuation = 1.0 / (u_light.constant + u_light.linear * distance + u_light.quadratic * (distance * distance));

    // Lecture de la texture (OpenGL fait la linéarisation automatiquement grâce au format GL_SRGB8_ALPHA8)
    vec4 texColor = texture2D(u_diffuseTexture, v_UV);

    // Ambiance Hémisphérique
    float hemiFactor = N.y * 0.5 + 0.5;
    vec3 ambient = mix(u_groundColor, u_skyColor, hemiFactor) * u_material.ambientColor * u_light.ambientColor;

    float nDotL = max(dot(N, L), 0.0);
    vec3 diffuse = nDotL * u_light.diffuseColor * u_material.diffuseColor;

    vec3 specular = vec3(0.0);
    if (nDotL > 0.0) {
        float nDotH = max(dot(N, H), 0.0);
        float specFactor = pow(nDotH, u_material.shininess);
        specular = specFactor * u_light.specularColor * u_material.specularColor;
    }
    
    vec3 lighting = ambient + (diffuse + specular) * attenuation;
    vec3 result = u_material.emissiveColor + lighting * texColor.rgb;
    
    // Sortie (OpenGL fait la compression Gamma automatiquement grâce à glEnable(GL_FRAMEBUFFER_SRGB))
    gl_FragColor = vec4(result, texColor.a);
}
