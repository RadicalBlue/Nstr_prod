#ifndef ROBOT_RETRAIT_H
#define ROBOT_RETRAIT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include <mqueue.h>

#include "liste.h"
#include "erreur.h"

void * th_Robot_retrait();

#endif
