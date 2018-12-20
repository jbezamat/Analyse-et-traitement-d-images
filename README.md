# Analyse-et-traitement-d-images
Membres de l'équipe : Baillot Lauren, Bezamat Jérémy, Bosseur Océane et Saliba Thomas

Sujet : Détection des lignes du terrain

##Pour exécuter une image :
<code>$ ./line_detector 0 chemin/image_name</code>

exemple:
<code>$ ./line_detector 0 data/log1/144-rgb.png</code>
##Pour générer une vidéo à partir de l'ensemble des images d'un dossier :
<code>$ ./line_detector 1 chemin/directory_name</code>

exemple:
<code>$ ./line_detector 1 data/log1</code>
Chaque image insérée à la vidéo sera affichée. Rester appuyé sur la touche "entrée" afin de faire défiler toutes les images et de pouvoir accéder à la vidéo qui est enregistrée dans chemin.

##Pour exécuter les images de travail utilisées:
<code>$ make test</code>

##Pour générer une vidéo à partir de log1:
<code>$ make video</code>
