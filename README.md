Parfait 👍
Voici un **README propre et complet** qui fusionne ce qu’il y avait dans ton repo GitHub (`README.md`) et la présentation plus détaillée de ton ancien site.

---

# Projet SIN – Aquarium Connecté (Bac STI2D)

**STI2D** : Sciences et Technologies de l’Industrie et du Développement Durable
**SIN** : Système d’Information et Numérique

Ce projet a été réalisé dans le cadre du **Bac STI2D spécialité SIN**.
Il consistait à concevoir un **aquarium connecté** permettant à un aquariophile de **monitorer** et **automatiser** ses aquariums via une interface web, le tout piloté par une carte **ESP32**.

---

## 🎯 Objectifs

* Créer une solution permettant :

  * d’**automatiser** certaines actions (distribution de nourriture, éclairage, chauffage, régulation de température, etc.) ;
  * de **monitorer** l’état des aquariums (température, historique, statut des actions).
* Fournir une **IHM (Interface Homme-Machine)** accessible depuis un navigateur web (PC ou smartphone).
* Héberger la page web **directement sur l’ESP32**, avec gestion du Wi-Fi intégrée.

---

## 🛠️ Mon rôle dans le projet

Nous étions une équipe de **4 élèves**.
Ma responsabilité principale était de développer **l’IHM** (interface web et interaction avec l’ESP32).
Pour cela, j’ai travaillé à la fois sur la partie **C++ embarquée** (côté ESP32) et sur la partie **front-end web** (HTML, CSS, JavaScript).

Ce projet m’a permis de :

* renforcer mes connaissances en **C++ (Arduino/ESP32)** ;
* approfondir l’utilisation de **JavaScript** pour faire le lien entre l’interface web et l’ESP32 via des requêtes **HTTP GET/POST** ;
* pratiquer l’intégration front-end (HTML, CSS) sur une ressource embarquée limitée (SPIFFS).

---

## 📚 Librairies utilisées

* **ESPAsyncWebServer** → gestion du serveur web embarqué
* **ESPAsyncWiFiManager** → configuration et gestion de la connexion Wi-Fi
* **SPIFFS** → stockage et lecture de fichiers (pages web, scripts) dans la mémoire flash de l’ESP32
* **EEPROM** → stockage de paramètres persistants
* **NTPClient** → récupération de l’heure via un serveur NTP (`europe.pool.ntp.org`) pour les automatisations

---

## ⚙️ Fonctionnalités principales

* **Distribution automatique/manuelle de nourriture** (avec mise à jour de l’historique)
* **Gestion de l’éclairage** (allumage/extinction depuis la page web et synchronisation avec l’état réel)
* **Gestion du chauffage** :

  * allumer/éteindre le chauffage
  * définir une température cible
  * afficher la température courante
* **Régulation automatique de la température** (activation/désactivation depuis l’interface web)
* **Interface responsive** utilisable sur ordinateur et smartphone

---

## 🖼️ Illustrations

Quelques exemples d’écrans et de montages :

* **Distribution de nourriture** :
  ![Distribution](https://github.com/user-attachments/assets/8fc36a0e-8b65-442e-9da2-8bf77fbec077)


* **Éclairage ON (page & réel)** :
  ![Éclairage page](https://github.com/user-attachments/assets/3f704c05-8f20-4c75-b54e-61219e39aaf1)

  ![Éclairage réel](https://github.com/user-attachments/assets/7521bc66-85b7-4a56-be7a-6c63b6b4e1c3)


* **Chauffage ON (page & réel)** :
  ![Chauffage page](https://github.com/user-attachments/assets/1c0a84a7-1d19-40aa-a225-8fc285bf7169)

  ![Chauffage réel](https://github.com/user-attachments/assets/a69d5112-7ad6-42e1-a832-13730d3c355e)


* **Régulation automatique de température activée** :
  ![Régulation activée](https://github.com/user-attachments/assets/6aae7424-b3f6-4907-98f1-79e2441df1fb)


* **Montage électronique (breadboard)** :
  ![Breadboard](https://github.com/user-attachments/assets/32724c5c-6f43-421d-9ae4-9e5506dda0ea)


* **Aperçu de la page web** :

  * Version **PC** : ![Page PC](https://github.com/user-attachments/assets/f3386d67-5922-4057-bc40-6677b064558d)

  * Version **Mobile** : ![Page mobile](https://github.com/user-attachments/assets/a590327a-ffa1-43a4-961e-a7c3717d8103)


---

## 🗂️ Organisation du code

* `src/` → Code C++ (ESP32)
* `data/` → Fichiers web (HTML/CSS/JS) chargés dans le SPIFFS
* `platformio.ini` → Configuration du projet (PlatformIO)
