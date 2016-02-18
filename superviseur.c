#include "superviseur.h"
#define NBRMACHINE 3


bool machineEnPanne[NBRMACHINE] = false;
pthread_mutex_t mutexMachine[NBRMACHINE];
pthread_mutex_t mutexConvoyeur;

void fnc_evenementielle_USER1()
{
  
}

void fnc_evenementielle_USER2()
{
  
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
  
  
  Liste listeThreadPiece = creatList();
  /** TODO: sigaction avec un siginfo bien défini pour avoir le pid de celui qui envoie le signal **/
  
  /** TODO:faire le sigaction **/
  
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