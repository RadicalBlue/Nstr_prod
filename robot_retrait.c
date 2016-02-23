#include "robot_retrait.h"

#define TIMEOUT 30 /* timeout pour le retrait du signal */
#define TEMPSRETRAIT 5 /* temps de retrait de la piece usinee sur le convoyeur */

#define DEFAILLANCE SIGUSR2
#define PIECERETIREE SIGUSR1

pid_t sender;

/* FONCTIONS */

/* Fonction principale
 * Treald
 */
void * th_Robot_retrait();

/* Fonction retrait basic de la piece usinee du convoyeur
 */
void retirer_piece_usinee_du_convoyeur();

/* Fonction qui retire la piece usinee du convoyeur
 */
void retirer_pc_usn();

/* Fonction fin timer
 * envoie un signal de defaillance
 */
void fnc_evenementielle_timerRR();

/* Fonction d'attente de signal
 */
void reveive_sig(int sig, siginfo_t * siginfo, void * context); 


/* CODE */

void * th_Robot_retrait()
{
	struct sigaction act;

	memset (&act, '\0', sizeof(act));

	/* Use the sa_sigaction field because the handles has two additional parameters */
	act.sa_sigaction = &receive_sig;

	/* the SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	act.sa_flags = SA_SIGINFO;

	if (sigaction(SIGUSR1, &act, NULL) < 0)
		erreur("sigaction robot retrait", 1);

	while (1);
}

void retirer_pc_usn()
{
	alarm(TIMEOUT);
	signal(SIGALRM, fnc_evenementielle_timerRR);
	//armer_timer(timer_RR, 30 sec);
	retirer_piece_usinee_du_convoyeur();
	alarm(0);
	//desarmer_timer(timerRR);
	kill(sender, PIECERETIREE);
	//send(fin_retrait_piece_usinee_du_convoyeur, X);
}

void retirer_piece_usinee_du_convoyeur()
{
	printf("retrait de la piece usinee\n");
	sleep(TEMPSRETRAIT);
	printf("Piece usinee retiree\n");
}

void fnc_evenementielle_timerRR()
{
	kill(sender, DEFAILLANCE);
	erreur("arret du robot de retrait, car l'operation de retrait de piece ne s'est pas terminee avant 30 sec.", 2);
}

void receive_sig(int sig)
{
	sender = (pid_t)siginfo->si_pid;
	retirer_pc_usn();
}
