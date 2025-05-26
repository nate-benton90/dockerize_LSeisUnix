/*
	This Program is used to implement the ADT HASH

	The HAS TEMPLATE is an array of size "SIZE" pointers. Each
	element of the array points to a LIST of HNODE (hash node)
	elements.
	Each HNODE element contains an identifier an a pointer to
	specific TYPE element, which is defined for the paramenter
	of the TEMPLATE.

	The basic operations in the HASH type are:

	1.- INSERT(Element of the type pointed to)
	2.- LOOKUP (Element of the type pointed to)
	3.- REMOVE(Element of the type pointed to)

	Generally the elements pointed to by the HASH (each HNODE)
	has an unique identifier that is used to index the HASH table.

*/

#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>

/*
	Implementation of the ADT HASH
*/

typedef struct 	hnode {
  struct hnode *next;
  char *	name;
  char *	data;
} hnode;

typedef struct 	HASH {
  hnode*	p[16];
  int		size;
} HASH;

void print_node(hnode* r)
{
  while (r != 0) {
	printf("(%s,%s) - ", r -> name, r -> data);
	r = r -> next;
  } 
}

void PrintHash(HASH *HashTable)
{
  int i;

  printf("\tThis HASH Table Has %d LISTS\n", HashTable -> size);
  printf("LISTS\t\t List's elements\n\n");
  for (i = 0; i < HashTable -> size ; i++) {
    printf( "hash[%d] = ");
    print_node(HashTable -> p[i]);
    printf(" NULL\n");
  }
}

void InitHash(HASH *HashTable, int size)
{
  int i;

  HashTable -> size = size; 
  /* HashTable -> p = malloc(size*sizeof(*hnode)); */
  for (i = 0; i < HashTable -> size; i++ ) HashTable -> p[i] = NULL;
}

void del_list(hnode *hn)
{
  if (hn -> next != 0)
    del_list(hn -> next);
  free(hn);
}

void empty(HASH *HashTable)
{
  int i;

  for (i = 0; i < HashTable -> size; i++ )
    if (HashTable -> p[i] != 0) 
      del_list(HashTable -> p[i]);
  free(HashTable -> p);
}

int hash_func(char *s1, int n)
{
  int i;

  i = 0;
  while (*s1 != '\0') i = i + *s1++;
  return(i%n);
}

void HashInsert(HASH *HashTable, char *s1, char *s2)
{
  hnode	*temp;
  int	key;

  key = hash_func(s1, HashTable -> size); 	

  temp = (hnode *) malloc(sizeof(hnode));
  temp -> name = malloc(strlen(s1) + 1);
  strcpy(temp -> name, s1);
  temp -> data = (char *) malloc(strlen(s2) + 1);
  strcpy(temp -> data, s2);

  if (HashTable -> p[key] == NULL) {
    HashTable -> p[key] = temp;
    temp -> next = NULL;
  } else {
    temp -> next = HashTable -> p[key];
    HashTable -> p[key] = temp;
  }

} /* end of the subroutine INSERT on hash table */

char * LookUp(HASH *HashTable, char *s1)
{
  hnode	*temp;
  int	key;

  key = hash_func(s1, HashTable -> size); 	

  temp = HashTable -> p[key];
	
  while (temp) {
    if (!strcmp(temp -> name, s1) )
      return (temp -> data);	
    temp = temp -> next;
  }

  return(NULL);
} /* end of the subroutine FIND on hash table */
