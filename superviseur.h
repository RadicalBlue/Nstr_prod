#ifndef SUPERVISEUR_H
#define SUPERVISEUR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#include "liste.h"
#include "erreur.h"
/*************************************************************
 * 
 * Fonction random sortant un numeros de piece entre 0 et 2
 * 
 * 
 * ***********************************************************/
int saisir_ordre_operateur();


/*************************************************************
 * 
 * Fonction faisant la correspondance entre le code de la piece
 * en parametre et la machine qui doit traiter la piece 
 * 
 * ***********************************************************/
int correspondance_machine_code(int code_piece);

/*************************************************************
 * 
 * Fonction retournant le pid du thread créé et prenant en 
 * parametre le code de la piece et le numero de la machine
 * 
 * ***********************************************************/
pthread_t creer_thread(int code_piece,int numero_machine);

#endif