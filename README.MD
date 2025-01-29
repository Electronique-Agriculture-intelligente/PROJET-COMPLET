# Projet d'Agriculture Intelligente

## Description du Projet

L'objectif de ce projet est d'optimiser l'utilisation des technologies intelligentes dans le domaine agricole. Il s'agit d'une continuation d'un projet initial développé dans le cadre de l'UE "Électronique et Interfaçage". Le système permet de mesurer les constituants essentiels du sol (azote, phosphore, potassium) et de prédire les cultures appropriées en fonction des données relevées.

### Limitations du Projet Initial
L'analyse du système existant a révélé plusieurs limitations techniques majeures :
1. Dysfonctionnement du montage électronique avec des lectures incorrectes des constituants du sol.
2. Utilisation de coordonnées GPS statiques limitant l'accès aux données météorologiques en temps réel.
3. Absence de mesures du pH et de la température du sol.

### Objectifs du Projet Amélioré
1. Correction et optimisation du montage électronique pour assurer des mesures fiables.
2. Intégration d'un module GPS et d'une application mobile pour une acquisition flexible des coordonnées géographiques.
3. Extension des capacités prédictives du système en intégrant des estimations de rendement.

## Structure du Dépôt GitHub

Le dépôt contient plusieurs dossiers et fichiers organisés comme suit :

```
|-- CODE PROJET          # Contient le code backend et frontend
|-- codeNPK              # Code flashé dans l'ESP32 pour la collecte des données NPK
|-- DATASETS ET NOTEBOOKS # Contient les ensembles de données et les notebooks d'analyse
|-- npkApp               # Application mobile pour l'acquisition et l'envoi des données GPS
|-- .gitignore           # Fichier de configuration Git
|-- README.md            # Documentation du projet
```

---

## Détails des Dossiers

### 1. CODE PROJET (Application Web Backend et Frontend)
Le dossier contient l'application web développée en Django pour le backend et ReactJS pour le frontend.

#### Installation Backend (Django)
```bash
cd CODE PROJET/Back
pip install -r requirements.txt
python manage.py runserver 0.0.0.0:8000
```

#### Installation Frontend (ReactJS)
```bash
cd CODE PROJET/Front
npm install
npm start
```

---

### 2. codeNPK (Code ESP32 pour la Mesure du Sol)
Ce dossier contient le firmware chargé sur l'ESP32 permettant la collecte des valeurs NPK et leur transmission.

#### Installation et Configuration
- Assurez-vous d'avoir installé les bibliothèques nécessaires sur l'IDE Arduino.
- Configurez les paramètres WiFi et les URLs du serveur dans le fichier `codeNPK.ino`.

```cpp
const char* ssid = "votre_ssid";
const char* password = "votre_mot_de_passe";
String basic = "http://votre_serveur:port/npkmeter/npkph/";
```

---

### 3. DATASETS ET NOTEBOOKS
Ce dossier contient les données utilisées pour entraîner le modèle de prédiction des cultures et leur rendement potentiel.

#### Contenu
- Jeux de données sur les propriétés du sol.
- Notebooks Jupyter pour l'analyse et la modélisation.

---

### 4. npkApp (Application Mobile)
Application React Native permettant l'envoi des coordonnées GPS pour l'analyse du sol.

#### Installation
```bash
git clone <url-du-depot>
cd npkApp
npm install
expo start
```

#### Fonctionnalités
- Récupération automatique de la position GPS.
- Envoi de SMS au microcontrôleur ESP32 pour déclencher les analyses.
- Interface intuitive et responsive.

---

## Contributions
Ce projet est open-source et toute contribution est la bienvenue !
1. Fork le projet
2. Créez une branche (`git checkout -b feature/NouvelleFonctionnalite`)
3. Committez vos modifications (`git commit -m 'Ajout d'une nouvelle fonctionnalité'`)
4. Poussez la branche (`git push origin feature/NouvelleFonctionnalite`)
5. Ouvrez une Pull Request

## Licence
Ce projet est sous licence MIT. Voir `LICENSE` pour plus de détails.

---

**École Nationale Supérieure Polytechnique de Yaoundé - Département Génie Informatique**

