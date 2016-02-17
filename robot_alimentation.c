#include "robot_alimentation.h"

/* Fonction d'attente de signal
 */
void reveive_sig(int sig) {deposer_pc_brt();}

/* Fonction qui deposela piece brut sur le convoyeur
 */
void deposer_pc_brt();

void * th_Robot_alimentation()
{
	while (1);
}

void fnc_evenementielle_timerRA()
{
	send(defaillance, X);
	exit("arret du robot d'alimentation, car l'operation de depot de piece a dure plus de 20 sec.");
}

void deposer_pc_brt()
{
	X = sender;
	armer_timer(timer_RA, 20 sec);
	deposer_piece_brute();
	desarmer_timer(timerRA);
	send(fin_depot_piece_brute_sur_convoyeur, X);
}
