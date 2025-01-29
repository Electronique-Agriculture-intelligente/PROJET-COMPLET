# NPK Soil Analyzer Mobile App
## Application Mobile pour l'Agriculture Intelligente - M1-GI Polytechnique Yaoundé

Cette application mobile fait partie du projet d'agriculture intelligente développé dans le cadre du cours d'électronique de M1-GI à l'École Nationale Supérieure Polytechnique de Yaoundé.

### Description
L'application NPK Soil Analyzer permet aux utilisateurs d'envoyer leur position GPS via SMS à un système d'analyse de sol NPK. Elle sert d'interface utilisateur mobile pour le système de capteurs NPK, permettant de déclencher des analyses de sol à distance.

### Fonctionnalités
- Récupération automatique des coordonnées GPS
- Envoi de SMS avec les coordonnées au format prédéfini
- Interface utilisateur intuitive avec indicateur de chargement
- Affichage des coordonnées actuelles avec leur précision
- Support multiplateforme (Android/iOS)
- Design moderne avec dégradé de couleurs

### Prérequis
- Node.js (v14 ou supérieur)
- npm ou yarn
- Expo CLI
- Android Studio (pour le développement Android)
- Xcode (pour le développement iOS, Mac uniquement)

### Technologies Utilisées
- React Native
- Expo
- expo-location
- expo-sms
- expo-linear-gradient
- @expo/vector-icons

### Installation
1. Clonez le dépôt :
```bash
git clone <url-du-depot>
cd npk-soil-analyzer-app
```

2. Installez les dépendances :
```bash
npm install
# ou
yarn install
```

3. Configurez le numéro de téléphone du microcontrôleur :
```javascript
// Dans App.js
const PHONE_NUMBER = "+237XXXXXXXXX"; // Remplacez par votre numéro
```

4. Lancez l'application :
```bash
expo start
```

### Configuration Requise sur le Téléphone
- Autorisation de localisation
- Autorisation d'envoi de SMS
- GPS activé
- Connection Internet pour la première installation

### Utilisation
1. Lancez l'application
2. Accordez les permissions de localisation demandées
3. Appuyez sur le bouton "Analyser le sol"
4. L'application va :
   - Récupérer votre position GPS
   - Envoyer un SMS au format `predict <latitude> <longitude>`
   - Afficher la position actuelle et sa précision

### Structure du Code
```
npk-soil-analyzer-app/
├── App.js                 # Point d'entrée principal
├── app.json              # Configuration Expo
├── assets/               # Images et ressources
├── package.json          # Dépendances
└── README.md             # Documentation
```

### Gestion des Erreurs
L'application gère plusieurs scénarios d'erreur :
- Refus des permissions de localisation
- Échec de récupération de la position GPS
- Échec d'envoi de SMS
- Problèmes de compatibilité des appareils

### Style et Design
L'application utilise :
- Un dégradé de bleu pour l'arrière-plan
- Des icônes Material Community Icons
- Une interface utilisateur responsive
- Des indicateurs visuels de chargement

### Développement
#### Scripts Disponibles
```bash
npm start        # Démarre le serveur de développement
npm run android  # Lance l'application sur Android
npm run ios      # Lance l'application sur iOS
npm run web      # Lance la version web
```

### Contribution
1. Fork le projet
2. Créez votre branche (`git checkout -b feature/NouvelleFonctionnalite`)
3. Committez vos changements (`git commit -m 'Ajout d'une nouvelle fonctionnalité'`)
4. Push vers la branche (`git push origin feature/NouvelleFonctionnalite`)
5. Créez une Pull Request

### Auteurs
Équipe de M1-GI
École Nationale Supérieure Polytechnique de Yaoundé
Département Génie Informatique

### Licence
Ce projet est sous licence MIT. Voir le fichier `LICENSE` pour plus de détails.

### Contact
Pour toute question ou suggestion, veuillez ouvrir une issue sur le dépôt GitHub.