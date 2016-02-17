#ifndef LISTE_H
#define LISTE_H


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct s_liste * Liste;

Liste * creatList();

Liste * removeFromList(Liste l, char * element);

Liste * addInList(Liste l, char * element);

bool isEmpty(Liste l);

Liste * removeFirst(Liste l);





#endif