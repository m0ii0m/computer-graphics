#version 120
varying vec3 v_Normal;
varying vec3 v_FragPos;

uniform vec3 u_viewPos;
uniform samplerCube u_cubemap;

void main(void) {
    vec3 N = normalize(v_Normal);
    vec3 I = normalize(v_FragPos - u_viewPos);

    // Réflexion spéculaire (chrome)
    vec3 R = reflect(I, N);
    vec3 reflectColor = textureCube(u_cubemap, R).rgb;

    // Réfraction (indice de l'air vers le verre ~ 1.0/1.52)
    vec3 refractDir = refract(I, N, 1.0 / 1.52);
    vec3 refractColor = textureCube(u_cubemap, refractDir).rgb;

    // Approximation de Fresnel (Schlick)
    float F0 = 0.04;
    float cosTheta = max(dot(-I, N), 0.0);
    float fresnel = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);

    vec3 result = mix(refractColor, reflectColor, fresnel);
    gl_FragColor = vec4(result, 1.0);
}
