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

