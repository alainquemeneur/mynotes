# mynotes

MyNotes : Auto-hébergement de notes

MyNotes est une webapp que j'ai développée pour auto-héberger des notes, accessibles en ligne. Il offre un service similaire à SimpleNote mais auto-hébergé et les notes sont chiffrées en AES256 avec le mot de passe de l'utilisateur. Il n'y a aucun moyen de récupérer les notes si l'utilisateur a oublié son mot de passe. Le logiciel gère une infinité de dossiers pour chaque utilisateur inscrit, dans lesquels on peut mettre une infinité de notes. Les dossiers (titres) et les notes (titres et contenus) sont chiffrés avec le mot de passe de l'utilisateur. Ce mot de passe ne peut être changé.

J'ai programmé ce logiciel pour répondre à mes besoins.

Techniquement, il s'agit d'une webapp installable n'importe quel OS (Windows, Linux, Mac, iOS, Android), mais prévue pour fonctionner surtout sous iOS ou Android. Le front end est programmé en HTML 5 + toolkit javascript JQuery Mobile. Le back end est constitué de scripts CGI écrits en langage C, traitant les données en interface avec une base de données MySQL ou MariaDB.

Ce programme ne peut fonctionner sans qrencode (pour la génération des QR codes 2FA)

<strong><u>Installation</u></strong>

Sur un serveur Linux, installer NginX, activer le https sur votre domaine, et <a href="https://techexpert.tips/nginx/nginx-shell-script-cgi/">activez les scripts CGI</a> sur votre serveur NginX

Installez mynotes dans /home/votre_repertoire/mynotes

Intégrez votre mynotes dans votre serveur web :
sudo ln -s /home/votre_repertoire/mynotes /var/www/html/mynotes

Installez JQuery Mobile dans /var/www/html/jq :
sudo cp jq.tgz /var/www/html
cd /var/www/html
sudo tar -zxvf ./jq.tgz

Désormais votre webapp est accessible à l'URL : https://votre_serveur/mynotes

Installez qrencode pour être en mesure de générer les QR codes nécessaires pour le 2FA Google Authenticator:
sudo apt-get install qrencode

Assurez vous des bons droits sur le répertoire qr :
cd /home/votre_repertoire/mynotes
chmod 777 qr

Il faut maintenant préparer la base de données MySQL :
cd /home/votre_repertoire/mynotes/build
mysql -u root -p mysql
create database mynotes;
create user 'mynotes'@'localhost' identified by 'votre_mot_de_passe';
grant all privileges on mynotes.* to 'mynotes'@'localhost';
quit

mysql -u mynotes -p mynotes
source ./install_mysql_tables.sql;
quit

Avant de compiler les scripts CGI, il faut leur indiquer le mot de passe choisi pour l'utilisateur mynotes dans MySQL ainsi que l'URL du serveur sur Internet:
cd /home/votre_repertoire/mynotes/src
Renseigner le mot de passe MySQL que vous avez choisi dans password.h

Il faut maintenant compiler les scripts CGI :
sudo apt install libmysqlclient-dev
cd /home/votre_repertoire/mynotes/src
sudo mkdir /usr/lib/cg-bin
chmod a+x compile*
sudo ./compile.mysql

Si vous utilisez MariaDB plutôt que MySQL compilez à la place par :
sudo ./compile.mariadb

Une fois que les scripts sont compilés et apparus dans /usr/lib/cgi-bin, il faut effacer par sécurité tous les fichiers présents dans /home/votre_repertoire/mynotes/src :
cd /home/votre_repertoire/mynotes/src
rm * (vérifiez bien d'être dans le bon répertoire avant de taper cela !!!)

Connectez-vous à https://votre_serveur/mynotes et utilisez la webapp

alain.quemeneur@gmail.com