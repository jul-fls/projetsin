//-----Inclusion des bibliothèques-----
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <SPIFFS.h>
#include <EEPROM.h>
#include <NTPClient.h>
//------Initialisation des variables-----//
#define light_pin 2                                                                //Définition du pin gpio de l'éclairage
#define heat_pin 4                                                                 //Définition du pin gpio du chauffage
#define distrib_pin 26                                                             //Définition du pin gpio du distributeur
#define capteurTemperature_pin 34                                                  //Définition du pin gpio du capteur de température
bool auto_heat_state = false;                                                       //Déclaration de la variable booléenne qui définit l'auto chauffage par défaut sur l'état activé
int tempval;                                                                       //Déclaration de la variable entière de la température réelle du capteur de température en °C
int desired_temp;                                                                  //Déclaration de la variable entière de la température désirée de l'eau
#define EEPROM_SIZE 1                                                              //Définition de l'espace utlisée dans l'eeprom
#define DESIRED_TEMP 0                                                             //Définition de l'emplacement mémoire dans l'eeprom pour stocker la température désirée

AsyncWebServer server(80);                                                         //Création du serveur web Asynchrone sur le port 80
//-----Initialisation du client NTP (Partie 1 obligatoire hors du setup)-----//
WiFiUDP ntpUDP;                                                                    //Création d'une nouvelle instance (objet) ntpUDP de WifiUDP
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200);                         /*Création d'une nouvelle instance (objet) timeClient de NTPClient
                                                                                   et défintion du serveur ntp "europe.pool.ntp.org" ainsi que d'un décalage 
                                                                                   positif de 7200 secondes soit 2h pour UTC + 2*/
//-----Fonction distrib-----//
void distrib(){                                                                    /*Déclaration de la fonction distrib() qui est une fonction void (qui ne retourne pas de valeurs) 
                                                                                   et qui ne prend pas de parametres*/
  digitalWrite(distrib_pin, HIGH);                                                 //Ecriture d'un "1" logique sur le pin du distributeur
  delay(1000);                                                                     //Attente de 1000 ms soit 1s
  digitalWrite(distrib_pin, LOW);                                                  //Ecriture d'un "0" logique sur le pin du distributeur
  Serial.println("Distribution effectuée !");                                      //Ecriture dans le moniteur série de "Distribution effectuée !"
}
//-----Début de la fonction Setup-----//
void setup(){                                                                      //Déclaration de la fonction setup() qui est une fonction qui ne s'execute qu'une fois à l'allumage de l'esp32
  Serial.begin(115200);                                                            //Réglage de la vitesse de communication à 115200 bauds (bits/s)
  Serial.println("\n");                                                            //Affichage dans le moniteur série d'un retour à la ligne
  //-----Initialisation des GPIO-----//
  pinMode(light_pin, OUTPUT);                                                      //Définition du mode SORTIE pour le pin "light_pin"
  pinMode(distrib_pin, OUTPUT);                                                    //Définition du mode SORTIE pour le pin "distrib_pin"
  pinMode(heat_pin, OUTPUT);                                                       //Définition du mode SORTIE pour le pin "heat_pin"
  pinMode(capteurTemperature_pin, INPUT);                                          //Définition du mode ENTREE pour le pin "capteurTemperature_pin"
  digitalWrite(light_pin, LOW);                                                    //Ecriture d'un "0" logique sur le pin "light_pin" (eteindre l'éclairage)
  digitalWrite(distrib_pin, LOW);                                                  //Ecriture d'un "0" logique sur le pin "distrib_pin" (eteindre le distributeur)
  digitalWrite(heat_pin, LOW);                                                     //Ecriture d'un "0" logique sur le pin "heat_pin" (eteindre le chauffage)
  //-----Initialisation de SPIFFS-----//
  if (!SPIFFS.begin()){                                                            /*Essai de démarrage de SPIFFS (le gestionnaire de fichier de la mémoire FLASH), 
                                                                                   si le démarrage échoue, alors*/
    Serial.println("Erreur SPIFFS...");                                            //Affichage de "Erreur SPIFFS..." dans le moniteur série
    return;                                                                        //Retour de l'erreur                                                               
  }
  File root = SPIFFS.open("/");                                                    /*Création de l'objet "root" qui vient de "File", qui est égal au chemin d'accès 
                                                                                   de la racine de la mémoire FLASH (le chemin "/")*/                                              
  //-----Initialisation du Wifi-----//
  DNSServer dns;                                                                   //Création de l'objet "dns" qui vient de "DNSServer"
  AsyncWiFiManager wifiManager(&server,&dns);                                      /*Création de l'objet "wifiManager" qui vient de  "AsyncWiFiManager", 
                                                                                   avec les parametres "&server" et "&dns"*/
  const char* ssid = "ESP32-AP";                                                   /*Déclaration de la constante ssid de type "char" qui est donc un caractère, 
                                                                                   qui est un pointeur qui vaut "ESP32-AP"*/
  wifiManager.autoConnect(ssid);                                                   /*Tentative d'auto connection avec les identifiants enregsitrés dans la memoire FLASH,
                                                                                   si la connexion est impossible, l'esp32, génerera son propre réseau avec le ssid fourni 
                                                                                   et sans mot de passe*/
  Serial.println("");                                                              //Affichage d'un retour à la ligne dans le moniteur série
  Serial.println("Connexion etablie!");                                            //Affichage de "Connexion etablie!" dans le moniteur série
  Serial.print("Adresse IP: ");                                                    //Affichage de "Adresse IP: " dans le moniteur série
  Serial.println(WiFi.localIP());                                                  //Affichage de l'adresse IP de l'esp32 sur le réseau dans le moniteur série
  //-----Initialisation EEPROM-----//
  EEPROM.begin(EEPROM_SIZE);                                                       //Initialisation de l'esp32 avec la taille qui sera utilisé "EEPROM_SIZE" soit 1 octet
  //-----Récupération de la température stockée-----//
  desired_temp = (int) EEPROM.read(DESIRED_TEMP);                                  /*Affectation à desired_temp du résultat de la conversion du contenu de l'EEPROM 
                                                                                   à l'emplacement mémoire "DESIRED_TEMP" en nombre entier*/
  //-----Initialisation du client NTP (Partie 2 obligatoire dans le setup)-----//
  timeClient.begin();                                                              //Démarrage du client temporel timeClient
  //-----Serveur Web-----//
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {                    /*Traitement de la requete d'obtention de la page "/" 
                                                                                   soit la racine du serveur web*/
    request->send(SPIFFS, "/index.html", "text/html");                             //Envoi au client du document "/index.html"
  });

  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request) {              /*Traitement de la requete d'obtention de la page "/w3.css" 
                                                                                   qui est une librairie css responsive*/
    request->send(SPIFFS, "/w3.css", "text/css");                                  //Envoi au client du document "/w3.css"
  });

  server.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request) {           /*Traitement de la requete d'obtention de la page "/index.css" 
                                                                                   qui est mon propre fichier css avec quelques classes que j'ai créé moi même*/
    request->send(SPIFFS, "/index.css", "text/css");                               //Envoi au client du document "/index.css"
  });

  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request) {            /*Traitement de la requete d'obtention de la page "/index.js" 
                                                                                   qui est mon fichier js principal pour la récupération des données 
                                                                                   et les requetes en arriere plan*/
    request->send(SPIFFS, "/index.js", "text/javascript");                         //Envoi au client du document "/index.js"
  });

  server.on("/jquery-3.4.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request) { /*Traitement de la requete d'obtention de la page "/jquery-3.4.1.min.js" 
                                                                                   qui est une librairie js*/
    request->send(SPIFFS, "/jquery-3.4.1.min.js", "text/javascript");              //Envoi au client du document "/jquery-3.4.1.min.js"
  });

  server.on("/pictures/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {/*Traitement de la requete d'obtention de la page "/pictures/favicon.ico" 
                                                                                   qui est la petite icone de poisson dans l'onglet du navigateur internet*/
    request->send(SPIFFS, "/pictures/favicon.ico", "image/ico");                   //Envoi au client du document "/pictures/favicon.ico"
  });

  server.on("/read_actual_temp", HTTP_GET, [](AsyncWebServerRequest *request) {    //Traitement de la requete d'obtention de la page "/read_actual_temp"
    tempval = map(analogRead(capteurTemperature_pin),0,4095,0,40);                 /*Affection à la variable entière "tempval" du résultat du mappage 
                                                                                   du résultat de la conversion analogique numérique entre 0 et 4095 
                                                                                   de la température en nombre entre 0 et 40 (degrés celsius)*/
    String temperature = String(tempval);                                          /*Affectation à la variable de type chaine de caractères "température" 
                                                                                   la conversion de "tempval" d'un nombre entier en chaine de caractères*/
    request->send(200, "text/html", temperature);                                  //Envoi au client du contenu de la variable "temperature"
  });

  server.on("/read_desired_temp", HTTP_GET, [](AsyncWebServerRequest *request) {   //Traitement de la requete d'obtention de la page "/read_desired_temp"
    String desiredtemperature = String((desired_temp + 1));                        /*Affectation à la variable chaine de caractères "desiredtemperaure", 
                                                                                   le résultat de la conversion du contenu de la variable "desired_temp" + 1 
                                                                                   (soit un nombre entier), en chaine de caractères*/
    request->send(200, "text/html", desiredtemperature);                           //Envoi aui client du contenu de la variable "desiredtemperature"
  });

  server.on("/post_desired_temp", HTTP_POST, [](AsyncWebServerRequest *request) {  //Traitement de la requete de post de la page "/post_desired_temp"
    String message;                                                                //Déclaration de la variable message de type chaine de caractères
    if (request->hasParam("DesiredTempValue", true)){                              //Si la requete contient un paramètre "DesiredTempValue", alors
      message = request->getParam("DesiredTempValue", true)->value();              //Affectation à la variable "message" du contenu du parametre "DesiredTempValue" de la requete
      desired_temp = message.toInt();                                              //Affection à la variable entière "desired_temp" du résultat de la conversion de message en nombre entier
      EEPROM.write(DESIRED_TEMP,desired_temp);                                     //Ecriture dans l'EEPROM du contenu de la variable "desired_temp" à l'emplacement mémoire "DESIRED_TEMP"
      EEPROM.commit();                                                             //Appliquer les changements dans l'EEPROM
      Serial.println("");                                                          //Afficher un saut de ligne dans le moniteur série
      Serial.print("Changement de température désirée effectue à : ");             //Affcicher à la suite "Changement de température désirée effectue à : " dans le moniteur série
      Serial.print(desired_temp);                                                  //Afficher à la suite le contenu de la variable "desired_temp"
      Serial.print(" °C");                                                         //Afficher à la suite " °C" dans le moniteur série
    }
    request->send(204);                                                            //Envoi au client du code HTTP "204" signifiant "ok mais tu n'as pas de contenu à recevoir"
  });

  server.on("/lighton", HTTP_GET, [](AsyncWebServerRequest *request) {             //Traitement de la requete d'obtention de la page "/lighton"
    digitalWrite(light_pin, HIGH);                                                 //Ecriture d'un "1" logique sur le pin de l'éclairage (allumer l'éclairage)
    request->send(204);                                                            //Envoi au client du code HTTP "204" signifiant "ok mais tu n'as pas de contenu à recevoir"
    Serial.println("Lumière allumée !");                                           //Affichage de "Lumière allumée !" dans le monieur série
  });

  server.on("/lightoff", HTTP_GET, [](AsyncWebServerRequest *request) {            //Traitement de la requete d'obtention de la page "/lightoff"
    digitalWrite(light_pin, LOW);                                                  //Ecriture d'un "0" logique sur le pin de l'éclairage (éteindre l'éclairage)
    request->send(204);                                                            //Envoi au client du code HTTP "204" signifiant "ok mais tu n'as pas de contenu à recevoir"
    Serial.println("Lumière éteinte !");                                           //Affichage de "Lumière éteinte !" dans le monieur série
  });

  server.on("/heaton", HTTP_GET, [](AsyncWebServerRequest *request) {              //Traitement de la requete d'obtention de la page "/heaton"
    digitalWrite(heat_pin, HIGH);                                                  //Ecriture d'un "1" logique sur le pin du chauffage (allumer le chauffage de l'eau)
    request->send(204);                                                            //Envoi au client du code HTTP "204" signifiant "ok mais tu n'as pas de contenu à recevoir"
    Serial.println("Chauffage allumé !");                                          //Affichage de "Chauffage allumé !" dans le monieur série
  });

  server.on("/heatoff", HTTP_GET, [](AsyncWebServerRequest *request) {             //Traitement de la requete d'obtention de la page "/heatoff"
    digitalWrite(heat_pin, LOW);                                                   //Ecriture d'un "0" logique sur le pin du chauffage (éteindre le chauffage de l'eau)
    request->send(204);                                                            //Envoi au client du code HTTP "204" signifiant "ok mais tu n'as pas de contenu à recevoir"
    Serial.println("Chauffage éteint !");                                          //Affichage de "Chauffage éteint !" dans le monieur série
  });

  server.on("/autoheaton", HTTP_GET, [](AsyncWebServerRequest *request) {          //Traitement de la requete d'obtention de la page "/autoheaton"
    auto_heat_state = true;                                                        //Affectation à la variable booléenne "auto_heat_state" de la valeur "true" (activé)
    request->send(204);                                                            //Envoi au client du code HTTP "204" signifiant "ok mais tu n'as pas de contenu à recevoir"
    Serial.println("Auto régulation de la température activée !");                 //Affichage de "Auto régulation de la température activée !" dans le moniteur série
  });

  server.on("/autoheatoff", HTTP_GET, [](AsyncWebServerRequest *request) {         //Traitement de la requete d'obtention de la page "/autoheatoff"
    auto_heat_state = false;                                                       //Affectation à la variable booléenne "auto_heat_state" de la valeur "false" (désactivé)
    request->send(204);                                                            //Envoi au client du code HTTP "204" signifiant "ok mais tu n'as pas de contenu à recevoir"
    Serial.println("Auto régulation de la température désactivée !");              //Affichage de "Auto régulation de la température désactivée !" dans le moniteur série
  });

  server.on("/distrib", HTTP_GET, [](AsyncWebServerRequest *request) {             //Traitement de la requete d'obtention de la page "/distrib"
    Serial.println("Distribution manuelle demandée");                              //Affichage de "Distribution manuelle demandée" dans le moniteur série
    distrib();                                                                     //Appel de la fonction distrib()
    request->send(204);                                                            //Envoi au client du code HTTP "204" signifiant "ok mais tu n'as pas de contenu à recevoir"
  });

  server.begin();                                                                  //Démarrage du serveur web
  Serial.println("Serveur actif!");                                                //Affichage de "Serveur actif!" dans le moniteur série
}//-----Fin de la fonction setup-----//

void loop(){                                                                       //-----Début de la fonction Loop-----//
  if (auto_heat_state == true){                                                    //si la régulation de température est activée, alors
    if (tempval >= desired_temp){                                                  //si la température réelle est supérieure ou égale à la température désirée, alors 
      digitalWrite(heat_pin, LOW);                                                 //Ecire un "0" sur le pin du chauffage (éteindre le chauffage)
      }
    else if (tempval < desired_temp){                                              //sinon si la température réelle est inférieure à la température désirée, alors
      digitalWrite(heat_pin, HIGH);                                                //Ecire un "1" sur le pin du chauffage (allumer le chauffage)
      }
    }
  timeClient.update();                                                             //Mise à jour de l'heure
  String time = timeClient.getFormattedTime();                                     //Récupération de l'heure et affectation dans la variable time
  
  if(time == "09:30:00" || time == "21:30:00"){                                    //Si l'heure est égale à 09:30:00 ou à 21:30:00, alors
    distrib();                                                                     //Appel la fonction distrib
  }
}//-----Fin de la fonction Loop-----//
.
