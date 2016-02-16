#include <signal.h>
#include <time.h> 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "myTimer.h"

/* fonction appelee par le signal SIGALRM */
void timer(int s)
{
	signal(SIGALRM,timer);
	/********************************************************************/
	/*                                                                  */
	/*	      fait pour le test                                     */
	/*                                                                  */
		      printf("timer fini\n");                              
	/********************************************************************/
	
	//break;
}


void setTimer(int valTime)
{
   /* protection du signal SIGALRM 
    * pour qu'il soit redirige a la fonction timer
    */
  signal(SIGALRM,timer);
  
  alarm(valTime);
  
}

void stopTimer()
{
  alarm(0);  
}
