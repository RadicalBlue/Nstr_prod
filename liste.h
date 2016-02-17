#ifndef LISTE_H
#define LISTE_H


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

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
Liste removeFromList(Liste l, char * element);

/*********************************************************
 *
 * Fonction qui ajoute un élément mis en paramètre
 * dans la liste mise en paramètre et retroune la liste
 * avec l'élément mis en tête 
 * 
 ********************************************************/
Liste addInList(Liste l, char * element);

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
 * Procedure détruisant la liste en paramètre
 * 
 ******************************************************/
void destroyList(Liste l);



#endif