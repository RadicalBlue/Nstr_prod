#include "machine.h"

extern pthread_t dialogue;
extern pthread_t machine[4];
extern pthread_t robot_alim;
extern pthread_t robot_retr;

/* Fonction qui lance l'usinage de la piece
 */
void usinage();

/* Fonction qui depose la piece usiné sur le convoyeur
 */
void depot();

/* fonction d'attente de signal
 * Lance une fonction selon le signal reçu
 */
void receive_sig(int sig);

void * th_Machine()
{
	while (1);
}

void usinage()
{
		X = sender; /* Garder l'ID du thread th_piece qui demande l'usinage*/
		retirer_piece_du_convoyeur();
		send(fin_depot_piece_brute_sur_table, X);
		usinage();
		send(fin_usinage, X);
}

void depot()
{
		deposer_piece();
		send(fin_depot_piece_usinee_sur_convoyeur, X);
}

void receive_sig(int sig)
{
	switch (sig)
	{
		case SIGUSR1 :
			//receive(deposer_piece_brute_sur_table);
			usinage();
			break;
		case SIGUSR2 :
			//receive(deposer_piece_usinee_sur_convoyeur);
			depot();
	}
}

