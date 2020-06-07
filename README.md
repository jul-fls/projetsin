# Projet BAC STI2D Sin
Ce projet consiste à créer un aquarium "connecté" pour un aquariophile de manière à pouvoir automatiser et monitorer tous ses aquariums sur des pages web avec des ESP32. 
En tant que membre du projet de mon groupe (4 élèves), j'avais la responsabilité de créer l'IHM (Interface Homme-Machine) avec une carte ESP-32 (de la société Espressif), j'ai donc utilisé les librairies suivantes :
->ESPAsyncWebServer (Pour le serveur Web)
->ESPAsyncWiFiManager (Pour la gestion de la connexion Wifi)
->SPIFFS (Pour l'accès à la memoire flash)
->EEPROM (Pour l'accès à l'EEPROM)
->NTPClient (Pour l'accès au serveur NTP "europe.pool.ntp.org", de manière à toujours avoir l'heure et la date d'accessible pour les automatisations)
