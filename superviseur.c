#include "superviseur.h"
#define NBRMACHINE 3


bool machineEnPanne[NBRMACHINE] = false;
pthread_mutex_t mutexMachine[NBRMACHINE];
pthread_mutex_t mutexConvoyeur;
Liste listeThreadPiece;


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

void fnc_evenementielle_USER2(int s, siginfo_t *siginfo)
{
  listeThreadPiece = removeFromList(listeThreadPiece,(pthread_t)siginfo->si_pid);
}

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
    saisir_ordre_operateur(code_piece);
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
      new_thread = creer_thread(code_piece,numero_machine);
      addInList(listeThreadPiece,new_thread);
    }
  }
}