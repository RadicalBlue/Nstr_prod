/* les messages envoye par les threads pieces sont les suivants:
 * "deposer brute conv\0" 
 * "deposer usine conv\0"
 * "deposer brute table\0"
 * "retirer usine conv\0"
 * "fin usinage piece\0"
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
extern pthread_mutex_t mutexEtat; /*id du mutex des etat pour assurer que deux threads ne change pas l'etat d'une machine en meme temps*/
extern pthread_t threadID; /*identifiant du thread ayant envoye le signal SIGUSR2 pour signaler un anomalie lors du traitement d'une piece par une machine*/
extern pthread_t thIdDialog;/*id du thread dialog*/
extern mqd_t messageQueueRobotAl; /*identifiant de la file de message utilise par les threads pieces et le thread robot alimentation*/
extern mqd_t messageQueueRobotRe; /*identifiant de la file de message utilise par les threads pieces et le thread robot retrait*/
extern mqd_t messageQueueMachine[NBRMACHINE]; /*identifiant de la file de message utilise par les threads pieces et les threads machine*/
/***************************************************************************************************************************************************/

bool machineEnPanne[NBRMACHINE] = {false};
Liste listeThreadPiece;
int code_piece, numero_machine;
bool etat;

typedef struct s_mydata{
  int piece;
  int machine;
} Data;

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
  printf("signal SIGUSR1 a ete recu\n");
  
  while(!isEmpty(listeThreadPiece))
  {
    pthread_t th = pullElement(listeThreadPiece);
    
    /*printf("element enleve %lX\n",(unsigned long)th);*/
    
    listeThreadPiece = removeFirst(listeThreadPiece);
    
    printf("envoi du signal kill au thread %lX\n",(long)th);
    
    pthread_kill(th,SIGKILL);
  }
  destroyList(listeThreadPiece);
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
  printf("signal SIGUSR2 a ete recu\n");
  
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
  printf("dialogue : start\n");
  
  int i;
  listeThreadPiece = creatList();
  
  /*test*/
  /*if(isEmpty(listeThreadPiece))
    printf("liste initialiser et vide\n");*/
  
  
  /*initialisation des mutex*/
  for(i = 0 ; i < NBRMACHINE; i++)
  {
    pthread_mutex_init (&mutexMachine[i],NULL);
  }
  
  pthread_mutex_init (&mutexConvoyeur,NULL);
  
  
 /*******************protection du signal SIGUSR1**************************************************/
  struct sigaction act1;
 
  memset (&act1, '\0', sizeof(struct sigaction));
  act1.sa_handler = &fnc_evenementielle_USER1;
  if (sigaction(SIGUSR1, &act1, NULL) < 0)
  {
    erreur("erreur sigaction   ",96);
  }
  
 /************************************************************************************************/
 /*******************protection du signal SIGUSR2**************************************************/
  struct sigaction act2;
 
  memset (&act2, '\0', sizeof(struct sigaction));

  act2.sa_handler = &fnc_evenementielle_USER2;

  if (sigaction(SIGUSR2, &act2, NULL) < 0)
  {
    erreur("erreur sigaction   ",96);
  }
  
 /************************************************************************************************/
  
  while(1)
  {
    code_piece = saisir_ordre_operateur();
    
    numero_machine = correspondance_machine_code(code_piece);
    
    pthread_mutex_lock(&mutexConvoyeur);
    
    etat = machineEnPanne[numero_machine];
    
    pthread_mutex_unlock(&mutexConvoyeur);
    
    if(etat == true)
    {
      printf("thDialog : machine demandee en panne\n");
      
      while(!isEmpty(listeThreadPiece))
      {
	/*attente active*/
      }
      
      erreur("FIN DU SYSTEM : etat defaillance detecte\n",96);
    }
    
    else
    {
      pthread_t new_thread = creer_thread(code_piece,numero_machine);
      
      listeThreadPiece = addInList(listeThreadPiece,new_thread);
      /*mapList(listeThreadPiece);*/
    }
    
    sleep(10);
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
  printf("piece %lX : start\n", (long) pthread_self());
  
  char message[20]; /* message envoye par le thread piece.*/
  char def[20];/*vairable pour tester les messages recu*/
  char messRec[50]; /*message recu par le thread piece*/
  /*struct mq_attr attr;structure permettant de recevoir les attributs du message dans la file*/
  ssize_t bitRecu; /*nombre de bit recu*/
  struct timespec timer;
  clock_gettime(CLOCK_REALTIME, &timer); /*initialisation du timer*/
  size_t sizeMessage = 20;
  struct s_mydata * p_data = (Data*) param_data;
  int piece = p_data->piece;
  int machine = p_data->machine;
 
  printf("piece %lX : demande mutex machine %d / piece %d\n", (long) pthread_self(), machine, piece);
  
  if(pthread_mutex_lock(&mutexMachine[machine])!=0)
  {
    erreur("piece : erreur de verouillage du mutex machine   ",96);
  }
  
  if(pthread_mutex_lock(&mutexConvoyeur)!=0)
  {
    erreur("piece : erreur de verouillage du mutex convoyeur   ",96);
  }
  
  strcpy(message, "deposer brute conv\0");
  
  printf("piece %lX : j'envoie le message depose la piece brute sur le convoyeur\n",(long)pthread_self());
  
  if(mq_send(messageQueueRobotAl,message,strlen(message),0)!=0)
  {
    erreur("envoie du message a la file de message robot al  ",95);
  }
  
  //messRec=malloc(attr.mq_msgsize+1);
  printf("piece %lX : j'attends de recevoir la confirmation du robot alimentation\n",(long)pthread_self());
  
  timer.tv_sec += 20;/* timer se declanchera dans 20 secondes */
  /*sleep(2);*/
  bitRecu = mq_timedreceive(messageQueueRobotAl,messRec, 50, NULL, &timer);
  
  if (bitRecu == -1)
  {
    erreur("piece : erreur de reception de message (messageQueueRobotAl)   ",94);
  }
  
  printf("piece %lX :j'ai recu la confirmation du robot alimentation\n",(long)pthread_self());
  
  sprintf(def,"defaillance");/*test pour savoir si le message recu est un message de defaillance*/
 
  /*si on ne reçois pas de message ou un message de defaillance on envoie le signal USR1 au thread th_Dialogue*/
  
  if(messRec == NULL || strcmp((char*)messRec,def)==0)
  {
    printf("piece %lX : arret du system de supervision : le robot d'alimentation ne répond pas ou n'a pas pu retirer la piece au bout de 20 secondes\n",(long)pthread_self());
    pthread_kill(thIdDialog,SIGUSR1);
  }
  
  /*Sinon le thread reçois fin de depot sur convoyeur*/
  sprintf(message,"deposer brute table");
  
  printf("piece %lX : j'envoie le message depose la piece brute sur la table\n",(long)pthread_self());
  
  if(mq_send(messageQueueMachine[numero_machine],message,sizeMessage,0)!=0)
  {
    erreur("piece : envoie du message a la file de message table  ",95);
  }
  
  //messRec=malloc(attr.mq_msgsize+1);
  printf("piece %lX : j'attends de recevoir la confirmation de la machine%d\n",(long)pthread_self(),numero_machine);
  
  timer.tv_sec += 50;/* timer se declanchera dans 50 secondes */
  
  bitRecu =mq_timedreceive(messageQueueMachine[numero_machine],messRec, 50, NULL, &timer);
  
  if (bitRecu == -1)
  {
    erreur("piece : erreur de reception de message (messageQueueRobotAl)   ",94);
  }
  
  /*si on ne reçois pas de message on envoie le signal USR2 au thread th_Dialogue*/
  printf("piece %lX : j'ai recu la confirmation de la machine%d\n",(long)pthread_self(),numero_machine);
  
  if(messRec == NULL)
  {
    printf("piece %lX : la machine numero %d n'a pas fini de retirer la pièce du convoyeur après 50 secondes\n",(long)pthread_self(), numero_machine);
    threadID = pthread_self();
    pthread_kill(thIdDialog,SIGUSR2);
  }
  
  /*sinon le thread recois la fin du de pot piece brute sur table*/
  if(pthread_mutex_unlock(&mutexConvoyeur)!=0) /*liberation du mutex convoyeur*/
  {
    erreur("piece : erreur de verouillage du mutex convoyeur   ",96);
  }
  
  //messRec=malloc(attr.mq_msgsize+1);
  printf("piece %lX : j'attends que la machine%d finisse de travailler\n",(long)pthread_self(),numero_machine);
  
  timer.tv_sec += 600;/* timer se declanchera dans 10 minutes */
  
  bitRecu =mq_timedreceive(messageQueueMachine[numero_machine],messRec, 50, NULL, &timer);
  
  if (bitRecu == -1)
  {
    erreur("piece : erreur de reception de message (messageQueueRobotAl)   ",94);
  }
  
  printf("piece %lX : j'ai recu la confirmation de fin d'usinage par la machine%d\n",(long)pthread_self(),numero_machine);
  
  if(messRec == NULL)
  {
    pthread_mutex_lock(&mutexEtat);
    machineEnPanne[numero_machine] = true;
    pthread_mutex_unlock(&mutexEtat);
    printf("piece %lX : la machine numero %d n'a pas fini de retirer la pièce du convoyeur après 50 secondes\n",(long)pthread_self(), numero_machine);
    threadID = pthread_self();
    pthread_kill(thIdDialog,SIGUSR2);
  }
  
  /*sinon il recois fin usinage*/
  pthread_mutex_lock(&mutexConvoyeur);
  
  sprintf(message,"deposer usine conv");
  
  printf("piece %lX : j'envoie le message depose la piece usinee sur le convoyeur\n",(long)pthread_self());
  
  if(mq_send(messageQueueMachine[numero_machine],message,sizeMessage,0)!=0)
  {
    erreur("piece : envoie du message a la file de message table  ",95);
  }
  
  //messRec=malloc(attr.mq_msgsize+1);
  printf("piece %lX : j'attends de recevoir la confirmation de depot de la machine%d\n",(long)pthread_self(),numero_machine);
  
  timer.tv_sec += 30;/* timer se declanchera dans 30 secondes */
  
  bitRecu =mq_timedreceive(messageQueueMachine[numero_machine],messRec, 50, NULL, &timer);
  
  if (bitRecu == -1)
  {
    erreur("piece : erreur de reception de message (messageQueueRobotAl)   ",94);
  }
  
  printf("piece %lX : j'ai recu la confirmation de depot de la machine%d\n",(long)pthread_self(),numero_machine);
  
  if(messRec == NULL)
  {
    printf("piece %lX : la machine numero %d n'a pas depose la pièce usine sur le convoyeur après 30 secondes\n",(long)pthread_self(), numero_machine);
    pthread_kill(thIdDialog,SIGUSR1);
  }
  
  /*sinon le thread recois fin de depot piece piece usine sur convoyeur*/
  sprintf(message,"retirer usine conv");
  
  printf("piece %lX : j'envoie le message retirer la piece usinee sur le convoyeur\n",(long)pthread_self());
  
  if(mq_send(messageQueueRobotRe,message,sizeMessage,0)!=0)
  {
    erreur("piece : envoie du message a la file de message robot retrait  ",95);
  }
  
  //messRec=malloc(attr.mq_msgsize+1);
  printf("piece %lX : j'attends de recevoir la confirmation du robot de retrait\n",(long)pthread_self());
  
  timer.tv_sec += 30;/* timer se declanchera dans 30 secondes */
  
  bitRecu =mq_timedreceive(messageQueueRobotRe,messRec, 50, NULL, &timer);
  
  if (bitRecu == -1)
  {
    erreur("piece : erreur de reception de message (messageQueueRobotAl)   ",94);
  }
  
  printf("piece %lX : j'ai recu la confirmation du robot retrait\n",(long)pthread_self());
  
  if(messRec == NULL)
  {
    printf("piece %lX : arret du system de supervision : le robot d'alimentation ne répond pas ou n'a pas pu retirer la piece au bout de 20 secondes\n",(long)pthread_self());
    pthread_kill(thIdDialog,SIGUSR1);
  }
  
  /*Sinon: le thread recois fin retrait piece usinee du convoyeur*/
  pthread_mutex_unlock(&mutexConvoyeur);
  pthread_mutex_unlock(&mutexMachine[numero_machine]);
  pthread_kill(thIdDialog,SIGUSR2);
  
  printf("piece %lX : Usinage de la piece %d : OK \n",(long)pthread_self(),code_piece);
  
  pthread_exit(0);
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
  Data * param = (Data *) malloc(sizeof(struct s_mydata));;
  param->piece = code_piece;
  param->machine = numero_machine;
  
  printf("code piece %d, numero machine %d \n",code_piece,numero_machine);
  /*printf("code piece dans le param %d, numero machine dans le param %d \n",param->piece,param->machine);*/
  
  if(pthread_create(&new_thread, NULL, &th_piece, (void *) param) != 0)
  {
    erreur("erreur creation de thread  ", 95);
  }
  
  return new_thread;
}
/*************************************************************/
