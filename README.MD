# Projet PSR 2023/2024

## But du projet

Notre projet consiste en la gestion d'un serveur nommé Sioux consistant simplement en une page HTML de vote pour une image et se récupérer les réponses dans un fichier CSV. Il fonctionne avec un exécutable nommé Ablette qui, quand à lui, récupére les données des différents clients se connectant à notre serveur Sioux.

## Lancement du serveur

Dans le répertoire principal, tapez ```make``` pour compiler l'entièreté des fichiers.

### Sioux

Pour lancez le serveur Sioux, tapez entrez dans le répertoire Sioux ```cd Sioux/``` puis ```./sioux -p``` ou ```./sioux --port``` suivi du numéro de port que vous désirez attribuer à votre serveur.

### Ablette

De la même manière, depuis le répertoire principal, entrez dans Ablette ```cd Ablette/``` puis ```./ablette -p``` ou ```./ablette --port``` suivi de la liste des ports attribués, séparés d'une virgule.

## Fonctionnement

Sur une page Firefox de votre machine, recherchez X.X.X.X:P où X.X.X.X est votre adresse IP et P le port que vous avez attribué à Sioux. Vous arriverez sur notre page vote.html . En réalisant un premier vote, le repertoire csv se crée, contenant le fichier vote-réponse.html dans lequel vous trouverez les infos entrées précédemment.

## Statistiques

Pour trouver les statistiques trouvées par Ablette, recherchez X.X.X.X:P/stats . Sur cette page, vous trouverez le top 5 des adresses sollicitant le plus le serveur.

## Nettoyage

Un simple ```make clean``` dans le répertoire principal supprimera tout les exécutables ainsi que le répertoire principal. 
