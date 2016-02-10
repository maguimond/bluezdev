# README #

## UTILISATION (POUR DEVELOPPEMENT SEULEMENT) ##
-------------
* Installer BlueZ
* - sudo apt-get install bluez
* Installer Bluetooth lib dev
* - sudo apt-get install libbluetooth-dev
* Pour accéder au code source de la librairie bluez: http://www.bluez.org/development/

## TESTS AVEC SCRIPT / BLUEZ ##
-------------
NOTE: CES TESTS ONT ETE EFFECTUES SOUS LA VERSION 4.99 DE BLUEZ.
VOIR Tools/Scripts.

* Avant d'aller explorer le code source du "gatttool", bien comprendre son fonctionnement (voir le jeu de commandes suivant):
* gatttool -b DA:D3:18:D1:CD:8F --char-write-req -a 0x0011 -n 0300;gatttool -b DA:D3:18:D1:CD:8F --char-write-req -a 0x0E -n deadbeefc7;gatttool -b DA:D3:18:D1:CD:8F --char-read --handle=0x0010 --listen

Details:

* Si ce n'est pas fait, on determine l'adresse du serveur: "hcitool lescan"
* On Specifie adresse du serveur : "gatttool -b DA:D3:18:D1:CD:8F"
* On ecrit dans le handle 0x0011 (handle de configuration***) la valeur 0300 dans le but de setter les flags de notification et d'indication 0b 0000 0011 0000 0000 (Selon le standard Bluetooth) : "--char-write-req -a 0x0011 -n 0300"
* On enchaîne la prochaine commande : ";"
* Toujours en specifiant la meme addresse, a qui nous parlons : "gatttool -b DA:D3:18:D1:CD:8F"
* Le handle 0x000E est celui cree pour les capteurs BlueMon/BlueTemp, voir le rapport de la config Bluetooth (https://bitbucket.org/genialebbbdev/bluetoothcfgtool/src/b05a2a3839791ee9101c0bb86acbe2e6502d34cb/ublue_setup.gen.out.txt?at=default)
* - En resume ce handle est comme un UART TX over Bluetooth, on y ecrit des bytes de test pour le moment, sans oublier d'y ajouter le checksum, sinon le BlueMon ou BlueTemp va donner une erreur de checksum. 
* - Se referer au document BlueMon-ProtocoledeCommunication.docx sous https://bitbucket.org/geniale-ele/bluemon.
* - Les bytes de test sont 0xDEADBEEF et le checksum a ajouter est alors !(DE+AD+BE+EF)=C7, dont : "--char-write-req -a 0x0E -n deadbeefc7"
* On enchaîne la prochaine commande : ";"
* Toujours en specifiant la meme addresse, a qui nous parlons : "gatttool -b DA:D3:18:D1:CD:8F"
* Maintenant on lit le UUID 0x1006 associe au handle 0x0010, en resume ce handle est comme un UART RX over Bluetooth, on y retrouve les reponses aux commandes (comme dans cet exemple) ou les valeurs des temperatures (plus tard niveau ou densite aussi) : "--char-read --handle=0x0010 --listen"
* - "listen" Pour ecouter si le characteristic nous transmet des donnees periodiquement.
* - Ca devrait retourner "de 15 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 c7"
* - "de" car c'est le premier byte du test "DEADBEEF", il croit que c'est la commande (voir BlueMon-ProtocoledeCommunication.docx)
* - "15 = ACK" (voir BlueMon-ProtocoledeCommunication.docx)
* - "10 00" le niveau de la batterie mais il est erronne car c'est sur le breadboard que je test)
* - Le reste j'ai encore du code a faire mais "c7" est le checksum retourne

*** Le handle de configuration est determine en la listant les CCC (Client Characteristic Configuration) : "char-read-uuid 2902"
    Ou 0x2902 est le UUID assigne par le standard Bluetooth, voir https://developer.bluetooth.org/gatt/descriptors/Pages/DescriptorViewer.aspx?u=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml

Exemple pour lire un UUID en mode interactif (tout ce qui a ete fait ci-haut peut etre fait en mode interactif)

* gatttool -b DA:D3:18:D1:CD:8F --interactive
* connect
* char-read-uuid 1006

## REVERSE-ENGINEERING BLUEZ (bluez-5.36) ##
-------------
Tout d'abord entrer dans le code source bluez (À downloader: http://www.bluez.org/development/ ou clôner: git clone git://git.kernel.org/pub/scm/bluetooth/bluez.git)

* Pour trouver un fichier parmis les répertoires: find -name NomDuFichier (ex: find -name bluetooth.h)
* Pour trouver le fichier contenant une string en particulier: grep -rnw 'Répertoire ou chercher' -e "StringAChercher" (ex: grep -rnw '.' -e "interactive")

### gatttool en mode interactif ###
Retracement des fonctions en mode interactif (ex: gatttool -b DA:D3:18:D1:CD:8F --interactive)

#### connect ####
* cmd_connect (attrib/interactive.c) -> gatt_connect
* gatt_connect (attrib/utils.c) -> bt_io_connect
* bt_io_connect (btio/btio.c) -> l2cap_connect -> connect
* connect (sys/socket.h)

#### char-read-uuid ####
* cmd_read_uuid (attrib/interactive.c) -> gatt_read_char_by_uuid
* gatt_read_char_by_uuid (attrib/gatt.c) -> g_attrib_send
* g_attrib_send (attrib/gattrib.c) -> bt_att_send
* bt_att_send (src/shared/att.c) -> queue_push_tail
* queue_push_tail (src/shared/queue.c -> )

#### char-write-req ####
* 
