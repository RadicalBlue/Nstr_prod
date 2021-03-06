#include "robot_alimentation.h"

//#define TIMEOUT 20 /* timeout pour le depot du signal */
#define TEMPSDEPOT 5 /* temps de depose de la piece brut sur le convoyeur */

//#define DEFAILLANCE SIGUSR2
//#define PIECEDEPOSEE SIGUSR1

//pid_t sender;
extern mqd_t messageQueueRobotAl;

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
//void fnc_evenementielle_timerRA();

/* Fonction d'attente de signal
 */
static void receive_sig(union sigval sv);

/* CODE */

void * th_Robot_alimentation()
{
	printf("Robot alimentation : start\n");
	//struct sigaction act;

	//memset (&act, '\0', sizeof(act));

	/* Use the sa_sigaction field because the handles has two additional parameters */
	//act.sa_sigaction = &receive_sig;

	/* the SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	//act.sa_flags = SA_SIGINFO;

	//if (sigaction(SIGUSR1, &act, NULL) < 0)
	//	erreur("sigaction robot alim", 1);
	
	//mqd_t mqdes;
	struct sigevent not;

	not.sigev_notify = SIGEV_THREAD;
	not.sigev_notify_function = receive_sig;
	not.sigev_notify_attributes = NULL;
	not.sigev_value.sival_ptr = &messageQueueRobotAl;
	if (mq_notify(messageQueueRobotAl, &not) == -1) {
		perror("robot_al : mq_notify");
		exit(EXIT_FAILURE);
	}
	

	while (1);
}

//void fnc_evenementielle_timerRA()
//{
//	kill(sender, DEFAILLANCE);
//	erreur("arret du robot d'alimentation, car l'operation de depot de piece a dure plus de 20 sec.", 2);
//}

void deposer_pc_brt()
{
	char * msg = "depose\0";

	//alarm(TIMEOUT);
	//signal(SIGALRM, fnc_evenementielle_timerRA);
	//armer_timer(timer_RA, 20 sec);
	deposer_piece_brute();
	//alarm(0);
	//desarmer_timer(timerRA);
	//kill(sender, PIECEDEPOSEE);
	mq_send(messageQueueRobotAl, msg, strlen(msg), (unsigned int) 0);
	//send(fin_depot_piece_brute_sur_convoyeur, X);
}

void deposer_piece_brute()
{
	printf("Robot alimentation : Depose de la piece brut sur le convoyeur\n");
	sleep(TEMPSDEPOT);
	printf("Robot alimentation : Piece brut deposee sur le convoyeur\n");
}

static void receive_sig(union sigval sv) 
{
	printf("Robot alimentation : message recu!\n");
	//sender = (pid_t)siginfo->si_pid;	
	struct mq_attr attr;
	ssize_t nr;
	void * buf;
	mqd_t mqdes = *((mqd_t *) sv.sival_ptr);


	if (mq_getattr(mqdes, &attr) == -1) {
		perror("Robot_al : mq_getattr");
		exit(EXIT_FAILURE);
	}
	buf = malloc(attr.mq_msgsize);

	if (buf == NULL) {
		perror("Robot_al : malloc");
		exit(EXIT_FAILURE);
	}

	nr = mq_receive(mqdes, buf, attr.mq_msgsize, NULL);
	if (nr == -1) {
		perror("Robot_al : mq_receive");
		exit(EXIT_FAILURE);
	}

	free(buf);
	deposer_pc_brt();
}
