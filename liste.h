#ifndef LISTE_H
#define LISTE_H


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "erreur.h"

typedef struct s_liste * Liste;

/***********************************************
 *
 * Fonction qui crée une structure liste vide
 * 
 ***********************************************/
Liste creatList();



/******************************************************
 *
 * Fonction qui enlève un élément mis en paramètre
 * de la liste mise en paramètre et retroune la liste
 * sans l'élément
 * 
 ******************************************************/
Liste removeFromList(Liste l, pthread_t element);

/*********************************************************
 *
 * Fonction qui ajoute un élément mis en paramètre
 * dans la liste mise en paramètre et retroune la liste
 * avec l'élément mis en tête 
 * 
 ********************************************************/
Liste addInList(Liste l, pthread_t element);

/************************************************
 *
 * Fonction retournant vrai si la liste est vide
 * ou faux si elle ne l'est pas
 * 
 ************************************************/
bool isEmpty(Liste l);

/******************************************************
 *
 * Fonction qui enlève le premier élément 
 * de la liste mise en paramètre et retroune la liste
 * sans l'élément
 * 
 ******************************************************/
Liste removeFirst(Liste l);

/******************************************************
 *
 * Fonction qui enlève le premier élément 
 * de la liste mise en paramètre et retroune l'élément
 * 
 ******************************************************/
pthread_t pullElement(Liste l);

/******************************************************
 *
 * Procedure détruisant la liste en paramètre
 * 
 ******************************************************/
void destroyList(Liste l);

/******************************************************
 *
 * Procedure affichant la liste en paramètre
 * 
 ******************************************************/
void mapList(Liste l);

#endif