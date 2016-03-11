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
  if(isEmpty(l) || l == NULL)
    erreur("La liste est vide ! on ne peut pas faire de removeFirst ",75);
  Liste temp = l;
  l = l->next;
  free(temp);
  return l;
}


void destroyList(Liste l)
{
  printf("destruction de la liste...\n");
  
  if(isEmpty(l))
  {
    free(l);
  }
  
  else 
  {
    while(l)
      l = removeFirst(l);
  }
  
  printf("liste detruite\n");
}

pthread_t pullElement(Liste l)
{
  if(isEmpty(l)|| l == NULL)
    erreur("La liste est vide ! on ne peut pas faire de pull",77);
  return l->data;
}

Liste removeFromList(Liste l, pthread_t element)
{
  
//   printf("removeFromList\n");
  
  if(isEmpty(l)|| l == NULL)
    erreur("La liste est vide ! on ne peut pas faire de removeFromList ",75);
  
  Liste temp = l->next;
  
  if(!isEmpty(temp)/*|| temp != NULL*/)
  { 
 
    while(pthread_equal(temp->data,element)== 0 && temp->data != 0)
    {
      temp = temp->next;
    }
    
//     printf("removeFromList : while du temp fin\n");
    
    temp = removeFirst(temp); /* effacement de l'élément mis en paramètre*/
    
//     printf("removeFromList\n");
    
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
  
  return temp;
}


void mapList(Liste l)
{
  if(isEmpty(l)|| l == NULL)
    erreur("La liste est vide ! on ne peut pas faire de map ",75);
  while(!isEmpty(l))
  {
    printf("elements de la liste: %lX\n",(unsigned long)l->data);
    l = l->next;    
  }  
}