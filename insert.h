#ifndef _INSERT_
#define _INSERT_
typedef struct node
{
	char mdp[LENPWD];
	struct node *next;
}node;

extern int critereVoyelles;
extern int occurenceVoyelles;
extern int occurenceConsonnes;
extern node** head;

int count_consonants(char* monString);

int count_vowels(char* monString);

int insert(node **head, char val[]);

int insertInList(char* mdp);

void* insert_mdp();

#endif
