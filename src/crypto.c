#include <string.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <ctype.h> 
#include <unistd.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <sys/sem.h>

char master_key[100],*mk;

void message_service(char *message)
{
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
      <div role=\"main\" class=\"ui-content\">\n\
   <p></p>\n\
   %s\n\
   </div>\n\
   <div data-role=\"footer\" data-position=\"fixed\" data-theme=\"b\">\n\
   <h4>MyNotes</h4>\n\
   </div>\n\
   </div>\n\
</body>\n\
</html>\n",message);
exit(0);
}

union type_argument
{
int valeur;
struct semid_ds *tampon;
unsigned short *tab_valeur;
} argument;

int sem;

int init_semaphore()
{
int sem;

if((sem=semget(ftok(getenv("HOME"),1),1,IPC_CREAT | 0644))==-1) message_service("Problème d'initialisation du service (sem)");
argument.valeur=1;
semctl(sem,0,SETVAL,argument);
}

int get_semaphore()
{
if((sem=semget(ftok(getenv("HOME"),1),1,0644))==-1) message_service("Problème d'initialisation du service (sem)");
}

void prendre_jeton()
{
struct sembuf s;

s.sem_num=0;
s.sem_op=-1;
s.sem_flg=0;
semop(sem,&s,1);
}

void rendre_jeton()
{
struct sembuf s;

s.sem_num=0;
s.sem_op=1;
s.sem_flg=0;
semop(sem,&s,1);
}

int get_master_key()
{
int seg;

if((seg=shmget(ftok(getenv("HOME"),1),100, 0644))==-1) message_service("En maintenance. Merci de bien vouloir vous connecter ultérieurement");
if((mk=shmat(seg,NULL,0))==(void *)-1) message_service("Problème d'initialisation du service (shm)");
get_semaphore();
prendre_jeton();
strcpy(master_key,mk);
rendre_jeton();
}

int init_master_key(char *masterk)
{
int seg;

if((seg=shmget(ftok(getenv("HOME"),1),100, 0644))!=-1) message_service("Initialisation déjà faite");
if((seg=shmget(ftok(getenv("HOME"),1),100, IPC_CREAT | 0644))==-1) message_service("Initialisation déjà faite");
if((mk=(char *)shmat(seg,NULL,0))==(void *)-1) message_service("Problème d'initialisation du service (shm)");
init_semaphore();
prendre_jeton();
strcpy(mk,masterk);
rendre_jeton();
message_service("Initialisé !");
}