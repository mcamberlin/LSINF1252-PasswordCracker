// Includes
#include <stdio.h> 		// pour utiliser fopen(), fputs(), fclose()
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>  		// pour utiliser read(), close()
#include <sys/types.h> 		// pour utiliser open()
#include <sys/stat.h> 		// pour utiliser open()
#include <fcntl.h>   		// pour utiliser open()
#include <semaphore.h> 		// pour les sémaphores
#include <pthread.h>  		// pour les threads 
#include "sha256.h"
#include "reverse.h"
#include "variables.h"
#include "insert.h"
#include "lectureFichier.h"
#include "reverse_hash.h"




/*--------------------------------------------------------------*/

/** La fonction count_consonants() compte le nombre de consommes dans une chaine de caractères
	@pre - @monString = un pointeur vers une chaîne de caractères dont on souhaite compter le nombre de consonnes
	@post - retourne le nombre de consonnes dans @monString

*/
int count_consonants(char* monString)
{
	int nbreConsonnes = 0;
	int j=0;
	for(int i=0;*(monString+i) !='\0';i++) // tant que on a pas vérifié chaque lettre de @monString
	{
		j=0;
		while(j<20 && *(monString+i) != CONSONNES[j]) // tant que on a pas vérifié toutes les consonnes de @CONSONNES
		{
			j++;
		}
		if(j<20 && *(monString+i) == CONSONNES[j]) // j<20 sinon on risque de dépasser la longueur du tableau @CONSONNES
		{
			nbreConsonnes++;
		}
	}
	return nbreConsonnes;
}



/*--------------------------------------------------------------*/

/** La fonction freeLinkedList() libère la mémoire associée à la liste chaînée passée en argument
	@pre 	- @head = un pointeur vers le pointeur head. Si @head == NULL, retourne -1
	@post 	- 0 si la suppression de la liste chaînée s'est réalisée avec succès, -1 sinon
*/
int freeLinkedList(node **head)
{
	if(head == NULL)
	{
		fprintf(stderr, "**head non spécifié dans freeLinkedList() \n");
		return -1;
	}
	if(*head == NULL) // cas où la liste est vide
	{
		return 1;
	}
	else // cas où la liste n'est pas vide
	{
		node* runner = *head; // noeud courant
		node* previous=runner;	// noeud précédent

		while(runner !=NULL)
		{
			previous = runner;
			runner = runner->next;
			free(previous);			
		}
		
		*head = NULL;
		return 0;
	}
}

/** La fonction printList() affiche les mdp contenus dans la liste chainée soit sur la sortie standard, soit dans un fichier @fichierSortie 
	@pre - @head = un pointeur vers le pointeur head. Si @head == NULL, retourne -1
	@post - 0 si l'affichage de la liste chaînée s'est réalisée avec succès, -1 sinon

*/
int printList(node** head)
{
	if(head == NULL) 
	{
		fprintf(stderr, "**head non spécifié dans printList() \n");
		return -1;
	}
	if(*head == NULL) // cas où la liste est vide
	{
		fprintf(stderr, "La liste chaînée de mdp est vide \n");
		return 0;
	}

	if(sortieStandard == 1) // cas où il faut écrire sur la sortie standard
	{
		node* runner = *head;
		while(runner != NULL)
		{
			printf("%s \n",runner->mdp);
			runner = runner->next;
		}
	}
	else // cas où il faut écrire dans le fichier @fichierSortie
	{

		printf("Début printList() dans le cas ou il faut ecrire dans un fichier de sortie \n");

		FILE* fichier = fopen(fichierSortie, "w+");
		if(fichier == NULL) // cas où @fopen() a planté
		{
			printf("Erreur dans l'ouverture du fichier: \n");
			return -1;
		}

		node* runner = *head;

		while(runner != NULL)
		{

			fputs(runner->mdp,fichier);
			fputs(&RETOUR_LIGNE,fichier);
			//printf("%s \n",runner->mdp);
			runner = runner->next;
		}

		fclose(fichier);
		if(fclose(fichier) !=0)
		{
			fprintf(stderr, "Erreur fermeture dans printList()\n");
		    	return -1;
		}
		
	}
	
	freeLinkedList(head);
	
	printf("Fin printList() \n");
	return 0;
}	

/*--------------------------------------------------------------*/

/** La fonction count_vowels() compte le nombre de voyelles dans une chaine de caractères
	@pre - @monString = un pointeur vers une chaîne de caractères dont on souhaite compter le nombre de voyelles
	@post - retourne le nombre de voyelles dans @monString

*/
int count_vowels(char* monString)
{
	int nbreVoyelles = 0;
	int j=0;
	for(int i=0;*(monString+i) != '\0';i++)// tant que on a pas vérifié chaque lettre de @monString
	{
		j=0;
		while(j<6 && *(monString+i) != VOYELLES[j]) // tant que on a pas vérifié toutes les voyelles
		{
			j++;
		}
		if(j<6 && *(monString+i) == VOYELLES[j]) // j<6 sinon on risque de dépasser la longueur du tableau @VOYELLES
		{
			nbreVoyelles++;
		}
	}
	return nbreVoyelles;
}


/*--------------------------------------------------------------*/

/** La fonction insert() insére dans une simple liste chainée, en tête de liste, un élément.
	@pre 	- @head = un pointeur vers le premier noeud de la liste chainée. Si @head == NULL, retourne -1
		- @value = un tableau de caractère représentant le mdp à insérer dans la simple liste chainée
	@post 	- 0 si l'insertion dans la liste chaînée s'est réalisée avec succès, -1 sinon
*/
int insert(node **head, char val[]) 
{
	if(head == NULL)
	{
		fprintf(stderr, "**head non spécifié dans insert() \n");
		return -1;
	}

	if(*head == NULL) // cas où la liste est vide
	{
		node* newNode = (node*) malloc(sizeof(node)); 
		if(newNode == NULL)
		{
			fprintf(stderr, "Erreur allocation de mémoire newNode dans le cas d'une liste vide dans insert() \n");
			return -1;
		}

		strcpy(newNode->mdp,val); // copie dans @newNode la chaine de caractères @val 
		newNode->next = NULL;
		*head = newNode;
		return 0;
	}
	else // cas où la liste n'est pas vide
	{
		// Insertion en tête de liste
        	node* newNode = (node*) malloc(sizeof(node)); 
        	if(newNode == NULL)
		{
			fprintf(stderr, "Erreur allocation de mémoire de newNode dans le cas d'une liste NON vide dans insert() \n");
			return -1;
		}
		strcpy(newNode->mdp,val); // copie dans @newNode la chaine de caractères @val 
        	newNode->next = *head;
        	*head = newNode;
        	return 0;
	}    
}
/** La fonction insertInList() compare le nombre d'occurences soit de voyelles, soit de consonnes dans la chaîne de caractères @mdp avec le nombre d'occurences maximal mis à jour au fur et à mesure en variable globale. 
Si le nbre d'occurences de voyelles ou consonnes est respectivement supérieur à @occurenceVoyelles, @occurenceConsonnes, alors, la liste chainée actuelle est libérée et une nouvelle est créée pour y placer @mdp.
Si le nbre d'occurences de voyelles ou consonnes est respectivement égal @occurenceVoyelles, @occurenceConsonnes, alors, la liste chainée actuelle est maintenue et @mdp est inséré en tête de liste.
Si le nbre d'occurences de voyelles ou consonnes est respectivement inférieur @occurenceVoyelles, @occurenceConsonnes, alors, rien est effectué.

	@pre - @head = un pointeur vers le pointeur head. Si @head == NULL, retourne -1
	@post - 0 si l'ajout de @mdp dans la liste chaînée s'est réalisé avec succès, -1 sinon

*/
int insertInList(char* mdp)
{
	if(critereVoyelles == 1) // cas où le critère de sélection des mdp sont les voyelles
	{
		int vowels = count_vowels(mdp);
		if(vowels==occurenceVoyelles) // cas où le mdp contient le même nombre de voyelles que les précédents
		{
			// Insertion du mdp en tête dans la liste chainée
			if(insert(head,mdp) ==-1)
			{
				return -1;
			}

		}
		if(vowels>occurenceVoyelles)
		{
			occurenceVoyelles = vowels;
			if(freeLinkedList(head) ==-1) // Libération de toute la liste chainée
			{
				return -1;
			}

			if(insert(head,mdp) ==-1) 
			{
				return -1;
			}

		}
		//if(vowels<occurenceVoyelles)// cas ou le mdp contient moins de voyelles que les précédents
		//{ ne pas ajouter le mdp }


	}
	else // cas où le critère de sélection des mdp sont les consonnes
	{
		int consonants = count_consonants(mdp);
		if(consonants==occurenceConsonnes) // cas où le mdp contient le même nombre de consonnes que les précédents
		{
			// Insertion du mdp en tête dans la liste chainée
			if(insert(head,mdp) ==-1)
			{
				return -1;
			}
		}
		if(consonants>occurenceConsonnes)
		{
			occurenceConsonnes = consonants;
			if(freeLinkedList(head) ==-1)// Libération de toute la liste chainée
			{
				return -1;
			}

			if(insert(head,mdp) ==-1)// Insertion du mdp en tête dans la liste chainée
			{
				return -1;
			}

		}
		//if(consonants<occurenceConsonnes)// cas ou le mdp contient moins de consonnes que les précédents
		//{ ne pas ajouter le mdp }
	}
	return 0;
}

/** La fonction insert_mdp() prend un un mot de passe en clair contenu dans la ressource @tab_mdp et l'insère dans la liste simple liste chainée.
	@pre - 
	@post - retourne EXIT_SUCESS si l'ajout s'est réalisé avec succès, EXIT_FAILURE sinon
*/
void* insert_mdp()
{
	while(!fin_de_lecture || nbreSlotHashRempli || nbreSlotMdpRempli || CalculExecution) 
/* tant que la lecture des fichiers binaires n'est pas finie ou 
que il y a encore des hashs dans la ressource @tab_hash ou 
que il y a encore des mdp en clair dans la ressource @tab_mdp ou
que des threads sont en cours d'exécution
*/
	{
		printf("nbreSlotHashRempli : %d, nbreSlotMdpRempli: %d, CalculExecution : %d \n",nbreSlotHashRempli,nbreSlotMdpRempli,CalculExecution);
		char* mdp;

		sem_wait(&full_mdp);
		pthread_mutex_lock(&mutex_mdp);

		printf("section critique insert_mdp\n");

		int conditionArret=0; // Condition nécessaire pour arrêter de parcourir le tableau une fois qu'un slot rempli a été trouvé

		for(int i=0; i<N && !conditionArret; i++)
		{
			if(*(tab_mdp+i)!=NULL) // cas où la case est remplie
			{
				conditionArret = 1;
				mdp = (char*) *(tab_mdp+i);
				//free(*(tab_mdp+i));
				*(tab_mdp+i)=NULL;
				nbreSlotMdpRempli--;
			}
		}

		pthread_mutex_unlock(&mutex_mdp);
		sem_post(&empty_mdp);

		printf("le mot de passe à insérer dans tab_mdp est %s\n", mdp);

		if(insertInList(mdp)==-1)
		{
			fprintf(stderr, "Erreur insertion mot de passe dans tab_mdp\n");
			return (void*) EXIT_FAILURE;
		}
	}
	printf("Fin insert_mdp() \n");
	return EXIT_SUCCESS;
}


