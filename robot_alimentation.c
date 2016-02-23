#include "robot_alimentation.h"

pid_t sender;

/* Fonction d'attente de signal
 */
void reveive_sig(int sig, siginfo_t * siginfo, void * context) 
{
	sender = (pid_t)siginfo->si_pid;	
	deposer_pc_brt();
}

/* Fonction qui deposela piece brut sur le convoyeur
 */
void deposer_pc_brt();

void * th_Robot_alimentation()
{
	struct sigaction act;

	memset (&act, '\0', sizeof(act));

	/* Use the sa_sigaction field because the handles has two additional parameters */
	act.sa_sigaction = &receive_sig;

	/* the SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	act.sa_flags = SA_SIGINFO;

	if (sigaction(SIGUSR1, &act, NULL) < 0)
		erreur("sigaction robot alim", 1);

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
