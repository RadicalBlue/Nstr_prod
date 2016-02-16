#include "erreur.h"


void erreur(char* mess, int code)
{
 	perror(mess);
 	exit(code);
}
