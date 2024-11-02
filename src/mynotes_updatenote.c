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

int main()
{
char *envoi;
int i,start,success;
char *query,*reponse,*lienretour,*textebouton,*totpsecret,*totpcode;
char *nom,*mdp,*code,*mdpsave,*dossier,*titre,*note;

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
dossier=(char*)malloc(1000+query_size);
titre=(char*)malloc(1000+query_size);
reponse=(char*)malloc(20000);
lienretour=(char*)malloc(100);
textebouton=(char*)malloc(20);
totpsecret=(char*)malloc(40);
totpcode=(char*)malloc(40);
query=(char*)malloc(20000+query_size);
if(veille_au_grain3(envoi,5,nom,mdp,code,0)==0)
	{
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
	}
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
					if(strcmp(totpcode,code)!=0)
						{
						strcpy(reponse,"Erreur : Code de vérification erroné ...");
						strcpy(lienretour,"/mynotes/index.html");
						strcpy(textebouton,"Réessayer");
						success=0;
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
      <div data-role=\"header\" data-position=\"fixed\" data-theme=\"b\">",nom,mdp,code);
if(success==1)
	{
	printf("<div data-role=\"navbar\">\n\
            <ul>\n\
               <li>\n\
                  <form action=\"/cgi-bin/mynotes_main.cgi\" method=\"post\" id=\"form-addnote\" data-transition=\"none\" data-rel=\"dialog\">\n\
                  <input type=\"hidden\" name=\"txt-compte\" id=\"txt-compte\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-enter\" id=\"txt-enter\" value=\"n\">\n\
                  <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"d\" data-icon=\"back\" class=\"ui-shadow\" style=\"max-width:100%%; border: 0; object-fit: contain;\">Retour aux dossiers</button>\n\
                  </form>\n\
               </li>\n\
            </ul>\n\
      </div>\n\
      </div>\n\
      <div role=\"main\" class=\"ui-content\" data-theme=\"b\">\n\
      <form autocomplete=\"off\" action=\"/cgi-bin/mynotes_doupdatenote.cgi\" method=\"post\" id=\"form\" data-transition=\"none\" data-rel=\"dialog\">\n\
      		<input type=\"hidden\" name=\"txt-compte\" id=\"txt-compte\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-password\" id=\"txt-password\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-code\" id=\"txt-code\" value=\"%s\">\n\
                  <input type=\"hidden\" name=\"txt-dossier\" id=\"txt-dossier\" value=\"%s\">\n\
            <div class=\"ui-field-contain\">\n\
                <label for=\"select-native-1\">Dossier:</label>\n\
                <select name=\"select-native-1\" id=\"select-native-1\">\n",nom,mdp,code,nom,mdp,code,dossier);
	sprintf(query,"select AES_DECRYPT(nomdossier,UNHEX(SHA2(\'%s\',512))) from dossier where compte=\'%s\';",mdp,nom);
	db_query(handler,query);
	for(i=0;i<db_ntuples(result);i++)
		{
      db_getvalue(result,i,0,reponse,20000);
      if(strcmp(dossier,reponse)!=0) printf("<option value=\"%s\">%s</option>\n",reponse,reponse);
      else printf("<option selected=\"selected\" value=\"%s\">%s</option>\n",reponse,reponse);
      }
   db_clear_result(result);
   sprintf(query,"select AES_DECRYPT(contenu,UNHEX(SHA2(\'%s\',512))) from note where compte=\'%s\' and AES_DECRYPT(nomdossier,UNHEX(SHA2(\'%s\',512)))=\'%s\' and AES_DECRYPT(nomnote,UNHEX(SHA2(\'%s\',512)))=\'%s\';",mdp,nom,mdp,dossier,mdp,titre);
	db_query(handler,query);
   printf("</select>\n\
            </div>\n\
            <input type=\"hidden\" name=\"txt-titreinitial\" id=\"txt-titreinitial\" value=\"%s\">\n\
            <div data-role=\"fieldcontain\">\n\
               <label for=\"txt-titre\">Titre</label>\n\
               <input type=\"text\" name=\"txt-titre\" id=\"txt-titre\" value=\"%s\" maxlength=\"200\" required=\"required\">\n\
            </div>\n",titre,titre);
   db_getvalue(result,0,0,reponse,20000);
   printf("<div data-role=\"fieldcontain\">\n\
               <label for=\"txt-note\">Note</label>\n\
               <textarea name=\"txt-note\" id=\"txt-note\" value=\"\" maxlength=\"20000\" required=\"required\">%s</textarea>\n\
            </div>\n\
            <button type=\"submit\" data-mini=\"false\" data-inline=\"false\" data-theme=\"c\" data-icon=\"action\">Enregistrer</button>\n\
         </form>\n\
      </div>\n\
      <div data-role=\"footer\" data-position=\"fixed\" data-theme=\"b\">\n\
         <h4>MyNotes</h4>\n\
      </div> <! footer>\n\
   </div> <! page>\n\
</body>\n\
</html>\n",reponse);
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