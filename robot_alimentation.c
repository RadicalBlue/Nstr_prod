#include "robot_alimentation.h"

#define TIMEOUT 20 /* timeout pour le depot du signal */
#define TEMPSDEPOT 5 /* temps de depose de la piece brut sur le convoyeur */

#define DEFAILLANCE SIGUSR2
#define PIECEDEPOSEE SIGUSR1

pid_t sender;

/* FONCTIONS */

/* Fonction principale
 * Thread
 */
void * th_Robot_alimentation();

/* Fonction basic de depot de piece brut sur le convoyeur
 */
void deposer_piece_brute();

/* Fonction qui deposela piece brut sur le convoyeur
 * avec gestion de letat de defaillance
 */
void deposer_pc_brt();

/* Fonction fin timer
 * envoie un signal de defaillance
 */
void fnc_evenementielle_timerRA();

/* Fonction d'attente de signal
 */
void reveive_sig(int sig, siginfo_t * siginfo, void * context); 

/* CODE */

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
	kill(sender, DEFAILLANCE);
	erreur("arret du robot d'alimentation, car l'operation de depot de piece a dure plus de 20 sec.", 2);
}

void deposer_pc_brt()
{
	alarm(TIMEOUT);
	signal(SIGALRM, fnc_evenementielle_timerRA);
	//armer_timer(timer_RA, 20 sec);
	deposer_piece_brute();
	alarm(0);
	//desarmer_timer(timerRA);
	kill(sender, PIECEDEPOSEE);
	//send(fin_depot_piece_brute_sur_convoyeur, X);
}

void deposer_piece_brute()
{
	printf("Depose de la piece brut sur le convoyeur\n");
	sleep(TEMPSDEPOT);
	printf("Piece brut deposee sur le convoyeur\n");
}

void reveive_sig(int sig, siginfo_t * siginfo, void * context) 
{
	sender = (pid_t)siginfo->si_pid;	
	deposer_pc_brt();
}
