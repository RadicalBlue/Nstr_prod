#include "robot_retrait.h"

//#define TIMEOUT 30 /* timeout pour le retrait du signal */
#define TEMPSRETRAIT 5 /* temps de retrait de la piece usinee sur le convoyeur */

//#define DEFAILLANCE SIGUSR2
//#define PIECERETIREE SIGUSR1

//pid_t sender;
extern mqd_t messageQueueRobotRe;

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
//void fnc_evenementielle_timerRR();

/* Fonction d'attente de signal
 */
static void receive_sig(union sigval sv);


/* CODE */

void * th_Robot_retrait()
{
	//struct sigaction act;

	//memset (&act, '\0', sizeof(act));

	/* Use the sa_sigaction field because the handles has two additional parameters */
	//act.sa_sigaction = &receive_sig;

	/* the SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	//act.sa_flags = SA_SIGINFO;

	//if (sigaction(SIGUSR1, &act, NULL) < 0)
	//	erreur("sigaction robot retrait", 1);

	//mqd_t mqdes;
	struct sigevent not;

	not.sigev_notify = SIGEV_THREAD;
	not.sigev_notify_function = receive_sig;
	not.sigev_notify_attributes = NULL;
	not.sigev_value.sival_ptr = &messageQueueRobotRe;   /* Arg. to thread func. */
	if (mq_notify(messageQueueRobotRe, &not) == -1) {
		perror("mq_notify");
		exit(EXIT_FAILURE);
	}

	while (1);
}

void retirer_pc_usn()
{
	char * msg = "retirer\0";

	//alarm(TIMEOUT);
	//signal(SIGALRM, fnc_evenementielle_timerRR);
	//armer_timer(timer_RR, 30 sec);
	retirer_piece_usinee_du_convoyeur();
	//alarm(0);
	//desarmer_timer(timerRR);
	//kill(sender, PIECERETIREE);
	mq_send(messageQueueRobotRe, msg, strlen(msg), (unsigned int) 0);
	//send(fin_retrait_piece_usinee_du_convoyeur, X);
}

void retirer_piece_usinee_du_convoyeur()
{
	printf("retrait de la piece usinee\n");
	sleep(TEMPSRETRAIT);
	printf("Piece usinee retiree\n");
}

//void fnc_evenementielle_timerRR()
//{
	//kill(sender, DEFAILLANCE);
	//erreur("arret du robot de retrait, car l'operation de retrait de piece ne s'est pas terminee avant 30 sec.", 2);
//}

static void receive_sig(union sigval sv)
{
	//sender = (pid_t)siginfo->si_pid;
	struct mq_attr attr;
	ssize_t nr;
	void *buf;
	mqd_t mqdes = *((mqd_t *) sv.sival_ptr);


	/* Determine maximum msg size; allocate buffer to receive msg */


	if (mq_getattr(mqdes, &attr) == -1) {
		perror("mq_getattr");
		exit(EXIT_FAILURE);
	}
	buf = malloc(attr.mq_msgsize);


	if (buf == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}


	nr = mq_receive(mqdes, buf, attr.mq_msgsize, NULL);
	if (nr == -1) {
		perror("mq_receive");
		exit(EXIT_FAILURE);
	}

	free(buf);
	retirer_pc_usn();
}
