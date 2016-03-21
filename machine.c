#include "machine.h"

//#define PIECEBRUT SIGUSR1 /* fin de depot de la piece brut sur la table */
//#define PIECEUSINEE SIGUSR1 /* fin depot piece usinee sur convoyeur */
//#define FINUSINAGE SIGUSR2 /* fin usinage de la piece */

#define TEMPSUSINAGE 5
#define TEMPSRETRAIT 5
#define TEMPSDEPOSE 5
#define NBRMACHINE 3

//pid_t sender;
extern pthread_t machine[4];
extern mqd_t messageQueueMachine[NBRMACHINE];
extern pthread_mutex_t mutexQueueMachine[NBRMACHINE]; /*id du mutex concernant les fils de message entre les machine et les pieces*/

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
static void receive_sig(union sigval sv);

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

	printf("machine %d : start\n", i_th + 1);

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
	char nomM[30];
	sprintf(nomM,"machine%d : mq_notify",i_th);
	
	struct sigevent not;
	
	if(pthread_mutex_lock(&mutexQueueMachine[i_th])!=0)
	{
	  erreur("piece : erreur de verouillage de la fil de massage machine   ",91);
	}
	not.sigev_notify = SIGEV_THREAD;
	not.sigev_notify_function = receive_sig;
	not.sigev_notify_attributes = NULL;
	not.sigev_value.sival_ptr = &messageQueueMachine[i_th];
	if (mq_notify(messageQueueMachine[i_th], &not) == -1) {
		perror(nomM);
		exit(EXIT_FAILURE);
	}
	if(pthread_mutex_unlock(&mutexQueueMachine[i_th])!=0)
	{
	  erreur("piece : erreur de verouillage de la fil de massage machine   ",90);
	}

	while (1);
}

void usinage_pc()
{
	char * depotUsnTbl = "deposer brute table\0";
	char * finUsn = "fin usinage piece\0";
	retirer_piece_du_convoyeur();
	//kill(sender, PIECEBRUT);
	if(pthread_mutex_lock(&mutexQueueMachine[i_th])!=0)
	{
	  erreur("piece : erreur de verouillage de la fil de massage machine   ",91);
	}
	mq_send(messageQueueMachine[i_th], depotUsnTbl, strlen(depotUsnTbl), (unsigned int) 0);
	if(pthread_mutex_unlock(&mutexQueueMachine[i_th])!=0)
	{
	  erreur("piece : erreur de déverouillage de la fil de massage machine   ",90);
	}
	usinage();
	//kill(sender, FINUSINAGE);
	if(pthread_mutex_lock(&mutexQueueMachine[i_th])!=0)
	{
	  erreur("piece : erreur de verouillage de la fil de massage machine   ",91);
	}
	mq_send(messageQueueMachine[i_th], finUsn, strlen(finUsn), (unsigned int) 0);
	if(pthread_mutex_unlock(&mutexQueueMachine[i_th])!=0)
	{
	  erreur("piece : erreur de déverouillage de la fil de massage machine   ",90);
	}
}

void usinage()
{
	printf("machine %d : Usinage de la piece\n", i_th + 1);
	sleep(TEMPSUSINAGE);
	printf("machine %d : Piece usinee\n", i_th + 1);
}

void retirer_piece_du_convoyeur()
{
	printf("machine %d : Retrait de la piece du convoyeur\n", i_th + 1);
	sleep(TEMPSRETRAIT);
	printf("machine %d : Piece retiree du convoyeur\n", i_th + 1);
}

void depot()
{
	char * msg = "depose usine conv\0";
	deposer_piece();
	//kill(sender, PIECEUSINEE);
	if(pthread_mutex_lock(&mutexQueueMachine[i_th])!=0)
	{
	  erreur("piece : erreur de verouillage de la fil de massage machine   ",91);
	}
	mq_send(messageQueueMachine[i_th], msg, strlen(msg), (unsigned int) 0);
	if(pthread_mutex_unlock(&mutexQueueMachine[i_th])!=0)
	{
	  erreur("piece : erreur de déverouillage de la fil de massage machine   ",90);
	}
}

void deposer_piece()
{
	printf("machine %d : depose de la piece\n", i_th + 1);
	sleep(TEMPSDEPOSE);
	printf("machine %d : Piece deposee\n", i_th + 1);
}

static void receive_sig(union sigval sv)
{
	//sender = (pid_t)siginfo->si_pid;
	struct mq_attr attr;
	ssize_t nr;
	void * buf;
	char * msg;
	mqd_t mqdes = *((mqd_t *) sv.sival_ptr);

	if (mq_getattr(mqdes, &attr) == -1) {
		perror("machine : mq_getattr");
		exit(EXIT_FAILURE);
	}
	buf = malloc(attr.mq_msgsize);

	if (buf == NULL) {
		perror("machine : malloc");
		exit(EXIT_FAILURE);
	}
	
	if(pthread_mutex_lock(&mutexQueueMachine[i_th])!=0)
	{
	  erreur("piece : erreur de verouillage de la fil de massage machine   ",91);
	}

	nr = mq_receive(mqdes, buf, attr.mq_msgsize, NULL);
	if (nr == -1) {
		perror("machine : mq_receive");
		exit(EXIT_FAILURE);
	}
	
	if(pthread_mutex_unlock(&mutexQueueMachine[i_th])!=0)
	{
	  erreur("piece : erreur de déverouillage du mutex de la fil de massage machine   ",90);
	}
	msg = (char *) buf;

	
	switch (msg[8])
	{
		case 'b' :
			//receive(deposer_piece_brute_sur_table);
			usinage_pc();
			break;
		case 'u' :
			//receive(deposer_piece_usinee_sur_convoyeur);
			depot();
	}
}

