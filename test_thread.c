#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

pthread_t pid_th;

void * th1();
void hdl_th1(int sig, siginfo_t * siginfo, void * context);

int main()
{
	if (pthread_create(&pid_th, NULL, th1, NULL))
	{
		fprintf(stderr, "Impossible de lancer le thread\n");
		exit(1);
	}

	printf("thread principale OK -> pid : %d / pid_th : %ld\n", getpid(), (long)pid_th);

	sleep(2);

	printf("signal à th1\n");

	sleep(5);

	pthread_kill(pid_th, SIGUSR1);

	printf("Fin thread principale\n");

	return 0;
}

void * th1()
{
	/*struct sigaction act;
	memset(&act, '\0', sizeof(act));
	act.sa_sigaction = &hdl_th1;
	act.sa_flags = SA_SIGINFO;
	if (sigaction(SIGUSR1, &act, NULL) < 0)
	{
		fprintf(stderr, "pb sigaction\n");
		exit(11);
	}
	*/
	sigset_t set;

	printf("thread 1 lancé -> pid : %d\n", getpid());
	pthread_sigwait(&set, SIGUSR1);
	printf("signal reçu");
	while (1);

}

void hdl_th1(int sig, siginfo_t * siginfo, void * context)
{
	printf("signal reçu provenant de : %ld\n", (long)siginfo->si_pid);
}
