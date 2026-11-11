# KindleRemoteDisplay

Go API, FrontEnd editor &amp; C programme for kindle to display the text from the website via API

## Prérequis

- Une Kindle JailBreak : [KindleModding - Find my Jailbreak](https://kindlemodding.org/jailbreak-wizard.html)

- Un cross-compiler pour votre modèle de Kindle (Pour ma part Toolchain ARM générique)

- Les bibliothèques C libcurl et FBInk

- Go

## Démarrage rapide

1. Clonnez le dépôt, modifiez la constante URL de *kindle-sync-app.c* avec l'IP de votre PC Hôte et lancez la compilation de *kindle-sync-app.c* avec le cross compiler adapté à votre kindle (pour ma part arm-kindlepw2-linux-gnueabi-gcc sur Kindle 5.15.1.1) -> nommez l'exécutable *kindle-sync-app*

2. Vérifier que le port 8080 est autorisé sur votre part feu. 

3. Copier l'exécutable issue de la compilation à la racine de votre kindle */mnt/us*, copier le script *kindle-sync-app.sh* dans le répertoire documents */mnt/us/documents*. 

4. Démontez la kindle et connectez la au même réseau wifi que le PC Hôte.

5. Sur le Pc Hôte, lancez l'API Go, depuis la racine du projet, Lancer *go run GoAPI/main.go*, L'interface de communication est maintenant accessible depuis *http://localhost:8080*. 

6. Lancez le script sur votre kindle via la page d'accueil ou la bibliothèque, **un appui tactile sur l'écran fermera la fenêtre** 

7. Et voila, les changements validés sur l'interface WEB s'afficheront sur la kindle ! 

## Descriptif technique

- Bibliothèque C particulière utilisée : libcurl = permet de gérer les requêtes http en C, FBInk = utilisé pour gérer les écrans à encre.

- Routes API : */message* (GET) permet de récupérer le message en plain/text. */* (GET) permet d'accéder à l'interface WEB. */static/* (GET) permet de récupérer les assets WEB (JS, CSS, image). */edit* (POST) permet de modifier le message avec un corps application/json.
