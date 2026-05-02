# Projet OpenGL - Moteur de rendu 3D interactif
**Auteurs :** Ludovic REGNIER & Richard HO

## Description du projet
Ce projet, réalisé dans le cadre du TP OpenGL - Préparation au Projet, est l'aboutissement de notre apprentissage d'OpenGL moderne. Il rassemble dans une architecture unifiée la gestion matricielle (CPU), une caméra orbitale interactive, et un pipeline d'illumination avancé.

## Fonctionnalités implémentées

Nous avons rempli l'ensemble des critères obligatoires et optionnels demandés :

### 1. Caméra Orbitale (Arcball) et Pipeline Matriciel
*   **LookAt :** Implémentation manuelle de la fonction `LookAt` en C++ pour générer la *View Matrix* à partir des vecteurs Forward, Right et Up, via des produits vectoriels et scalaires.
*   **Contrôles interactifs :** La caméra navigue autour de l'objet via les coordonnées sphériques (azimut, élévation, rayon) contrôlables à la souris (clic gauche + mouvement / molette) et au clavier (flèches directionnelles).
*   **Architecture propre :** Les transformations sont calculées côté CPU. La *World Matrix*, la *View Matrix* et la *Projection Matrix* sont envoyées séparément au Vertex Shader.
*   **Normal Matrix :** Calculée de manière optimisée directement dans le Vertex Shader en extrayant la sous-matrice 3x3 de la *World Matrix* (`mat3(u_worldMatrix)`), évitant la distorsion des normales lors des translations.

### 2. Illumination Avancée (Blinn-Phong)
*   Implémentation du modèle de **Blinn-Phong** calculé par fragment (Phong shading).
*   **Ambiance Hémisphérique :** Remplacement de la couleur ambiante statique par une interpolation (mix) entre une couleur de "ciel" et une couleur de "sol", basée sur l'orientation de la normale par rapport à l'axe vertical (Y).

### 3. Pipeline Colorimétrique Linéaire (Correction Gamma)
*   **Linéarisation des textures (Input) :** Utilisation du format interne `GL_SRGB8_ALPHA8` lors de la création des textures via `stb_image`, déléguant au matériel la conversion sRGB vers Linéaire.
*   **Compression Gamma (Output) :** Activation matérielle via `glEnable(GL_FRAMEBUFFER_SRGB)` pour assurer une sortie correcte sur le moniteur après les calculs d'éclairage en espace linéaire.

## Instructions de compilation et d'exécution

*(Note : Projet testé sous Linux avec environnement X11/Wayland)*

**Dépendances requises :** `g++`, `freeglut`, `glew`, `glfw` (x11 ou wayland).

1. Ouvrez un terminal à la racine du projet.
2. Compilez et lancez le programme avec la commande :
   ```bash
   make run
   ```

(En cas d'erreur No GLX display sous environnement Wayland pur, la commande suivante peut être utilisée pour forcer XWayland : `DISPLAY=:1 make run` ou `export GLFW_PLATFORM=x11` avant le lancement).

## Contrôles

*   **Clic Gauche + Déplacement :** Tourner la caméra autour de la cible (Azimut / Élévation).
*   **Molette de la souris :** Zoomer / Dézoomer (Ajustement du rayon).
*   **Flèches Haut/Bas/Gauche/Droite :** Rotation alternative au clavier.
*   **Page Up / Page Down :** Zoom alternatif au clavier.
