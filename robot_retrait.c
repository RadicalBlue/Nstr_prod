#include "robot_retrait.h"


/* Fonction d'attente de signal
 */
void receive_sig(int sig) {retirer_pc_usn();}

/* Fonction qui retire la piece usinee du convoyeur
 */
void retirer_pc_usn();

void * th_Robot_retrait()
{
	while (1);
}

void retirer_pc_usn()
{
	X = sender;
	armer_timer(timer_RR, 30 sec);
	retirer_piece_usinee_du_convoyeur();
	desarmer_timer(timerRR);
	send(fin_retrait_piece_usinee_du_convoyeur, X);
}

void fnc_evenementielle_timerRR()
{
	send(defaillance, X);
	exit("arret du robot de retrait, car l'operation de retrait de piece ne s'est pas terminee avant 30 sec.");
}
