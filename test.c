#include "superviseur.h"
#include "robot_retrait.h"
#include "robot_alimentation.h"
#include "machine.h"
#include "liste.h"
#include "erreur.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>



int main()
{
  /* test de la liste */
  pthread_t th_test1;
  pthread_t th_test2;
  pthread_t th_test3;
  pthread_t th_test4;
  
  Liste testList = creatList();
  printf("liste vide creee\n");
  
  if(isEmpty(l))
    printf("la liste est bien vide\n");
  else 
  {
    printf("erreur liste non vide !\n") ;
    exit(99);
  }
  
  testList = addInList(testList, th_test1);
  printf("ajout d'un element dans la liste\n");
  
  printf("th ajoute : %ld\n",testList->data);
  
  destoyList(testList);
  return 0;
}