#include <stdlib.h>
#include "pbwt.h"

DICT *
dictCreate (int size)
{
    DICT *dict = malloc(sizeof(DICT));

    for (dict->dim = 10, dict->size = 1024; dict->size < size; ++dict->dim, dict->size *= 2);
    dict->table = mycalloc (dict->size * sizeof(int));
    dict->names = malloc(dict->size/2 * sizeof(char *));
    return dict; 
}

int
dictDestroy (DICT *dict)
{
  int i = 0;
  for (i = 1; i <= dict->max; ++i)
    free (dict->names[i]);
  free (dict->names);
  free (dict->table);
  free (dict);
  return 0;
}

char *
dictName (DICT *dict, int i)
{
    return dict->names[i+1];
}
