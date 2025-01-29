/*
* codeNPK.ino
* ------------
* Code flashé dans l'ESP8266 pour la collecte de données NPK et transmission via HTTPS au serveur via un point d'API POST
*
* Liens utilisés :
*   https://how2electronics.com/measure-soil-nutrient-using-arduino-soil-npk-sensor/
*   https://randomnerdtutorials.com/esp8266-nodemcu-http-get-post-arduino/
*   https://randomnerdtutorials.com/esp32-http-get-post-arduino/
*   https://microdigisoft.com/send-and-receive-sms-using-esp32-ttgo-t1-call-with-arduino-ide/
*   https://www.developershome.com/sms/readSmsByAtCommands.asp
*
* Rédigé par l'équipe de stagiaires
*
* Créé le 26/08/2023
* Modifié le 15/02/2024
*/

// URL des requêtes HTTP POST et GET, à modifier en fonction de l'API qu'on a créé côté serveur
String basic = "http://172.20.10.2:8000/npkmeter/npkph/"; 
String prediction = "http://172.20.10.2:8000/npkmeter/npkph/prediction"; // URL de la prédiction

#define TEST_NPK
// Importation des bibliothèques nécessaires
#include <SoftwareSerial.h>
#include <Wire.h>

#if defined(ESP8266)
  #define MICROCONTROLLER_TYPE "ESP8266"
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
  WiFiServer server(80);

  // Broches pour interagir avec le MAX485 modbus (On peut modifier les valeurs)
  #define DE 13
  #define RE 15

  // On peut également modifier ces informations en fonction de nos besoins
  #define RX  4// Emulation de la broche qu'on veut pour la réception en série
  #define TX  0// Emulation de la broche qu'on veut pour la transmission en série

SoftwareSerial mod(RX, TX); // Initialisation d'une communication série avec le MAX485

  /*
  * Fonction : httpGETRequest
  * --------------------
  * Argument :
  *   - url (String), endpoint
  *
  * Fonction pour effectuer une requête HTTP GET vers le serveur
  *
  * Retourne sous format JSON (String) le résultat
  */
  String httpGETRequest(String url) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, url);

    int httpResponseCode = http.GET();

    String payload = "{}";

    if(httpResponseCode > 0) {
      Serial.print("HTTP Response Code: ");
      Serial.println(httpResponseCode);
      payload = http.getString();
    }
    else {
      Serial.print("Error Code: ");
      Serial.println(httpResponseCode);
    }

    http.end();

    return payload;
  }

  /*
  * Fonction : updateServer
  * --------------------
  * Fonction pour écouter sur le endpoint /post du serveur et effectuer une requête HTTP POST des 
  * valeurs lues au serveur sans avoir à les envoyer de façon constante
  *
  * Ne retourne rien
  */
  void updateServer() {
    WiFiClient client = server.available();
    if(!client) {
      #ifdef TEST_NPK
        Serial.print("N : ");
        Serial.println(nitrogen());
        Serial.print("P : ");
        Serial.println(potassium());
        Serial.print("K : ");
        Serial.println(phosphorous());
        Serial.println();
        delay(1000);
      #endif
      return;
    }

    while(client.connected()) {
      if(client.available()) {
        String request = client.readStringUntil('\r');
        Serial.println(request);

        if(request.indexOf("/post") != -1) {
          if(WiFi.status() == WL_CONNECTED) { // Vérification de l'état de connexion au point d'accès
            httpPOSTRequest(basic);
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/json");
            client.println("")
            client.println("{\"message\": \"Sent\"}");
            client.stop();
          }
        }
      }
    } 
  }

#elif defined(ESP32)
  // Tâches pour le multithreading
  TaskHandle_t SMS;
  TaskHandle_t Wifi;

  // Module utilisé et buffer
  #define TINY_GSM_MODEM_SIM800
  #define TINY_GSM_RX_BUFFER 1024
  #define MICROCONTROLLER_TYPE "ESP32"
  #include <WiFi.h>
  #include <ArduinoJson.h>
  #include <WebServer.h>
  #include <HTTPClient.h>
  #include <TinyGsmClient.h>
  // #include <WiFiClient.h>

  #define DE 19
  #define RE 25
  #define RX 26
  #define TX 18

  WebServer server(80);

  SoftwareSerial modbus(RX, TX);

  StaticJsonDocument<250> jsonDocument;
  char buffer[250];

  // Broches pour la communication avec le module SIM800 à travers l'ESP32
  #define MODEM_RST 5
  #define MODEM_PWKEY 4
  #define MODEM_POWER_ON 23
  #define MODEM_TX 27 
  #define MODEM_RX 26
  #define I2C_SDA 21
  #define I2C_SCL 22

  #define IP5306_ADDR 0x75
  #define IP5306_REG_SYS_CTL0 0x00

  #define SerialMon Serial
  #define SerialAT Serial1

  TinyGsm modem(SerialAT);
  String gsm;

  bool setPowerBoostKeepOn(int en) {
    Wire.beginTransmission(IP5306_ADDR);
    Wire.write(IP5306_REG_SYS_CTL0);
    if(en) {
      Wire.write(0x37);
    }
    else {
      Wire.write(0x35);
    }
    return Wire.endTransmission() == 0;
  }

  /*
  * Fonction : updateSerial
  * --------------------
  * Fonction pour lire les valeurs d'un moniteur série et l'écrire dans un autre moniteur série.
  * Notamment SerialAT et SerialMon
  *
  * Ne retourne rien
  */
  void updateSerial() {
    while(SerialMon.available()) {
      SerialAT.write(SerialMon.read());
    }

    while(SerialAT.available()) {
      SerialMon.write(SerialAT.read());
    }
  }

/*
* Fonction : httpGETRequest
* --------------------
* Argument :
*   - url (String), endpoint
*
* Fonction pour effectuer une requête HTTP GET vers le serveur
*
* Retourne sous format JSON (String) le résultat
*/
  String httpGETRequest(String url) {
  HTTPClient http;

  http.begin(url);

  int httpResponseCode = http.GET();

  String payload = "{}";

  if(httpResponseCode > 0) {
    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error Code: ");
    Serial.println(httpResponseCode);
  }

  http.end();

  return payload;
}

void handleReceivedSMS(void *pvParameters) {
  for(;;) {
    // Vérifier l'arrivée de nouveaux messages
    if(SerialAT.available()) {
      // Lecture de la notification
      gsm = SerialAT.readString(); 
      gsm.trim();
      gsm.toLowerCase();
      /* 
      On vérifie bien qu'il s'agit d'une notification de réception de SMS et on stocke l'index du message reçu.
      Ex -> +CMTI: "SM", <index> où SM représente la mémoire de stockage
      */
      if(gsm.startsWith("+CMTI:")) {
        int comma = gsm.indexOf(',');
        String index = gsm.substring(comma + 1);
        index.trim();
        // Lecture du SMS reçu via son index
        SerialAT.println("AT+CMGR=" + index);
        delay(200); // Temps d'exécution de la requête
        /*
        Vérification de lecture du message
        */
        if(SerialAT.available()) {
          gsm = SerialAT.readString();
          gsm.trim();
          /*
          Si c'est le message qui est lu, on récupère le numéro du destinataire
          et on s'assure que ça commence par "get" ou "predict" pour envoyer
          des requêtes HTTP
          */
          if(gsm.startsWith("+CMGR:")) {
            int firstComma = gsm.indexOf(',');
            int secondComma = gsm.indexOf(',', firstComma + 1);
            int thirdComma = gsm.indexOf(',', secondComma + 1);
            String number = gsm.substring(firstComma + 2, secondComma - 1);

            int ash = gsm.lastIndexOf('"');
            // Récupération du contenu du SMS sachant que le retour à la ligne est un caractère
            String message = gsm.substring(ash + 2);
            message.trim();
            if(message.startsWith("get")) {
              int res = httpPOSTRequest(basic);
              String response = httpGETRequest(basic);
              if(modem.sendSMS(number, response)) {
                SerialMon.println(response);
              }
              else {
                SerialMon.println("Echec d'envoi du SMS");
              }
            }
            if(message.startsWith("predict")) {
              int res = httpPOSTRequest(basic);
              String response = httpGETRequest(prediction);
              if(modem.sendSMS(number, response)) {
                SerialMon.println(response);
              }
              else {
                SerialMon.println("Echec d'envoi du SMS");
              }
            }
            // Suppression du SMS via son index
            SerialAT.println("AT+CMGD=" + index);
            delay(200);
          }
        }
      }
    }
  }
}

void create_json() {  
  jsonDocument.clear();  
  jsonDocument["message"] = "Ok";
  serializeJson(jsonDocument, buffer);
}

void postData() {
  int res = httpPOSTRequest(basic);
  create_json();
  if(res == 200) {
    server.send(200, "application/json", buffer);
  } else {
    server.send(404, "application/json", buffer);
  }
}

void updateServer(void *pvParameters) {
  server.on("/post", postData);
  server.begin();
  for(;;) {
    server.handleClient();
  }
}

#else
  #error "Type de microcontrôleur non pris en charge"
#endif

// Adresses de l'azote, du  phosphore et du potassium sur le MAX485
const byte nitro_inquiry_frame[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos_inquiry_frame[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota_inquiry_frame[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

byte values[11];

// Informations à modifier si on utilise un autre réseau WiFi
const char* ssid = "OH MY FOX !! 🦊🙆‍♂️"; // Nom du point d'accès WiFi
const char* password = "01234567895"; // Mot de passe du point d'accès WiFi

void setup() {
  #if defined(ESP8266)

    pinMode(DE, OUTPUT); // Configuration de la broche DE en sortie
    pinMode(RE, OUTPUT); // Configuration de la broche RE en sortie
    Serial.begin(115200); // Démarrage de la communication en série avec la vitesse de 115200 pour la connexion au WiFi
    SerialMon.print("Carte ESP8266");
    mod.begin(9600); // Initialisation pour la communication en série avec le MAX485 modbus
    Serial.println("Modbus initialisé.");
    WiFi.begin(ssid, password); // Initialisation de la connexion au point d'accès WiFi

    Serial.print("Connexion");
    while(WiFi.status() != WL_CONNECTED) { // Vérification de la connexion au point d'accès
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connecté au réseau WiFi avec l'adresse IP : ");
    Serial.println(WiFi.localIP());

    server.begin();

    Serial.println("Serveur HTTP démarré.");
  #elif defined(ESP32)
    SerialMon.begin(115200);

    modbus.begin(9600);
    SerialMon.println("Carte ESP32");
    pinMode(RE, OUTPUT); // Configuration de la broche RE en sortie
    pinMode(DE, OUTPUT); // Configuration de la broche DE en sortie
    SerialMon.println("Modbus initialisé.");

    Wire.begin(I2C_SDA, I2C_SCL);
    bool isOk = setPowerBoostKeepOn(1);
    SerialMon.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

    // Set modem reset, enable, power pins
    pinMode(MODEM_PWKEY, OUTPUT);
    pinMode(MODEM_RST, OUTPUT);
    pinMode(MODEM_POWER_ON, OUTPUT);
    digitalWrite(MODEM_PWKEY, LOW);
    digitalWrite(MODEM_RST, HIGH);
    digitalWrite(MODEM_POWER_ON, HIGH);

    // Set GSM module baud rate and UART pins
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    delay(3000);

    SerialMon.println("Initialisation du modem...");
    modem.init();

    SerialAT.println("AT"); // Handshake réussi
    updateSerial();
    delay(200);
    SerialAT.println("AT+CMGF=1"); // Configurer le mode texte
    updateSerial();
    delay(200);
    SerialAT.println("AT+CNMI=2,1,0,0,0"); // Décider comment les nouveaux messages reçus devront être gérés. Dès réception, notification et stockage en mémoire
    updateSerial();

    WiFi.begin(ssid, password); // Initialisation de la connexion au point d'accès WiFi

    Serial.print("Connexion");
    while(WiFi.status() != WL_CONNECTED) { // Vérification de la connexion au point d'accès
      delay(500);
      SerialMon.print(".");
    }
    SerialMon.println("Connecté !");
    Serial.print("Connecté au réseau WiFi avec l'adresse IP : ");
    Serial.println(WiFi.localIP());

    xTaskCreatePinnedToCore(
      updateServer,
      "WiFi",
      10000,
      NULL,
      1, 
      &Wifi,
      0
    );
    delay(500);

    xTaskCreatePinnedToCore(
      handleReceivedSMS,
      "SMS",
      10000,
      NULL,
      1,
      &SMS,
      1
    );
    delay(500);
  #endif
}

void loop() {
  #if defined(ESP8266)
    updateServer();
  #elif defined(ESP32)

  #endif
}

//Fonction: retoune fichier.json
// void handlePostRequest(){
//   StaticJsonDocument<200> doc;
//   doc["message"] = "Ok";

//   String json;
//   serializeJson(doc, json);

//   server.send(200,"application/json", json);
// }

/*
* Fonction : nitrogen
* -------------------
* Fonction pour obtenir la valeur d'azote à partir du capteur NPK à travers le MAX485 modbus
*
* Retourne un entier de type byte
*/
byte nitrogen() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if(modbus.write(nitro_inquiry_frame, sizeof(nitro_inquiry_frame)) == 8) {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);

    for(byte i = 0; i < 7; i++) {
      values[i] = modbus.read();
      Serial.print(values[i], HEX);
    }
    Serial.println();
  }
  return values[4];
}

/*
* Fonction : phosphorous
* ----------------------
* Fonction pour obtenir la valeur de phosphore à partir du capteur NPK à travers le MAX485 modbus
*
* Retourne un entier de type byte
*/
byte phosphorous() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if(modbus.write(phos_inquiry_frame, sizeof(phos_inquiry_frame)) == 8) {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);

    for(byte i = 0; i < 7; i++) {
      values[i] = modbus.read();
      Serial.print(values[i], HEX);
    }
    Serial.println();
  }
  return values[4];
}

/*
* Fonction : potassium
* --------------------
* Fonction pour obtenir la valeur de potassium à partir du capteur NPK à travers le MAX485 modbus
*
* Retourne un entier de type byte
*/
byte potassium() {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  if(modbus.write(pota_inquiry_frame, sizeof(pota_inquiry_frame)) == 8) {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);

    for(byte i = 0; i < 7; i++) {
      values[i] = modbus.read();
      Serial.print(values[i], HEX);
    }
    Serial.println();
  }
  return values[4];
}

/*
* Fonction : httpPOSTRequest
* --------------------
* Argument :
*   - url (String), endpoint
*
* Fonction pour effectuer une requête HTTP POST vers le serveur
*
* Code de réponse
*/
int httpPOSTRequest(String url) {
  WiFiClient client;

  HTTPClient http;

  http.begin(client, url); // Initialisation de la communication HTTP

  http.addHeader("Content-Type", "application/json"); // On ajoute une entête pour spécifier que la requête se fera au format JSON
  String httpRequest = "{\"n\":\"" + String(nitrogen())
    + "\",\"p\":\"" + String(potassium()) + "\",\"k\":\"" + String(potassium())
    + "\",\"ph\":\"" + String(6) + "\"}";
  Serial.println(httpRequest);
  int httpResponseCode = http.POST(httpRequest); // Emission de la requête HTTPS POST

  Serial.print("HTTP Response Code : ");
  Serial.println(httpResponseCode);

  http.end();

  return httpResponseCode;
}