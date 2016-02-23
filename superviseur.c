#include "superviseur.h"
#define NBRMACHINE 3


bool machineEnPanne[NBRMACHINE] = false;
pthread_mutex_t mutexMachine[NBRMACHINE];
pthread_mutex_t mutexConvoyeur;
Liste listeThreadPiece;
int code_piece, numero_machine;
bool etat;


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
void fnc_evenementielle_USER1(int s, siginfo_t *siginfo)
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
void fnc_evenementielle_USER2(int s, siginfo_t *siginfo)
{
  listeThreadPiece = removeFromList(listeThreadPiece,(pthread_t)siginfo->si_pid);
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

  /* Use the sa_sigaction field because the handles has two additional parameters */
  act.sa_sigaction = &fnc_evenementielle_USER1;

  /* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
  act.sa_flags = SA_SIGINFO;

  if (sigaction(SIGUSR1, &act1, NULL) < 0)
  {
    erreur("erreur sigaction : ",96);
  }
  
 /************************************************************************************************/
 /*******************protection du signal SIGUSR2**************************************************/
  struct sigaction act2;
 
  memset (&act2, '\0', sizeof(struct sigaction));

  /* Use the sa_sigaction field because the handles has two additional parameters */
  act.sa_sigaction = &fnc_evenementielle_USER2;

  /* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
  act.sa_flags = SA_SIGINFO;

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
/*************************************************************/



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

pthread_t creer_thread(int code_piece,int numero_machine)
{
  /*TODO: retourner l'id du thread cree*/
}