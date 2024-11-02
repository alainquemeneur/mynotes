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
#include "crypto.c"

int main()
{
char *envoi;
int i,start,success,nb_messages,lastid,idfirst,idlast,id;
char *query,*reponse,*lienretour,*textebouton,*totpsecret,*totpcode;
char *nom,*mdp,*code,*mdpsav,*dossier,*titre,*note,*jour,*mois,*annee,*heure,*minute,*seconde;

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
mdpsav=(char*)malloc(1000+query_size);
code=(char*)malloc(1000+query_size);
titre=(char*)malloc(1000+query_size);
note=(char*)malloc(1000+query_size);
dossier=(char*)malloc(1000+query_size);
lienretour=(char*)malloc(100);
textebouton=(char*)malloc(20);
totpsecret=(char*)malloc(40);
totpcode=(char*)malloc(40);
jour=(char*)malloc(40);
mois=(char*)malloc(40);
annee=(char*)malloc(40);
heure=(char*)malloc(40);
minute=(char*)malloc(40);
seconde=(char*)malloc(40);
reponse=(char*)malloc(20000);
query=(char*)malloc(20000+query_size);
get_chaine(envoi,5,nom);
tamb(nom);
get_chaine(envoi,4,mdp);
tamb(mdp);
get_chaine(envoi,3,code);
tamb(code);
get_chaine(envoi,2,dossier);
tamb(dossier);
get_chaine(envoi,1,titre);
tamb(titre);
strcpy(mdpsav,mdp);
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
      		if(strcmp(totpsecret,"")==0) success=1;
      		else
         		{
         		if(strcmp(totpcode,code)==0) success=1;
         		else
            		{
            		strcpy(reponse,"Erreur : Code de vérification erroné ...");
            		strcpy(lienretour,"/mynotes/index.html");
            		strcpy(textebouton,"Réessayer");
            		sleep(2);
         			}
         		}
         	if(success==1)
         		{
					}
				}
			}
		}
	}
strcpy(mdp,mdpsav);
printf("Content-Type: text/html\n\n\
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
      <div data-role=\"header\" data-position=\"fixed\" data-theme=\"b\">\n\
      <div data-role=\"navbar\">\n\
            <ul>\n\
               <li>\n\
                  <form action=\"/cgi-bin/mynotes_main.cgi\" method=\"post\" id=\"form-main\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-nom\" id=\"txt-nom\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-enter\" id=\"txt-enter\" value=\"n\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"d\" data-icon=\"back\" class=\"ui-shadow\" style=\"max-width:100%%; border: 0; object-fit: contain;\">Retour aux dossiers</button>\n\
                  </form>\n\
               </li>\n\
            </ul>\n\
         </div>\n\
      </div>\n\
      <div role=\"main\" class=\"ui-content\">\n",nom,mdp,code);
if(success!=1) 
	{
	printf("%s\n",reponse);
	printf("</div>\n\
<div data-role=\"footer\" data-position=\"fixed\" data-theme=\"b\">\n\
<h4>MyNotes</h4>\n\
</div>\n\
</div>\n\
</body>\n\
</html>\n");
	}
else
	{
	printf("<p>Dossier : <strong>%s</strong></p>\n",dossier);
   sprintf(query,"select AES_DECRYPT(nomnote,UNHEX(SHA2(\'%s\',512))),AES_DECRYPT(contenu,UNHEX(SHA2(\'%s\',512))),jour,mois,annee,heure,minute,seconde from note where compte=\'%s\' and AES_DECRYPT(nomdossier,UNHEX(SHA2(\'%s\',512)))=\'%s\' and AES_DECRYPT(nomnote,UNHEX(SHA2(\'%s\',512)))=\'%s\';",mdpsav,mdpsav,nom,mdpsav,dossier,mdpsav,titre);
   db_query(handler,query);
   db_getvalue(result,0,2,reponse,20000);
   printf("<p>Note enregistrée le : <strong>%s/",reponse);
   db_getvalue(result,0,3,reponse,20000);
   printf("%s/",reponse);
   db_getvalue(result,0,4,reponse,20000);
   printf("%s à ",reponse);
   db_getvalue(result,0,5,reponse,20000);
   printf("%s:",reponse);
   db_getvalue(result,0,6,reponse,20000);
   printf("%s:",reponse);
   db_getvalue(result,0,7,reponse,20000);
   printf("%s</strong>\n</p>\n",reponse);
   db_getvalue(result,0,0,reponse,20000);
   printf("<br><p><strong>%s</strong>\n\
            </p>\n",reponse);
   db_getvalue(result,0,1,reponse,20000);
   printf("<p>%s</p>\n</div>\n\
      <div data-role=\"footer\" data-position=\"fixed\" data-theme=\"b\">\n\
         <div data-role=\"navbar\">\n\
            <ul>\n\
            	<li>\n\
                  <form action=\"/cgi-bin/mynotes_updatenote.cgi\" method=\"post\" id=\"form-adddossier\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-compte\" id=\"txt-compte\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-dossier\" id=\"txt-dossier\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-titre\" id=\"txt-titre\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"d\" data-icon=\"edit\" class=\"ui-shadow\" style=\"max-width:100%%; border: 0; object-fit: contain;\">Modifier note</button>\n\
                  </form>\n\
               </li>\n\
               <li>\n\
                  <form action=\"/cgi-bin/mynotes_supprnote.cgi\" method=\"post\" id=\"form-modifdossier\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-compte\" id=\"txt-compte\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-dossier\" id=\"txt-dossier\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-titre\" id=\"txt-titre\" value=\"%s\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"d\" data-icon=\"delete\" class=\"ui-shadow\" style=\"max-width:100%%; border: 0; object-fit: contain;\">Supprimer note</button>\n\
                  </form>\n\
               </li>\n\
            </ul>\n\
      	</div> <! navbar>\n\
      </div> <! footer>\n\
   </div> <! page>\n\
</body>\n\
</html>\n",reponse,nom,mdp,code,dossier,titre,nom,mdp,code,dossier,titre);
	}
}