#include "superviseur.h"
#include "robot_retrait.h"
#include "robot_alimentation.h"
#include "machine.h"
#include "erreur.h"
#include "liste.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <fcntl.h>


#define NB_MACHINE 3

pthread_t thIdDialog;
pthread_t machine[4];
pthread_t robot_alim;
pthread_t robot_retr;
pthread_t threadID; /*identifiant du thread ayant envoye le signal SIGUSR2 pour signaler un anomalie lors du traitement d'une piece par une machine*/

pthread_mutex_t mutexMachine[NB_MACHINE]; /*id du mutex concernant les machines en fonctionnement*/
pthread_mutex_t mutexConvoyeur; /*id du mutex du convoyeur: si il est utilise ou non*/
pthread_mutex_t mutexEtat; /*id du mutex des etat pour assurer que deux threads ne change pas l'etat d'une machine en meme temps*/


mqd_t messageQueueRobotAl; /*identifiant de la file de message utilise par les threads pieces et le thread robot alimentation*/
mqd_t messageQueueRobotRe; /*identifiant de la file de message utilise par les threads pieces et le thread robot retrait*/
mqd_t messageQueueMachine[NB_MACHINE]; /*identifiant de la file de message utilise par les threads pieces et les threads machine*/

Liste listeThreadPiece;
/* Fonction qui lance tous les threads necessaire au bon fonctionnement du code
 */
void initialisation_mq();
void initialisation_mutex();
void initialisation_thread();
void destruction();

int main()
{
	int i;
	printf("initialisation\n");
	initialisation_mq();
	initialisation_mutex();
	initialisation_thread();
	listeThreadPiece = creatList();
	
	
	pthread_join(thIdDialog,NULL);
	pthread_join(robot_alim,NULL);
	pthread_join(robot_retr,NULL);
	for (i = 0; i < NB_MACHINE; i++)
		  pthread_join(machine[i],NULL);
	
	return 0;
}

void initialisation_thread()
{
	int i;
	if (pthread_create(&thIdDialog, NULL, th_Dialogue, NULL))
		erreur("Impossible de lancer le thread thIdDialog\n", 1);


	for (i = 0; i < NB_MACHINE; i++)
		if (pthread_create(&machine[i], NULL, th_Machine, NULL))
			erreur("impossible de lancer un thread machine\n", 2);


	if (pthread_create(&robot_alim, NULL, th_Robot_alimentation, NULL))
		erreur("impossible de lancer le thread robot alimentation\n", 3);

	if (pthread_create(&robot_retr, NULL, th_Robot_retrait, NULL))
		erreur("impossible de lancer le thread robot retrait\n", 4);
}
void initialisation_mutex()
{
  	int i;
  	if(pthread_mutex_init(&mutexConvoyeur,NULL))
		erreur("erreur d'initialisation du mutex convoyeur\n", 5);
	for (i = 0; i < NB_MACHINE; i++)
		if (pthread_mutex_init(&mutexMachine[i], NULL))
			erreur("erreur d'initialisation du mutex de la machine\n", 6);
	if(pthread_mutex_init(&mutexEtat,NULL))
		erreur("erreur d'initialisation du mutex de etat\n", 7);
}
void initialisation_mq()
{
	int i;
  	messageQueueRobotAl = mq_open("/messageQueueRobotAl",O_CREAT | O_RDWR,0666,NULL);
	if (messageQueueRobotAl == (mqd_t) -1)
		erreur("erreur d'initialisation de la file de message avec le robot alimentation\n", 8);
	messageQueueRobotRe = mq_open("/messageQueueRobotRe",O_CREAT | O_RDWR,0666,NULL);
	if (messageQueueRobotRe == (mqd_t) -1)
		erreur("erreur d'initialisation de la file de message avec le robot retrait\n", 9);
	
	for (i = 0; i < NB_MACHINE; i++)
	{
		char nomQ[15];
		sprintf(nomQ,"/QueueMachine%d",i);
		messageQueueMachine[i] = mq_open(nomQ,O_CREAT | O_RDWR,0666,NULL);
		if (messageQueueMachine[i] == (mqd_t) -1)
		erreur("erreur d'initialisation de la file de message avec les machine\n", 10);
	}
}
void destruction()
{
	int i;
	for (i = 0; i < NB_MACHINE; i++)
	{
		char nomQ[15];
		sprintf(nomQ,"/QueueMachine%d",i);
		if (mq_unlink(nomQ) == -1)
		erreur("erreur de suppression de la file de message avec les machine\n", 11);
	}
	if (mq_unlink("/messageQueueRobotAl") == -1)
		erreur("erreur de suppression de la file de message avec les machine\n", 12);
	if (mq_unlink("/messageQueueRobotAl") == -1)
		erreur("erreur de suppression de la file de message avec les machine\n", 13);
	if(pthread_mutex_destroy(&mutexConvoyeur))
		  erreur("erreur de destruction du mutex convoyeur",14);
	if(pthread_mutex_destroy(&mutexEtat))
		  erreur("erreur de destruction du mutex des etat",15);
	for (i = 0; i < NB_MACHINE; i++)
		if (pthread_mutex_destroy(&mutexMachine[i]))
			erreur("erreur de destruction du mutex machine\n", 6);
}
