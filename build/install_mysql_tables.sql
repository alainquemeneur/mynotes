CREATE TABLE compte (email text,mdp text,actif text,totpsecret text,totpcode text);
CREATE UNIQUE INDEX comptex ON compte (email(100));
CREATE TABLE dossier (compte text,nomdossier varbinary(400));
CREATE UNIQUE INDEX dossierx ON dossier (compte(100),nomdossier(400));
CREATE TABLE note (compte text,nomdossier varbinary(400),nomnote varbinary(400),contenu varbinary(40000),horodatage datetime,jour text,mois text,annee text,heure text,minute text,seconde text);
CREATE UNIQUE INDEX notex ON note (compte(100),nomdossier(400),nomnote(400));