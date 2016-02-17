#include "liste.h"


struct s_liste {
  Liste next; /* pointeur sur le reste de la liste */
  char *data
};


Liste creatList()
{
  Liste l = malloc(sizeof(s_list));
  if (l)
  {
    l->next = NULL;
    l-> data = NULL;
  }
}

bool isEmpty(Liste l)
{
  if(l->data == NULL && l->next == NULL)
    return true;
  return false;
}

Liste addInList(Liste l, char * element)
{
  Liste newlist = creatList();
  
  newlist->next = l;
  newlist->data = element;
  
  return newlist;  
}

Liste removeFirst(Liste l)
{
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


Liste removeFromList(Liste l, char * element)
{
  Liste temp = l->next;
  
  while(temp->data != element && temp->data != NULL)
  {
    temp = temp->next;
  }
  
  temp = removeFirst(temp); /* effacement de l'élément mis en paramètre*/
  
  Liste newlist = creatList();
  
  while(l->data != element)
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