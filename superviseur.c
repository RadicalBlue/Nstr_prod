/* les messages envoye par les threads pieces sont les suivants:
 * "deposer brute conv\0" 
 * "deposer usine conv\0"
 * "deposer brute table\0"
 * "retirer usine conv\0"
 * 
 * les messages recu par les threads pieces sont les suivants:
 * "depose\0"
 * "retirer\0"
 * "defaillance\0"
 */

#include "superviseur.h"
#define NBRMACHINE 3


/***************************************************************************************************************************************************/
extern pthread_mutex_t mutexMachine[NBRMACHINE]; /*id du mutex concernant les machines en fonctionnement*/
extern pthread_mutex_t mutexConvoyeur; /*id du mutex du convoyeur: si il est utilise ou non*/
extern pthread_t threadID; /*identifiant du thread ayant envoye le signal SIGUSR2 pour signaler un anomalie lors du traitement d'une piece par une machine*/
extern pthread_t thIdDialog;/*id du thread dialog*/
extern mqd_t messageQueueRobotAl; /*identifiant de la file de message utilise par les threads pieces et le thread robot alimentation*/
extern mqd_t messageQueueRobotRe; /*identifiant de la file de message utilise par les threads pieces et le thread robot retrait*/
extern mqd_t messageQueueMachine[NBRMACHINE]; /*identifiant de la file de message utilise par les threads pieces et les threads machine*/
/***************************************************************************************************************************************************/

bool machineEnPanne[NBRMACHINE] = false;
Liste listeThreadPiece;
int code_piece, numero_machine;
bool etat;

typedef struct s_mydata{
  int piece;
  int machine;
} data;

/***********************************************************
 * 
 * 
 * Fonctions evenementielles
 * 
 * *******************************************************/
/*Fonction exacutee en cas d'envoie du signal SIHUSR1 qui indique la fin anormale
 * d'un thread de traitement de piece, suite a un probleme durant le depot ou
 * de retrait de piece
 */
void fnc_evenementielle_USER1(int s)
{
  while(!isEmpty(listeThreadPiece))
  {
    pthread_t th = pullElement(listeThreadPiece);
    listeThreadPiece = removeFirst(listeThreadPiece);
    pthread_kill(th,SIGKILL);
  }
  erreur("Fin anormale du system\n", 90);
}
/*
 *
 * fonction executee en cas d'envoi du signal SIGUSR2 qui indique la fin nomarle d'un thread 
 * de traitement de piece ou bien la fin du thread suite au une panne de machine utilisee
 * 
 */
void fnc_evenementielle_USER2(int s)
{
  listeThreadPiece = removeFromList(listeThreadPiece,threadID);
}
/*******************************************************************************************/



/**********************************************************
 * 
 * Procedure du thread dialog 
 * 
 * 
 * *******************************************************/
void * th_Dialogue()
{
  int i;
  /*initialisation des mutex*/
  for(i = 0 ; i < NBRMACHINE; i++)
  {
    pthread_mutex_init (&mutexMachine[i],NULL);
  }
  
  pthread_mutex_init (&mutexConvoyeur,NULL);
  
  listeThreadPiece = creatList();
  
 /*******************protection du signal SIGUSR1**************************************************/
  struct sigaction act1;
 
  memset (&act1, '\0', sizeof(struct sigaction));
  act.sa_handler = &fnc_evenementielle_USER1;
  if (sigaction(SIGUSR1, &act1, NULL) < 0)
  {
    erreur("erreur sigaction : ",96);
  }
  
 /************************************************************************************************/
 /*******************protection du signal SIGUSR2**************************************************/
  struct sigaction act2;
 
  memset (&act2, '\0', sizeof(struct sigaction));

  act.sa_handler = &fnc_evenementielle_USER2;

  if (sigaction(SIGUSR1, &act2, NULL) < 0)
  {
    erreur("erreur sigaction : ",96);
  }
  
 /************************************************************************************************/
  
  while(1)
  {
    code_piece = saisir_ordre_operateur();
    numero_machine = correspondance_machine_code(code_piece);
    pthread_mutex_lock(mutexConvoyeur);
    etat = machineEnPanne[numero_machine];
    pthread_mutex_unlock(mutexConvoyeur);
    if(etat == true)
    {
      printf("machine demandee en panne\n");
      while(!isEmpty(listeThreadPiece))
      {
	/*attente active*/
      }
      erreur("FIN DU SYSTEM : etat defaillance detecte\n",96);
    }
    else
    {
      pthread_t new_thread = creer_thread(code_piece,numero_machine);
      addInList(listeThreadPiece,new_thread);
    }
  }
}
/************************************************************/

/**********************************************************
 * 
 * Procedure des threads Piece
 * 
 * 
 * *******************************************************/
void * th_piece(void * param_data)
{
  char message[20]; /* message envoye par le thread piece.*/
  char def[20];/*vairable pour tester les messages recu*/
  char *messRec; /*message recu par le thread piece*/
  struct mq_attr attr;/*structure permettant de recevoir les attributs du message dans la file*/
  ssize_t bitRecu; /*nombre de bit recu*/
  struct   timespec timer;
  clock_gettime(CLOCK_REALTIME, &tm); /*initialisation du timer*/
  size_t sizeMessage = 20;
  int piece = param_data->piece;
  int machine = param_data->machine;
  
  if(phtread_mutex_lock(&mutexMachine[machine])!=0)
  {
    erreur("erreur de verouillage du mutex machine : ",96);
  }
    if(phtread_mutex_lock(&mutexConvoyeur)!=0)
  {
    erreur("erreur de verouillage du mutex convoyeur : ",96);
  }
  strcpy(message,"deposer brute conv");
  if(mq_send(messageQueueRobotAl,message,sizeMessage,0)!=0)
  {
    erreur("envoie du message a la file de message robot al: ",95);
  }
  tm.tv_sec += 20;/* timer se declanchera dans 20 secondes */
  nr = mq_receive(messageQueueRobotAl,messRec, attr.mq_msgsize, NULL, timer);
  if (nr == -1)
  {
    erreur("erreur de reception de message (messageQueueRobotAl) : ",94);
  }
  strcpy(def,"defaillance");/*test pour savoir si le message recu est un message de defaillance*/
  /*si on ne reçois pas de message ou un message de defaillance on envoie le signal USR1 au thread th_Dialogue*/
  if(messRec == NULL || strcmp(messRec,def))
  {
    printf("arret du system de supervision : le robot d'alimentation ne répond pas ou n'a pas pu retirer la piece au bout de 20 secondes/n");
    pthread_kill(SIGUSR1,thIdDialog);
  }
  /*Sinon le thread reçois fin de depot sur convoyeur*/
  
  strcpy(message,"deposer brute table");
  if(mq_send(messageQueueMachine[numero_machine],message,sizeMessage,0)!=0)
  {
    erreur("envoie du message a la file de message robot al: ",95);
  }
  
  tm.tv_sec += 50;/* timer se declanchera dans 20 secondes */
  nr = mq_receive(messageQueueMachine[numero_machine],messRec, attr.mq_msgsize, NULL, timer);
  if (nr == -1)
  {
    erreur("erreur de reception de message (messageQueueRobotAl) : ",94);
  }
  /*si on ne reçois pas de message on envoie le signal USR2 au thread th_Dialogue*/
  if(messRec == NULL)
  {
    printf("la machine numero %d n'a pas fini de retirer la pièce du convoyeur après 50 secondes/n", numero_machine);
    threadID = pthread_self();
    pthread_kill(SIGUSR2,thIdDialog);
  }
  /*sinon le thread reois la fin du de pot piece brute*/

 
 
}


/*************************************************************
 * 
 * Fonction random sortant un numeros de piece entre 0 et 2
 * 
 * 
 * ***********************************************************/
  /* On suppose a<b*/
int rand_a_b(int a, int b)
{
    return rand()%(b-a) +a;
}


int saisir_ordre_operateur()
{
  srand(time(NULL));
  return rand_a_b(0,2);
}
/*************************************************************
 * 
 * Fonction faisant la correspondance entre le code de la piece
 * en parametre et la machine qui doit traiter la piece 
 * 
 * ***********************************************************/
int correspondance_machine_code(int code_piece)
{
  return code_piece;
}
/*************************************************************/


/*************************************************************
 * 
 * Fonction retournant le pid du thread créé et prenant en 
 * parametre le code de la piece et le numero de la machine
 * 
 * ***********************************************************/
pthread_t creer_thread(int code_piece,int numero_machine)
{
  pthread_t new_thread;
  data param;
  param.piece = code_piece;
  param.machine = numero_machine;
  
  if(pthread_create(&new_thread,NULL,th_piece,data) != 0)
  {
    erreur("erreur creation de thread :", 95);
  }
  return new_thread;
}
/*************************************************************/