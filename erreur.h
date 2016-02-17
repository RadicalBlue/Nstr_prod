#ifndef ERREUR_H
#define ERREUR_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>


/**************************************************
 *
 * Procedure affichant un message mess et
 * faisant un exit avec le code d'erreur code
 * 
 * ************************************************/
void erreur(char* mess, int code);

#endif
