#include <stdio.h>

//Constantes
const char CONSONNES[] = {'b','c','d','f','g','h','j','k','l','m','n','p','q','r','s','t','v','w','x','z'};
const char [] = {'a','e','i','o','u','y'};
int occurenceVoyelles=0;
int occurenceConsonnes=0;

typedef struct node{
    char mdp[LENPWD];
    struct node *next;
} node;

//Création de la liste simplement chainée:
node** head;
 

/** La fonction count_consonants() compte le nombre de consommes dans une chaine de caractères
	@pre - monString = un pointeur vers une chaîne de caractères dont on souhaite compter le nombre de consonnes
	@post - retourne le nombre de consonnes dans monString

*/
int count_consonants(char* monString)
{
	int nbreConsonnes = 0;
	int j=0;	
	for(int i=0;*(monString+i) !='\0';i++) // tant que on a pas vérifié chaque lettre de monString
	{
		j=0;
		while(j<20 && *(monString+i) != consonnes[j])
		{
			j++;	
		}
		if(*(monString+i) == consonnes[j])
		{
			nbreConsonnes++;
		}
	}
	return nbreConsonnes;
}

/** La fonction count_vowels() compte le nombre de voyelles dans une chaine de caractères
	@pre - monString = un pointeur vers une chaîne de caractères dont on souhaite compter le nombre de voyelles
	@post - retourne le nombre de voyelles dans monString

*/
int count_vowels(char* monString)
{
	int nbreVoyelles = 0;
	int j=0;
	for(int i=0;*(monString+i) != '\0';i++)// tant que on a pas vérifié chaque lettre de monString
	{
		j=0;
		//printf("*(monString+i) : %c \n",*(monString+i));
		while(j<6 && *(monString+i) != voyelles[j])
		{
			//printf(" voyelles[j] : %c , *(monString+i) : %c \n",voyelles[j],*(monString+i));
			j++;
		}
		if(voyelles[j] == *(monString+i))
		{
			nbreVoyelles++;
		}
	}
	return nbreVoyelles;
}



/** La fonction insert() insére dans une simple liste chainée un élément
	@pre 	- @head = un pointeur vers le premier noeud de la liste chainée. Si @head == NULL, retourne -1
		- @value = un tableau de caractère représentant le mdp à insérer dans la liste chainée
	@post 	- 0 si l'insertion dans la liste chaînée s'est réalisée avec succès, -1 sinon
*/
int insert(node_t **head, char val[]) 
{
	// Si aucun argument 
	if(head == NULL) 
	{
		return -1;
	}

	// Si la liste est vide
	if(*head == NULL) 
	{
		// Ajouter le nouveau élément
		// Définition du nouveau noeud
		node* newNode = (node*) malloc(sizeof(node)); 
		if(newNode == NULL)
		{
			return -1;
		}
		
		strcpy(newNode->mdp,val); // copie dans le nouveau noeud la chaine de caractères val
		newNode->next = NULL;
		*head = newNode;
		return 0;
	}

	// Si la liste n'est pas vide
	else
	{
		// Insertion en tête de liste
		// Définition du nouveau noeud
        	node* newNode = (node*) malloc(sizeof(node)); 
        	if(newNode == NULL)
		{
			return -1;
		}
		strcpy(newNode->mdp,val); // copie dans le nouveau noeud la chaine de caractères val 
        	newNode->next = *head;          
        	*head = newNode; 
        	return 0;
	}    
}

/** La fonction insert() insére dans une simple liste chainée un élément
	@pre 	- @head = un pointeur vers le premier noeud de la liste chainée. Si @head == NULL, retourne -1
		- @value = un tableau de caractère représentant le mdp à insérer dans la liste chainée
	@post 	- 0 si l'insertion dans la liste chaînée s'est réalisée avec succès, -1 sinon
*/
int insert(node **head, char val[]) 
{
	// Si aucun argument 
	if(head == NULL) 
	{
		return -1;
	}

	// Si la liste est vide
	if(*head == NULL) 
	{
		// Ajouter le nouveau élément
		// Définition du nouveau noeud
		node* newNode = (node*) malloc(sizeof(node)); 
		if(newNode == NULL)
		{
			return -1;
		}
		
		strcpy(newNode->mdp,val); // copie dans le nouveau noeud la chaine de caractères val
		newNode->next = NULL;
		*head = newNode;
		return 0;
	}

	// Si la liste n'est pas vide
	else
	{
		// Insertion en tête de liste
		// Définition du nouveau noeud
        	node* newNode = (node*) malloc(sizeof(node)); 
        	if(newNode == NULL)
		{
			return -1;
		}
		strcpy(newNode->mdp,val); // copie dans le nouveau noeud la chaine de caractères val 
        	newNode->next = *head;          
        	*head = newNode; 
        	return 0;
	}    
}

/** La fonction free() libère la liste chaînée associée à sa tête passée en argument
	@pre 	- @head = un pointeur vers le pointeur head. Si @head == NULL, retourne -1
	@post 	- 0 si la suppression de la liste chaînée s'est réalisée avec succès, -1 sinon
*/
int free(node **head) 
{
	if(head == NULL)
	{
		return -1;
	}

	if(*head == NULL) // si la liste est vide
	{
		return 1;
	}
	
	node* runner = *head;
	node* previous;

	while(runner !=NULL)
	{
		previous = runner;
		runner = runner->next;		
		free(previous);	
	}
	*head = NULL;
	return 0;
}













int main(int argc, char *argv[]) 
{
	// tests
	char monString1[] = "fermier";
	int nbreVoyelles1 =  count_vowels(monString1);
	int nbreConsonnes1 = count_consonants(monString1);

	// Création de la liste chainée
	head = (node**) malloc(sizeof(node*)); // head est un pointeur vers la tete de la liste chaînée
	if(head == NULL)
	{
		fprintf(stderr, "Erreur malloc allocation mémoire pour head\n");
		return EXIT_FAILURE;
	}
      
    


	printf("Il y a %d voyelles dans %s \n",nbreVoyelles1,monString1);
	printf("Il y a %d consonnes dans %s \n",nbreConsonnes1,monString1);
}


/*
		else// cas ou le critère de sélection des mdp sont les consonnes
		{
			if(count_consants(mdp)==occurenceConsonnes) // cas ou le mdp contient le meme nombre de consonnes que les occurences précédentes
			{
				// Insert dans la liste chainée
			}
			else if(count_consants(mdp)>occurenceConsonnes)
			{
				occurenceConsonnes = count_consants(mdp);
				// 1.Libérer toutes la liste chainée 
					//1.1 tester la valeur de retour pour vérifier qu'il n'y ait pas d'erreur
				// 2.Insérer le nouveau mdp
					//2.1 tester la valeur de retour pour vérifier qu'il n'y ait pas d'erreur
			}
			else // cas ou le mdp contient moins de consonnes que les précédents
			{
				// ignorer
			}
			
		}
*/


