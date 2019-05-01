
/*
	Fichier cracker.c réprésente le code dans son intégralité

	Auteurs:
		- CAMBERLIN Merlin
		- PISVIN Arthur
	Version: 01-05-19 - Ajout fonction count_vowels() et count_consonants() + création liste chainée

	Commandes à indiquer dans le shell:
		- cd ~/Documents/LSINF1252-PasswordCracker-Gr118-2019
		- gcc cracker.c reverse.c sha256.c -o cracker -lpthread 
		- ./cracker arg1 arg2 arg3
		- echo -n monString | sha256sum

	Commandes git:
		- git status
		- git add monfichier
		- git commit -m "blablabla" monfichier
		- git commit -i -m "blablabla" monfichier // en cas de fusion de conflits
		- git push
	Remarques:
		- Ne pas oublier de faire les docstring à chaque fois
		- A chaque malloc, ne pas oublier de free en cas d'erreur
		- A chaque initialisation, tester la valeur de retour pour s'assurer du bon fonctionnement
		- Ajuster la version et le commentaire avec
		- fprintf(stderr, "Erreur malloc cas où argument -o spécifié %d\n", errno);
*/

// CONSTANTES dans le préprocesseurs
#define LENPWD 6 // Nbre maximal de caractères dans les mots de passes originels

// Includes
#include <stdio.h>  
#include <stdlib.h>
#include <string.h> // pour utiliser la fonction strstr() semblable à contains()
#include <unistd.h>  // pour utiliser read(), close()
#include <sys/types.h> // pour utiliser open()
#include <sys/stat.h> // pour utiliser open()
#include <fcntl.h>   // pour utiliser open()
#include <ctype.h>  //pour isdigit
#include <semaphore.h> //pour semaphore
#include <pthread.h>  //pour les threads 
#include "sha256.h"
#include "reverse.h"


// Valeurs par défaut
int nbreThreadsCalcul = 1;
int N = 2;			// Le nombre de slot du buffer
int critereVoyelles = 1;	//true
int sortieStandard = 1;		//true
int nbreFichiersEntree = 0;
int fin_de_lecture = 0;       	//false
int nbreSlotHashRempli = 0;

// Déclaration d'un tableau de pointeurs contenant les noms des fichiers d'entrée
char** fichiersEntree; 

// Initialisation du mutex et des 2 sémaphores pour le producteur-consommateur 
pthread_mutex_t mutex_hash;
sem_t empty_hash; 
sem_t full_hash; 


// Déclaration d'une structure représentant un hash  
typedef struct hash
{
	char hash[32]; // 1 hash = 32 bytes et 1char = 1 byte
}hash;

// Déclaration d'un tableau de pointeur de hash
hash** tab_hash;


// Constantes
const char CONSONNES[] = {'b','c','d','f','g','h','j','k','l','m','n','p','q','r','s','t','v','w','x','z'};
const char VOYELLES[] = {'a','e','i','o','u','y'};
int occurenceVoyelles=0;
int occurenceConsonnes=0;

// Déclaration d'une structure représentant un noeud de la simple liste chaînée
typedef struct node
{
    char mdp[LENPWD];
    struct node *next;
}node;

// Déclaration d'une la liste simplement chainée
node** head;


/*--------------------------------------------------------------*/

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
		while(j<20 && *(monString+i) != CONSONNES[j]) // tant que on a pas comparé avec toutes les consonnes
		{
			j++;	
		}
		if(*(monString+i) == CONSONNES[j])
		{
			nbreConsonnes++;
		}
	}
	return nbreConsonnes;
}


/*--------------------------------------------------------------*/

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
		while(j<6 && *(monString+i) != VOYELLES[j]) // tant que on a pas vérifié toutes les voyelles
		{
			j++;
		}
		if(*(monString+i) == VOYELLES[j])
		{
			nbreVoyelles++;
		}
	}
	return nbreVoyelles;
}


/*--------------------------------------------------------------*/

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
		fprintf(stderr, "**head non spécifié dans insert() \n");
		return -1;
	}

	// Si la liste est vide
	if(*head == NULL) 
	{
		// Ajouter le nouveau élément
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

	// Si la liste n'est pas vide
	else
	{
		// Insertion en tête de liste
        	node* newNode = (node*) malloc(sizeof(node)); 
        	if(newNode == NULL)
		{
			fprintf(stderr, "Erreur allocation de mémoire de newNode dans le cas d'une liste NON vide dans insert() \n");
			return -1;
		}
		strcpy(newNode->mdp,val); // copie dans le nouveau noeud la chaine de caractères val 
        	newNode->next = *head;          
        	*head = newNode; 
        	return 0;
	}    
}


/*--------------------------------------------------------------*/

/** La fonction free() libère la liste chaînée associée à sa tête passée en argument
	@pre 	- @head = un pointeur vers le pointeur head. Si @head == NULL, retourne -1
	@post 	- 0 si la suppression de la liste chaînée s'est réalisée avec succès, -1 sinon
*/
int freeLinkedList(node **head) 
{
	if(head == NULL)
	{
		fprintf(stderr, "**head non spécifié dans insert() \n");
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


/*--------------------------------------------------------------*/

/** La fonction affiche_hash est le consommateur du producteur-consommateur
	@pre - 
	@post - lis les fichiers binaires en entrée, lance reversehash(), ajuste la liste chaînée
*/
void* affiche_hash()
{
	while(!fin_de_lecture || nbreSlotHashRempli)
	// Tant que la lecture du fichier n'est pas finie ou que le buffer n'est pas vide.
	{
		printf("nbre de slot rempli = %d\n", nbreSlotHashRempli);
		char* mdp = (char*) malloc(sizeof(char)*LENPWD);
		if(mdp == NULL)
		{
			fprintf(stderr, "Erreur allocation mémoire mdp dans affiche_hash\n");
			return (void*) EXIT_FAILURE;
		}

		uint8_t* hash;

		printf("avant section critique affiche_hash\n");
		sem_wait(&full_hash);
		pthread_mutex_lock(&mutex_hash);
		//début section critique
		int conditionArret = 1; // condition nécessaire pour sortir de la boucle for une fois que le pointeur du slot a ete copié dans hash
		for(int i=0; i<2*nbreThreadsCalcul && conditionArret; i++)
		{
			if(*(tab_hash+i)!=NULL) //si la case est remplie
			{
				hash = (uint8_t*) *(tab_hash+i);
				printf("L'adresse dans tab_hash est : %p à l'indice %d (affiche)\n", *(tab_hash+i), i);
				printf("le hash traité dans reverehash est : %s\n", (*(tab_hash+i))->hash);
				*(tab_hash+i)=NULL;
				nbreSlotHashRempli--;
				conditionArret = 0;
			}
		}

		pthread_mutex_unlock(&mutex_hash);
		sem_post(&empty_hash);

		printf("début reversehash\n");
		if( reversehash(hash, mdp, LENPWD) ) // cas ou reversehash() a trouvé le mdp originel
		{
			printf("\nle hash affiché grace à affiche_hash est %s\n\n", mdp);
			
			/* ---------------------------------------------- */
			// COMPARER LE NOMBRE D'OCCURENCES DU MDP AVEC CEUX DANS LA LISTE CHAINEE

			if(critereVoyelles == 1) // cas ou le critère de sélection des mdp sont les voyelles
			{
				if(count_vowels(mdp)==occurenceVoyelles) // cas ou le mdp contient le meme nombre de voyelles que les précédents
				{
					// Insertion du mdp en tête dans la liste chainée
					if(insert(head,mdp) ==-1)
					{
						return (void*) EXIT_FAILURE;
					}
					
				}
				if(count_vowels(mdp)>occurenceVoyelles)
				{
					occurenceVoyelles = count_vowels(mdp);
					// 1.Libérer toutes la liste chainée
					if(freeLinkedList(head) ==-1)
					{
						
						return (void*) EXIT_FAILURE;
					}

					// Insertion du mdp en tête dans la liste chainée
					if(insert(head,mdp) ==-1)
					{
						return (void*) EXIT_FAILURE;
					}

				}
				//if(count_vowels(mdp)<occurenceVoyelles)// cas ou le mdp contient moins de voyelles que les précédents
				//{ ne pas ajouter le mdp }
				

			}
			else // cas ou le critère de sélection des mdp sont les consonnes
			{
				if(count_consonants(mdp)==occurenceConsonnes) // cas ou le mdp contient le meme nombre de consonnes que les précédents
				{
					// Insertion du mdp en tête dans la liste chainée
					if(insert(head,mdp) ==-1)
					{
						return (void*) EXIT_FAILURE;
					}
					
				}
				if(count_consonants(mdp)>occurenceVoyelles)
				{
					occurenceConsonnes = count_consonants(mdp);
					// 1.Libérer toutes la liste chainée
					if(freeLinkedList(head) ==-1)
					{
						return (void*) EXIT_FAILURE;
					}

					// Insertion du mdp en tête dans la liste chainée
					if(insert(head,mdp) ==-1)
					{
						return (void*) EXIT_FAILURE;
					}

				}
				//if(count_consonants(mdp)<occurenceConsonnes)// cas ou le mdp contient moins de consonnes que les précédents
				//{ ne pas ajouter le mdp }
			}
		}
		else // cas ou reversehash n'a pas su trouvé le mdp originel
		{
			printf("\npas de mot de passe trouvé pour ce hash\n ");
		}
		free(mdp);

	}
	printf("fin affiche_hash");
	return EXIT_SUCCESS;
}


/*-------------------Lecture de fichier ----------------------------*/

/** La fonction lectureFichier a comme rôle de lire par 32 bytes le fichier "fichier" et de remplir dans tab_hash 
	@pre - fichier = pointeur vers le fichier à lire
	@post   - return 0 si tout est ok
		- return -1 si on peut pas ouvrir ou lire le fichier
		- return -2 si erreur malloc
*/
void *lectureFichier(void * fichier)
{
	int fd = open((char*)fichier, O_RDONLY);
	if(fd ==-1)
	{
		printf("Erreur d'ouverture dans lectureFichier\n");
		return NULL;
	}

	hash* ptr = (hash*) malloc(sizeof(hash));
	if(ptr==NULL)
	{
		printf("Erreur malloc allocation mémoire ptr dans lectureFichier\n");
		close(fd);
		return NULL;
	}

	int r = read(fd, ptr,sizeof(hash));
	if(r==-1)
	{
		printf("Erreur de lecture dans lectureFichier\n");
		free(ptr);
		close(fd);
		return NULL;
	}

	while(!fin_de_lecture) //tant qu'on est pas au bout du fichier
	{
		sem_wait(&empty_hash);
		pthread_mutex_lock(&mutex_hash);

		//Début section critique

		// Chercher de la place dans le tableau pour ajouter
		int place_trouvee = 1;
		for(int i=0; i<N  && place_trouvee; i++)
		{
			if(*(tab_hash+i)==NULL) //si la case est vide
			{
				hash* ptrhash = (hash*) malloc(sizeof(hash));
				if(ptrhash == NULL)
				{
					printf("Erreur malloc allocation mémoire ptrhash dans lectureFichier\n");
					close(fd);
					free(ptr);
					return NULL;
				}
				memcpy(ptrhash, ptr, sizeof(hash));
				*(tab_hash+i)=ptrhash;
				printf("L'adresse dans tab_hash est : %p à l'indice %d (lecture)\n", *(tab_hash+i), i);
				printf("Le hash placé dans tab_hash est %s\n", (*(tab_hash+i))->hash );
				place_trouvee=0;
				nbreSlotHashRempli++;
			}
		}

		// Fin section critique
		pthread_mutex_unlock(&mutex_hash);
		sem_post(&full_hash);

		//lecture du hash suivant
		r = read(fd, ptr, sizeof(hash));
		if(r==0)
		{
			pthread_mutex_lock(&mutex_hash);
			printf("le nombre de bytes restant est %d\n", r);
			fin_de_lecture=1;
			pthread_mutex_unlock(&mutex_hash);
		}
	}
	free(ptr);
	close(fd);
	printf("fin lecture\n");
	return EXIT_SUCCESS;
}

/*--------------------------MAIN------------------------------------*/


/** La fonction main est la fonction principale de notre programme:
	@pre - argc = nombre d'arguments passés lors de l'appel de l'exécutable
		- argv = tableau de pointeurs reprenant les arguments donnés par l'exécutable
	@post - exécute les différentes instructions du code
		- retourn EXIT_FAILURE si une erreur se produit
*/
int main(int argc, char *argv[]) {

	
/* 1e étape :  lecture des arguments de la commande de l'exécutable [FAIT]*/
	printf("\n \t\t\t Interprétation des commandes \n");
	int opt;
	int index = 1; // index des fichiers binaires
	//fprintf(stderr, "Usage: %s [-t NTHREADS] [-c] [-o FICHIEROUT] FICHIER1 [FICHIER2 ... FICHIERN]\n",argv[0]);
	while (index<argc) // Tant qu'il reste des options à vérifier
	{
		opt = getopt(argc, argv, "t:o:c");
		switch (opt) 
		{
			case 't':
				nbreThreadsCalcul = atoi(optarg); // conversion du tableau de caractères en int
				N = nbreThreadsCalcul*2;
				printf("-t spécifié : nombre de threads de calcul = %d ;\n",nbreThreadsCalcul);
				index+=2;
				break;
			case 'c':
				critereVoyelles = 0;
				printf("-c spécifié : critère de sélection = occurence des consonnes ;\n");
				index++;
				break;
			case 'o':
				sortieStandard = 0;
				char* fichierSortie = optarg;
				printf("-o spécifié : %s\n",fichierSortie);
				index+=2;
				break;
			default: 
				nbreFichiersEntree = argc - index;
				fichiersEntree = (char**) malloc(nbreFichiersEntree * sizeof(argv[index]));
				printf("-%d fichier(s) binaire(s) spécifié(s) : ",nbreFichiersEntree);
				for(int i=0;index<=(argc-1);index++,i++)
				{
					*(fichiersEntree+i)=argv[index];
					printf("%s ",argv[index]);
				}
				printf("\n");
				break;
		}
	}
	printf("\t\t\t Fin de l'interprétation des commandes \n\n");


	//Initialisation du mutex et des sémaphores
	pthread_mutex_init(&mutex_hash, NULL);

	int err = sem_init(&empty_hash, 0,N);
	if(err != 0)// cas où sem_init a planté 
	{
		fprintf(stderr, "Erreur sem_init du sémaphore empty_hash \n");
		return EXIT_FAILURE;
	}

	err = sem_init(&full_hash, 0, 0);
	if(err != 0)// cas où sem_init a planté 
	{
		fprintf(stderr, "Erreur sem_init du sémaphore full_hash\n");
		return EXIT_FAILURE;
	}


	//Création du tableau contenant les hash
	tab_hash = (struct hash**) malloc( N*sizeof(hash*));	// !!! tab_hash est un tableau d'adresses et pas de hash
	if(tab_hash==NULL)
	{
		fprintf(stderr, "Erreur malloc allocation mémoire pour tab_hash\n");
		return EXIT_FAILURE;
	}

	//Initialisation des cellules du tableaux d'adresse à NULL pour savoir vérifier si une case est NULL.
	for(int i=0; i<N; i++)
	{
		*(tab_hash+i)=NULL;
	}

	/* Me - Il faudra prendre en compte par la suite que il peut y avoir plusieurs fichiers d'entrée dont chaque pointeur est stocké dans fichiersEntree */

	// Création de la liste chainée
	head = (node**) malloc(sizeof(node*)); // head est un pointeur vers la tete de la liste chaînée
	if(head == NULL)
	{
		fprintf(stderr, "Erreur malloc allocation mémoire pour head\n");
		return EXIT_FAILURE;
	}
	
	
	
	






	char* fichier = (char*) malloc(sizeof(argv[1])); // Me - le fichier d'entree ne se situe pas nécessairement à la premiere place du tableau argv
	if(fichier==NULL)
	{
		fprintf(stderr, "Erreur allocation mémoire pour nomFichier\n");
		return EXIT_FAILURE;
	}
	printf("index : %d", index);
	strcpy(fichier,argv[index-1]); // CHANGER POUR PLUSIEURS FICHIERS


	//Initialisation des threads
	pthread_t producteur;
	pthread_t consommateur[nbreThreadsCalcul];
	

	err = pthread_create(&producteur, NULL, &lectureFichier, (void*) fichier);
	if(err !=0) // cas où pthread_create a planté
	{
		fprintf(stderr, "Erreur pthread_create 1\n");
		return EXIT_FAILURE;
	}


	// Boucle pour la creation de plusieurs threads pour reversehash
	for(int i=0; i<nbreThreadsCalcul; i++)
	{
		err = pthread_create(&consommateur[i], NULL, &affiche_hash, (void*)NULL);
		if(err!=0) // cas où pthread_create a planté
		{
			fprintf(stderr, "Erreur pthread_create 2\n");
			return EXIT_FAILURE;
		}
	}


	pthread_join(producteur,NULL);

	// Boucle pour la creation de plusieurs threads pour la lecture
	for(int i=0; i<1; i++)
	{
		// Me - y a pas ca aussi : pthread_join(producteur,NULL); ?
		pthread_join(consommateur[i],NULL);
	}

	free(tab_hash);

	/* 6e étape : quand tous les threads ont fini de s'executer, affiche sur stdout ou écrit dans
	FICHIEROUT la liste chainée qu'il reste
	*/
        printf ("\n \n \nFin du programme...\n");
        return EXIT_SUCCESS;

}
