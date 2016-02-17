#include "superviseur.h"
#define NBRMACHINE 3


bool machineEnPanne[NBRMACHINE] = false;
pthread_mutex_t mutexMachine[NBRMACHINE];
pthread_mutex_t mutexConvoyeur;


void * th_Dialogue()
{
  int i;
  /*initialisation des mutex*/
  for(i = 0 ; i < NBRMACHINE; i++)
  {
    pthread_mutex_init (&mutexMachine[i],NULL);
  }
  
  pthread_mutex_init (&mutexConvoyeur,NULL);
  
  /** TODO: sigaction avec un siginfo bien défini pour avoir le pid de celui qui envoie le signal **/
   /** TODO: faire une liste de char* avec des fonction d'ajout et de suppression **/
  
}