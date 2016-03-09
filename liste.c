#include "liste.h"


struct s_liste{
  Liste next; /* pointeur sur le reste de la liste */
  pthread_t data;
};


Liste creatList()
{
  Liste l = malloc(sizeof(struct s_liste));
  if (l)
  {
    l->next = NULL;
    l-> data = 0;
  }
  else erreur(": erreur d'allouement d'espace memoire\n", 99);
  return l;
}

bool isEmpty(Liste l)
{
  if(l->data == 0 && l->next == NULL)
    return true;
  return false;
}

Liste addInList(Liste l, pthread_t element)
{
  Liste newlist = creatList();
  
  newlist->next = l;  
  newlist->data = element;
  return newlist;  
}

Liste removeFirst(Liste l)
{
  if(isEmpty(l)|| l == NULL)
    erreur("La liste est vide ! on ne peut pas faire de removeFirst ",75);
  Liste temp = l;
  l = l->next;
  free(temp);
  return l;
}


void destroyList(Liste l)
{
  while(l)
    l = removeFirst(l);
}

pthread_t pullElement(Liste l)
{
  if(isEmpty(l)|| l == NULL)
    erreur("La liste est vide ! on ne peut pas faire de pull",77);
  return l->data;
}

Liste removeFromList(Liste l, pthread_t element)
{
  Liste temp = l->next;
  
  while(pthread_equal(temp->data,element)== 0 && temp->data != 0)
  {
    temp = temp->next;
  }
  
  temp = removeFirst(temp); /* effacement de l'élément mis en paramètre*/
  
  Liste newlist = creatList();
  
  while(pthread_equal(temp->data,element)== 0)
  {
    newlist = addInList(newlist,l->data);
    l = l->next;
  }
  while(temp)
  {
    newlist = addInList(newlist,temp->data);
    temp = temp->next;
  }
  return newlist;
}


void mapList(Liste l)
{
  while(!isEmpty(l))
  {
    printf("%lX\n",(unsigned long)l->data);
    l = l->next;    
  }  
}