#include "machine.h"

//#define PIECEBRUT SIGUSR1 /* fin de depot de la piece brut sur la table */
//#define PIECEUSINEE SIGUSR1 /* fin depot piece usinee sur convoyeur */
//#define FINUSINAGE SIGUSR2 /* fin usinage de la piece */

#define TEMPSUSINAGE 5
#define TEMPSRETRAIT 5
#define TEMPSDEPOSE 5

//pid_t sender;
extern pthread_t machine[4];
int i_th;


/* FONCTIONS */

/* Fonction qui lance l'usinage de base de la piece
 */
void usinage();

/* Fonction d'usinage de la piece
 */
void usinage_pc();

/* Fonction qui depose la piece usinee sur le convoyeur
 */
void deposer_piece();

/* Fonction qui depose la piece usiné sur le convoyeur
 */
void depot();

/* fonction d'attente de signal
 * Lance une fonction selon le signal reçu
 */
void receive_sig(int sig, siginfo_t *siginfo, void * context);

/* Fonction qui retire la piece du convoyeur
 */
void retirer_piece_du_convoyeur();

/* CODE */

void * th_Machine()
{
	pthread_t moi = pthread_self();

	i_th = 0;

	while (machine[i_th] != moi)
		i_th++;

	//struct sigaction act;

	//memset (&act, '\0', sizeof(act));

	/* Use the sa_sigaction field because the handles has two additional parameters */
	//act.sa_sigaction = &receive_sig;

	/* the SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	//act.sa_flags = SA_SIGINFO;

	//if (sigaction(SIGUSR1, &act, NULL) < 0)
	//	erreur("sigaction1 machine", 1);
	//if (sigaction(SIGUSR2, &act, NULL) < 0)
	//	erreur("sigaction2 machine", 1);
	
	struct sigevent not;

	not.sigev_notify = SIGEV_THREAD;
	not.sigev_notify_function = receive_sig;
	not.sigev_notify_attributes = NULL;
	not.sigev_value.sival_ptr = &TROLOLO_TODO;
	if (mq_notify(TROLOLO_TODO, &not) == -1) {
		perror("mq_notify");
		exit(EXIT_FAILURE);
	}

	while (1);
}

void usinage_pc()
{
	char * depotUsnTbl = "deposer brute table\0";
	char * depotUsnCnv = "deposer usine conv\0";
	retirer_piece_du_convoyeur();
	//kill(sender, PIECEBRUT);
	mq_send(TROLOLO_TODO, depotUsnTbl, strlen(depotUsnTbl), (unsigned int) 0);
	usinage();
	//kill(sender, FINUSINAGE);
	mq_send(TROLOLO_TODO, depotUsnCnv, strlen(depotUsnCnv), (unsigned int) 0);
}

void usinage()
{
	printf("Usinage de la piece\n");
	sleep(TEMPSUSINAGE);
	printf("Piece usinee\n");
}

void retirer_piece_du_convoyeur()
{
	printf("Retrait de la piece du convoyeur\n");
	sleep(TEMPSRETRAIT);
	printf("Piece retiree du convoyeur\n");
}

void depot()
{
	deposer_piece();
	kill(sender, PIECEUSINEE);
}

void deposer_piece()
{
	printf("depose de la piece\n");
	sleep(TEMPSDEPOSE);
	printf("Piece deposee\n");
}

void receive_sig(int sig, siginfo_t * siginfo, void * context)
{
	sender = (pid_t)siginfo->si_pid;

	switch (sig)
	{
		case (int)SIGUSR1 :
			//receive(deposer_piece_brute_sur_table);
			usinage_pc();
			break;
		case (int)SIGUSR2 :
			//receive(deposer_piece_usinee_sur_convoyeur);
			depot();
	}
}

