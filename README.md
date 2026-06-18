# Projet OpenGL - Moteur de rendu 3D interactif
**Auteurs :** Ludovic REGNIER & Richard HO

## Description du projet
Ce projet, réalisé dans le cadre du cours OpenGL ESIEE Paris en E4, est l'aboutissement de notre apprentissage d'OpenGL moderne. Il rassemble dans une architecture unifiée la gestion matricielle (CPU), une caméra orbitale interactive, et un pipeline de rendu avancé avec illumination directe et indirecte.

## Fonctionnalités implémentées

### Partie 1 : Affichage

#### 1.a - Chargement de modèles OBJ
*   Intégration de la bibliothèque **TinyOBJLoader** pour charger des fichiers OBJ avec matériaux (.mtl).
*   Reconstruction automatique des vertices OpenGL-compatibles (fusion des indices position/normal/texcoord).
*   Calcul automatique des normales lissées lorsqu'elles sont absentes du fichier.

#### 1.b - Illumination directe (Blinn-Phong)
*   Modèle **Blinn-Phong** calculé par fragment avec atténuation de la lumière.
*   Combinaison des textures et matériaux issus du fichier OBJ.

#### 1.c - Illumination indirecte
*   **Diffuse indirecte** : sampling de la cubemap avec la normale du fragment.
*   **Spéculaire indirecte** : sampling de la cubemap avec le vecteur de réflexion.
*   **Ambiance hémisphérique** : interpolation ciel/sol basée sur l'orientation de la normale.
*   **Approximation de Fresnel (Schlick)** pour conservation d'énergie (kD + kS ≤ 1).
*   L'équation finale suit : `DiffuseIndirect + SpeculaireIndirect + ForEach_Light(DiffuseDirect + SpeculaireDirect)`

#### 1.d - Rendu hors écran (FBO)
*   Rendu principal dans un **Framebuffer Object** (RGBA16F + Depth24).
*   Recopie vers le backbuffer via un quad plein écran et un shader de post-traitement.

### Partie 2 : Navigation

#### 2.a - Placement des objets
*   4 objets dans la scène à des positions, rotations et échelles différentes :
    - **Cube** : couleur rouge simple, rotation animée (Blinn-Phong)
    - **Stanford Bunny** : modèle OBJ avec matériaux (Blinn-Phong)
    - **Dragon** : environment mapping avec réflexion/réfraction (Fresnel)
    - **Sol** : plan texturé avec texture procédurale

#### 2.b - UBO
*   **Uniform Buffer Object** pour transférer les matrices projection + caméra à tous les shaders.

### Partie 3 : Options

#### 3.a - Post-traitement (FBO)
*   3 effets de post-traitement sélectionnables avec la touche **P** :
    - **Sépia**, **Luminance (grayscale)**, **Inversion des couleurs**

#### 3.c - Skybox (Cubemap)
*   Skybox rendue avec une cubemap (images de Pise).

#### 3.d - Compute Shader (texture procédurale)
*   Génération d'une texture **marbre** procédurale via un Compute Shader (FBM noise).

#### 3.f & 3.g - Fresnel (Schlick)
*   Implémentation de l'approximation de Fresnel-Schlick dans les shaders d'environment mapping et de Blinn-Phong pour balancer correctement diffuse et spéculaire.

### Pipeline colorimétrique
*   **Linéarisation des textures** : format `GL_SRGB8_ALPHA8` pour la conversion sRGB → linéaire.
*   **Correction gamma** : effectuée dans le shader de post-traitement (`pow(color, 1/2.2)`).

## Instructions de compilation et d'exécution

**Dépendances requises :** `g++`, `glew`, `glfw`, OpenGL 4.3+ (pour le Compute Shader).

1. Ouvrez un terminal à la racine du projet.
2. Compilez et lancez :
   ```bash
   make run
   ```

(En cas d'erreur No GLX display sous Wayland : `DISPLAY=:1 make run`)

## Contrôles

*   **Clic Gauche + Déplacement :** Tourner la caméra (azimut / élévation).
*   **Molette :** Zoomer / Dézoomer.
*   **Flèches directionnelles :** Rotation au clavier.
*   **Page Up / Page Down :** Zoom au clavier.
*   **P :** Cycle des effets de post-traitement (Normal → Sépia → Luminance → Inversion).
