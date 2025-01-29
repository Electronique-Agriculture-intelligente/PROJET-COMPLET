# Smart Agriculture NPK Project
## Projet d'Agriculture Intelligente - M1-GI Polytechnique Yaoundé

Ce projet a été développé dans le cadre du cours d'électronique de M1-GI à l'École Nationale Supérieure Polytechnique de Yaoundé, département Génie Informatique.

### Description
Ce système permet la collecte et l'analyse automatisée des données NPK (Azote, Phosphore, Potassium) du sol en utilisant des capteurs connectés. Les données sont transmises via HTTPS à un serveur central et peuvent également être consultées par SMS, permettant aux agriculteurs de surveiller la qualité de leur sol à distance.

### Caractéristiques
- Lecture des valeurs NPK du sol en temps réel
- Transmission des données via WiFi
- Interface HTTP pour la collecte des données
- Support des requêtes GET et POST
- Possibilité de consultation des données par SMS (version ESP32)
- Prédiction de la qualité du sol basée sur les données collectées

### Matériel Requis
- ESP8266 ou ESP32 (deux versions supportées)
- Capteur NPK avec interface Modbus
- Module MAX485 pour la communication avec le capteur
- Module SIM800 (pour la version ESP32 avec support SMS)
- Câbles de connexion
- Alimentation 5V

### Configuration des Broches
#### Version ESP8266
- DE: Pin 13
- RE: Pin 15
- RX: Pin 4
- TX: Pin 0

#### Version ESP32
- DE: Pin 19
- RE: Pin 25
- RX: Pin 26
- TX: Pin 18
- Pins supplémentaires pour le module SIM800:
  - MODEM_RST: Pin 5
  - MODEM_PWKEY: Pin 4
  - MODEM_POWER_ON: Pin 23
  - MODEM_TX: Pin 27
  - MODEM_RX: Pin 26

### Installation
1. Clonez ce dépôt
2. Ouvrez le fichier `codeNPK.ino` dans l'IDE Arduino
3. Installez les bibliothèques requises:
   - SoftwareSerial
   - ESP8266WiFi (pour ESP8266)
   - ESP8266HTTPClient (pour ESP8266)
   - WiFi (pour ESP32)
   - TinyGSM (pour ESP32)
   - ArduinoJson (pour ESP32)
4. Configurez les paramètres WiFi:
   ```cpp
   const char* ssid = "votre_ssid";
   const char* password = "votre_mot_de_passe";
   ```
5. Modifiez les URLs du serveur selon votre configuration:
   ```cpp
   String basic = "http://votre_serveur:port/npkmeter/npkph/";
   String prediction = "http://votre_serveur:port/npkmeter/npkph/prediction";
   ```

### Utilisation
#### Via HTTP
- Endpoint POST `/post`: Envoie les données NPK au serveur
- Endpoint GET: Récupère les dernières données enregistrées

#### Via SMS (ESP32 uniquement)
- Envoyez "get" pour recevoir les dernières mesures
- Envoyez "predict" pour recevoir une prédiction basée sur les données actuelles

### Structure du Projet
```
codeNPK/
├── codeNPK.ino           # Code principal
└── README.md             # Documentation
```

### Fonctionnalités
- `nitrogen()`: Lecture des valeurs d'azote
- `phosphorous()`: Lecture des valeurs de phosphore
- `potassium()`: Lecture des valeurs de potassium
- `httpPOSTRequest()`: Envoi des données au serveur
- `httpGETRequest()`: Récupération des données du serveur
- `handleReceivedSMS()`: Gestion des commandes SMS (ESP32)

### Contributions
Ce projet a été développé par l'équipe de stagiaires de M1-GI. Pour contribuer:
1. Fork le projet
2. Créez votre branche (`git checkout -b feature/AmazingFeature`)
3. Commit vos changements (`git commit -m 'Add some AmazingFeature'`)
4. Push vers la branche (`git push origin feature/AmazingFeature`)
5. Ouvrez une Pull Request




École Nationale Supérieure Polytechnique de Yaoundé
Département Génie Informatique
