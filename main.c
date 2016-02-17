#include "superviseur.h"
#include "robot_retrait.h"
#include "robot_alimentation.h"
#include "machine.h"
#include "erreur.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#define NB_MACHINE 3

pthread_t dialogue;
pthread_t machine[4];
pthread_t robot_alim;
pthread_t robot_retr;

/* Fonction qui lance tous les threads necessaire au bon fonctionnement du code
 */
void initialisation();

int main()
{
	initialisation();

	return 0;
}

void initialisation()
{
	if (pthread_create(&dialogue, NULL, th_Dialogue, NULL))
		erreur("Impossible de lancer le thread dialogue\n", 1);


	for (int i = 0; i < NB_MACHINE; i++)
		if (pthread_create(&machine[i], NULL, th_Machine, NULL))
			erreur("impossible de lancer un thread machine\n", 2);


	if (pthread_create(&robot_alim, NULL, th_Robot_alimentation, NULL))
		erreur("impossible de lancer le thread robot alimentation\n", 3);

	if (pthread_create(&robot_retr, NULL, th_Robot_retrait, NULL))
		erreur("impossible de lancer le thread robot retrait\n", 4);
}
