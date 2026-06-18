#version 330 core
in vec3 v_Normal;
in vec2 v_UV;
in vec3 v_FragPos;

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
uniform samplerCube u_envmap; // Cubemap pour l'illumination indirecte

uniform vec3 u_skyColor;
uniform vec3 u_groundColor;

out vec4 FragColor;

void main(void) {
    vec3 N = normalize(v_Normal); 
    vec3 lightVec = u_light.position - v_FragPos;
    float distance = length(lightVec);
    vec3 L = normalize(lightVec);
    vec3 V = normalize(u_viewPos - v_FragPos);
    vec3 H = normalize(L + V);

    float attenuation = 1.0 / (u_light.constant + u_light.linear * distance + u_light.quadratic * (distance * distance));

    // Lecture de la texture
    vec4 texColor = texture(u_diffuseTexture, v_UV);


    // illumination indirecte

    // Diffuse indirecte
    vec3 diffuseIndirect = texture(u_envmap, N).rgb * u_material.diffuseColor;

    // Ambiance Hémisphérique
    float hemiFactor = N.y * 0.5 + 0.5;
    vec3 hemiAmbient = mix(u_groundColor, u_skyColor, hemiFactor) * u_material.ambientColor;

    // Spéculaire indirecte : sampling de la cubemap avec le vecteur réflexion
    vec3 R = reflect(-V, N);
    vec3 specularIndirect = texture(u_envmap, R).rgb * u_material.specularColor;

    // Approximation de Fresnel
    float F0 = 0.04; // diélectrique standard
    float cosTheta = max(dot(N, V), 0.0);
    float fresnel = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);

    // Conservation d'énergie : kD + kS <= 1
    float kS = fresnel;
    float kD = 1.0 - kS;

    vec3 indirect = kD * (diffuseIndirect * 0.3 + hemiAmbient) + kS * specularIndirect * 0.3;


    // Illumination directe

    float nDotL = max(dot(N, L), 0.0);
    vec3 diffuseDirect = nDotL * u_light.diffuseColor * u_material.diffuseColor;

    vec3 specularDirect = vec3(0.0);
    if (nDotL > 0.0) {
        float nDotH = max(dot(N, H), 0.0);
        float specFactor = pow(nDotH, u_material.shininess);
        specularDirect = specFactor * u_light.specularColor * u_material.specularColor;
    }

    vec3 direct = (kD * diffuseDirect + kS * specularDirect) * attenuation;


    // Couleur finale

    // Couleur = Emissive + Indirect + Direct
    vec3 result = u_material.emissiveColor + (indirect + direct) * texColor.rgb;
    
    // Sortie (OpenGL fait la compression Gamma automatiquement grâce à glEnable(GL_FRAMEBUFFER_SRGB))
    FragColor = vec4(result, texColor.a);
}
