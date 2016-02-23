/* Example of using sigaction() to setup a signal handler with 3 arguments
 *  * including siginfo_t.
 *   */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
 
static void hdl (int sig, siginfo_t *siginfo, void *context)
{
	switch (sig)
	{
		case (int)SIGUSR1:
			printf("SIGUSR1 : ");
			break;
		case (int)SIGUSR2:
			printf("SIGUSR2 : ");
	}

	printf ("Sending PID: %ld, UID: %ld\n",
			(long)siginfo->si_pid, 
			(long)siginfo->si_uid);
}
 
int main (int argc, char *argv[])
{
	struct sigaction act;
		 
	memset (&act, '\0', sizeof(act));
			 
	/* Use the sa_sigaction field because the handles has two additional parameters */
	act.sa_sigaction = &hdl;
				 
	/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	act.sa_flags = SA_SIGINFO;
					 
	if (sigaction(SIGUSR1, &act, NULL) < 0) {
		perror ("sigaction");
		return 1;
	}
	if (sigaction(SIGUSR2, &act, NULL) < 0) {
		perror ("sigaction");
		return 2;
	}
						 
	while (1)
		sleep (10);
							 
	return 0;
}
