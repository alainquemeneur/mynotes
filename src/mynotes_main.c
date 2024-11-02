#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#define __USE_XOPEN
#include <time.h>
#include "mysql.h"
#include "mysql.c"
#include "commun.c"
#include "password.h"
#include "totp.c"
#include "crypto.c"

struct dossier
{
char nom[200];
};

struct note
{
char dossier[200],nom[200],contenu[20000],horodatage[100],jour[4],mois[4],annee[6],heure[4],minute[4],seconde[4];
};

int main()
{
char *envoi;
int i,j,start,success,nb_dossiers,nb_notes;
char *query,*reponse,*lienretour,*textebouton,*totpsecret,*totpcode;
char *nom,*mdp,*code,*enter,*mdpsave;
struct dossier *dossiers;
struct note *notes;

//if(getenv("HTTPS")==NULL || strcmp(getenv("HTTPS"),"on")!=0) Send("https non activé");
envoi=read_POST();
success=0;
start=strlen(envoi);
for(i=0;i<start;i++)
	{
	if(envoi[i]<32) envoi[i]=0;
	}
query_size=atoi(getenv("CONTENT_LENGTH"));
nom=(char*)malloc(1000+query_size);
mdp=(char*)malloc(1000+query_size);
mdpsave=(char*)malloc(1000+query_size);
code=(char*)malloc(1000+query_size);
enter=(char*)malloc(1000+query_size);
reponse=(char*)malloc(20000);
lienretour=(char*)malloc(100);
textebouton=(char*)malloc(20);
totpsecret=(char*)malloc(40);
totpcode=(char*)malloc(40);
query=(char*)malloc(20000+query_size);
if(veille_au_grain3(envoi,4,nom,mdp,code,0)==0)
	{
	get_chaine(envoi,4,nom);
	tamb(nom);
	get_chaine(envoi,3,mdp);
	tamb(mdp);
	get_chaine(envoi,2,code);
	tamb(code);
	get_chaine(envoi,1,enter);
	tamb(enter);
	}
else strcpy(enter,"n");
strcpy(mdpsave,mdp);
hache(mdp);
if((handler=db_opendatabase("mynotes","localhost","mynotes",PASSWORD))==NULL)
	{
	strcpy(reponse,"Erreur : Impossible d'accéder à la base de données");
	strcpy(lienretour,"/mynotes/index.html");
	strcpy(textebouton,"Réessayer");
	}
else
	{
	sprintf(query,"select * from compte where email=\'%s\';",nom);
	db_query(handler,query);
	if(db_ntuples(result)==0)
		{
		strcpy(reponse,"Erreur : Ce compte n'existe pas ...");
		strcpy(lienretour,"/mynotes/index.html");
		strcpy(textebouton,"Réessayer");
		db_clear_result(result);
		db_close(handler);
		}
	else
		{
		db_clear_result(result);
		sprintf(query,"select * from compte where email=\'%s\' and mdp=\'%s\';",nom,mdp);
		db_query(handler,query);
		if(db_ntuples(result)==0)
			{
			strcpy(reponse,"Erreur : Mauvais mot de passe ...");
			strcpy(lienretour,"/mynotes/index.html");
			strcpy(textebouton,"Réessayer");
			db_clear_result(result);
			db_close(handler);
			sleep(2);
			}
		else
			{
			db_clear_result(result);
			sprintf(query,"select * from compte where email=\'%s\' and mdp=\'%s\' and actif=\'oui\';",nom,mdp);
			db_query(handler,query);
			if(db_ntuples(result)==0)
				{
				strcpy(reponse,"Erreur : Votre compte n'a pas été activé par l'administrateur ...");
				strcpy(lienretour,"/mynotes/index.html");
				strcpy(textebouton,"Réessayer");
				db_clear_result(result);
				db_close(handler);
				}
			else
				{
				db_clear_result(result);
				sprintf(query,"select totpsecret,totpcode from compte where email=\'%s\';",nom);
				db_query(handler,query);
				db_getvalue(result,0,0,totpsecret,40);
				db_getvalue(result,0,1,totpcode,40);
				db_clear_result(result);
				success=1;
				if(strcmp(totpsecret,"")!=0)
					{
					if(strcmp(enter,"y")==0)
						{
						if(checkcode(totpsecret,code)==0)
							{
							sprintf(query,"update compte set totpcode=\'%s\' where email=\'%s\';",code,nom);
							db_query(handler,query);
							db_clear_result(result);
							}
						else 
							{
							strcpy(reponse,"Erreur : Code de vérification erroné ...");
							strcpy(lienretour,"/mynotes/index.html");
							strcpy(textebouton,"Réessayer");
							success=0;
							sleep(2);
							}
						}
					else
						{
						if(strcmp(totpcode,code)!=0)
							{
							strcpy(reponse,"Erreur : Code de vérification erroné ...");
							strcpy(lienretour,"/mynotes/index.html");
							strcpy(textebouton,"Réessayer");
							success=0;
							sleep(2);
							}
						}
					}
				if(success==1)
					{
					sprintf(query,"rm /var/www/html/mynotes/qr/qr%s.png",nom);
					system(query);
					sprintf(query,"rm /tmp/mynotes_totp%s",nom);
					system(query);
					}
				}
			}
		}
	}
strcpy(mdp,mdpsave);
printf("Set-Cookie: mynotesemail=%s; HttpOnly; Path=/\n\
Set-Cookie: mynotesmdp=%s; HttpOnly; Path=/\n\
Set-Cookie: mynotescode=%s; HttpOnly; Path=/\n\
Content-Type: text/html\n\n\
<!DOCTYPE html>\n\
<html>\n\
<head>\n\
<title>MyNotes</title>\n\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n\
<meta charset=\"utf-8\">\n\
<link rel=\"icon\" type=\"image/x-icon\" href=\"/mynotes/mynotes.png\">\n\
<link rel=\"stylesheet\" href=\"/mynotes/themes/orange.min.css\" />\n\
<link rel=\"stylesheet\" href=\"/mynotes/themes/jquery.mobile.icons.min.css\" />\n\
<link rel=\"stylesheet\" href=\"/jq/jquery.mobile.structure-1.4.5.min.css\" />\n\
<script src=\"/jq/demos/js/jquery.min.js\"></script>\n\
<script src=\"/jq/jquery.mobile-1.4.5.min.js\"></script>\n\
</head>\n\
<body>\n\
   <div data-role=\"page\" data-theme=\"b\">\n\
      <div data-role=\"header\" data-position=\"fixed\" data-theme=\"b\">\n<h4>MyNotes</h4>\n",nom,mdp,code);
if(success==1)
	{
	printf("</div>\n\
      <div role=\"main\" class=\"ui-content\" data-theme=\"b\">\n\
      <div style=\"text-align:center\"><img src=\"/mynotes/mynotesmini.png\"></div>\n\
      <div><strong>Dossiers et notes</strong>\n");
	sprintf(query,"select AES_DECRYPT(nomdossier,UNHEX(SHA2(\'%s\',512))) from dossier where compte=\'%s\' order by nomdossier asc;",mdp,nom);
	db_query(handler,query);
	nb_dossiers=db_ntuples(result);
	dossiers=(struct dossier *)malloc(nb_dossiers*sizeof(struct dossier));
	for(i=0;i<nb_dossiers;i++) 
		{
		db_getvalue(result,i,0,reponse,20000);
		strcpy(dossiers[i].nom,reponse);
		}
	db_clear_result(result);
	for(i=0;i<nb_dossiers;i++)
		{
		printf("<div data-role=\"collapsible\" data-collapsed=\"true\" data-theme=\"b\">\n\
		<h4>%s</h4>\n\
		<ul data-role=\"listview\">\n",dossiers[i].nom); 
		sprintf(query,"select AES_DECRYPT(nomnote,UNHEX(SHA2(\'%s\',512))),AES_DECRYPT(contenu,UNHEX(SHA2(\'%s\',512))),horodatage,jour,mois,annee,heure,minute,seconde,AES_DECRYPT(nomdossier,UNHEX(SHA2(\'%s\',512))) from note where compte=\'%s\' and AES_DECRYPT(nomdossier,UNHEX(SHA2(\'%s\',512)))=\'%s\' order by horodatage desc;",mdp,mdp,mdp,nom,mdp,dossiers[i].nom);
		db_query(handler,query);
		nb_notes=db_ntuples(result);
		notes=(struct note *)malloc(nb_notes*sizeof(struct note));
		for(j=0;j<nb_notes;j++)
			{
			db_getvalue(result,j,0,reponse,20000);
			strcpy(notes[j].nom,reponse);
			db_getvalue(result,j,1,reponse,20000);
			strcpy(notes[j].contenu,reponse);
			db_getvalue(result,j,2,reponse,20000);
			strcpy(notes[j].horodatage,reponse);
			db_getvalue(result,j,3,reponse,20000);
			strcpy(notes[j].jour,reponse);
			db_getvalue(result,j,4,reponse,20000);
			strcpy(notes[j].mois,reponse);
			db_getvalue(result,j,5,reponse,20000);
			strcpy(notes[j].annee,reponse);
			db_getvalue(result,j,6,reponse,20000);
			strcpy(notes[j].heure,reponse);
			db_getvalue(result,j,7,reponse,20000);
			strcpy(notes[j].minute,reponse);
			db_getvalue(result,j,8,reponse,20000);
			strcpy(notes[j].seconde,reponse);
			db_getvalue(result,j,9,reponse,20000);
			strcpy(notes[j].dossier,reponse);
			printf("<li>\n\
					<form action=\"/cgi-bin/mynotes_viewnote.cgi\" method=\"post\" id=\"form-viewnote\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-dossier\" id=\"txt-dossier\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-titre\" id=\"txt-titre\" value=\"%s\">\n\
                  <input type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"d\" data-icon=\"eye\" value=\"%s<br>%s/%s/%s\">\n\
                  </form>\n\
                  </li>\n",nom,mdp,code,notes[j].dossier,notes[j].nom,notes[j].nom,notes[j].jour,notes[j].mois,notes[j].annee);
			}
		free(notes);
		db_clear_result(result);
		printf("</ul>\n</div>\n");
		}
	printf("</div>\n\
      </div>\n\
      <div data-role=\"footer\" data-position=\"fixed\" data-theme=\"b\">\n\
         <div data-role=\"navbar\">\n\
            <ul>\n\
            	<li>\n\
                  <form action=\"/cgi-bin/mynotes_adddossier.cgi\" method=\"post\" id=\"form-adddossier\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-compte\" id=\"txt-compte\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"d\" data-icon=\"plus\" class=\"ui-shadow\" style=\"max-width:100%%; border: 0; object-fit: contain;\">Dossier</button>\n\
                  </form>\n\
               </li>\n\
               <li>\n\
                  <form action=\"/cgi-bin/mynotes_modifdossier.cgi\" method=\"post\" id=\"form-modifdossier\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-compte\" id=\"txt-compte\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"d\" data-icon=\"edit\" class=\"ui-shadow\" style=\"max-width:100%%; border: 0; object-fit: contain;\">Dossier</button>\n\
                  </form>\n\
               </li>\n\
               <li>\n\
                  <form action=\"/cgi-bin/mynotes_supprdossier.cgi\" method=\"post\" id=\"form-deldossier\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-compte\" id=\"txt-compte\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"d\" data-icon=\"delete\" class=\"ui-shadow\" style=\"max-width:100%%; border: 0; object-fit: contain;\">Dossier</button>\n\
                  </form>\n\
               </li>\n\
               <li>\n\
                  <form action=\"/cgi-bin/mynotes_addnote.cgi\" method=\"post\" id=\"form-addnote\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-compte\" id=\"txt-compte\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"d\" data-icon=\"plus\" class=\"ui-shadow\" style=\"max-width:100%%; border: 0; object-fit: contain;\">Note</button>\n\
                  </form>\n\
               </li>\n\
               <li>\n\
                  <form action=\"/cgi-bin/mynotes_2fa.cgi\" method=\"post\" id=\"form-2fa\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-compte\" id=\"txt-compte\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"d\" data-icon=\"gear\" class=\"ui-shadow\" style=\"max-width:100%%; border: 0; object-fit: contain;\">2FA</button>\n\
                  </form>\n\
               </li>\n\
            </ul>\n\
      	</div> <! navbar>\n\
      </div> <! footer>\n\
   </div> <! page>\n\
</body>\n\
</html>\n",nom,mdp,code,nom,mdp,code,nom,mdp,code,nom,mdp,code,nom,mdp,code);
	}
else
	{
	printf("</div>\n\
      <div role=\"main\" class=\"ui-content\">\n\
      %s<br>\n\
<a href=\"%s\" data-role=\"button\" data-icon=\"back\" data-theme=\"c\">%s</a><br><br>\n",reponse,lienretour,textebouton);
	printf("</div>\n\
<div data-role=\"footer\" data-position=\"fixed\" data-theme=\"b\">\n\
<h4>MyNotes</h4>\n\
</div>\n\
</div>\n\
</body>\n\
</html>\n");
	}
}