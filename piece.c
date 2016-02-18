#include "piece.h"

void * th_Piece(int code_piece, int num_machine)
{
	/* DECLARATIONS */
	Mutex mutex_machine[numero_machine];
	Mutex mutex_convoyeur;
	Mutex mutex1;

	char * message;
	int numero_machine;
	int epsilon

	/* ALGO */
	pthread_mutex_lock(mutex_machine[numero_machine]);
	pthread_mutex_lock(mutex_convoyeur);
	send(deposer_piece_brute_sur_convoyeur, th_Robot_alimentation);
	timed_receive(message, 20 sec + epsilon);

	if (message == NULL || message == "defaillance")
	{
		pthread_kill(th_Dialogue, SIGUSR1);
		erreur("arret du systeme de supervision : le robot d'alimentation ne repond pas ou bien il n'a pas pu retirer la piece au bout de 20 sec.", 11);
	}

	/* Sinon, le thread a recu fin_depot_brut_sur_convoyeur */
	send(deposer_piece_brute_sur_table, th_Machine[numero_machine]);
	timed_receive(message, 50 sec + epsilon);
	if (message == NULL)
	{
		pthread_kill(th_Dialogue, SIGUSR1);
		fprintf(stderr, "arret du systeme de supervision : la machine %d n'a pas fini de retirer la piece du convoyeur apres 50 sec.", numero_machine);
		exit(12);
	}

	/* Sinon : le thread a recu fin_depot_piece_brute_sur_table */
	pthread_mutex_unlock(mutex_convoyeur);
	timed_receive(message, 10 min);
	if (message == NULL)
	{
		pthread_mutex_lock(mutex1);
		machineEnPanne[numero_machine] = true;
		pthread_mutex_unlock(mutex1);
		pthread_kill(th_Dialogue, SIGUSR2);
		fprintf(stderr, "Machine %d en panne; elle n'a pas depose la piece usinee apres 30 sec.", numero_machine);
		exit(13);
	}

	/* Sinon : le thread a recu fin_usinage */
	pthread_mutex_lock(mutex_convoyeur);
	send(deposer_piece_usinee_sur_convoyeur, th_Machine[numero_machine]);
	timed_receive(message, 30 sec);
	if (message == NULL)
	{
		pthread_kill(th_Dialogue, SIGUSR1);
		fprintf(stderr, "La machine %d n'a pas depose la piece usinee apres 30 sec");
		exit(14);
	}

	/* Sinon : le thread a recu fin_depot_piece_usinee_sur_convoyeur */
	send(retirer_piece_usinee_du_convoyeur, th_Robot_retrait);
	timed_receive(message, 30 sec);
	if (message == NULL)
	{
		pthread_kill(th_Dialogue, SIGUSR1);
		erreur("Le robot de retrait n'a pas fini le retrait de la piece usinee apres 30 sec", 15);
	}

	/* Sinon : le thread a recu fin_retrait_piece_usinee_du_convoyeur */
	pthread_mutex_unlock(mutex_convoyeur);
	pthread_mutex_unlock(mutex_machine[numero_machine]);
	pthread_kill(th_Dialogue, SIGUSR2);
	printf("Usinage de la piece %d : OK", code_piece);
}
