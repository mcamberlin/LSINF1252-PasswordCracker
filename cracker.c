
/*
	Fichier cracker.c réprésente le code dans son intégralité

	Auteurs:
		- CAMBERLIN Merlin
		- PISVIN Arthur
	Version: 05-05-19 - Implémentation de la lecture de plusieurs fichiers binaires d'entée.

	Commandes à indiquer dans le shell:
		- cd ~/Documents/LSINF1252-PasswordCracker-Gr118-2019
		- gcc cracker.c reverse.c sha256.c -o cracker -lpthread -Wall -Werror
		- ./cracker -t 5 -o "output.txt" test-input/02_6c_5.bin
		- echo -n monString | sha256sum

	Commandes git:
		- git status
		- git add monfichier
		- git commit -m "blablabla" monfichier
		- git commit -i -m "blablabla" monfichier // en cas de fusion de conflits
		- git push

	Remarques:
		- Ne pas oublier de faire les docstring à chaque fois
		- A chaque malloc, ne pas oublier en cas d'erreur
		- A chaque initialisation, tester la valeur de retour pour s'assurer du bon fonctionnement
		- Ajuster la version et le commentaire avec
		- fprintf(stderr, "Erreur malloc cas où argument -o spécifié %d\n", errno);
	A indiquer dans le rapport:
		- différence entre reversehash() et reverse_hash()
		- mdp signifie mot de passe
		- Si le nombre de thread de calcul est supérieur au nombre de hash, on fixe le nombre de thread de calcul au nombre de hash contenu dans le fichier
*/

// CONSTANTES dans le préprocesseurs
#define LENPWD 16 // Nbre maximal de caractères dans les mots de passes originels

// CONSTANTES
const char RETOUR_LIGNE = '\n';
const char CONSONNES[] = {'b','c','d','f','g','h','j','k','l','m','n','p','q','r','s','t','v','w','x','z'};
const char VOYELLES[] = {'a','e','i','o','u','y'};

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


// Valeurs par défaut
int nbreThreadsCalcul = 1;
int N = 2;			// Le nombre de slot du buffer
int critereVoyelles = 1;	// true
int sortieStandard = 1;		// true
int nbreFichiersEntree = 1;	// Entier comptant le nombre de fichiers binaires donnés en entrée
char* fichierSortie;		// Pointeur vers le nom du fichier de sortie si -o est spécifié
char** fichiersEntree; 		// Tableau de pointeurs vers les fichiers d'entrée
int occurenceVoyelles=0;	// Entier du nbre maximal d'occurences de voyelles dans un mdp
int occurenceConsonnes=0;	// Entier du nbre maximal d'occurences de consonnes dans un mdp

// Valeurs initiales
int fin_de_lecture = 0;       	// Initialement à false
int nbreSlotHashRempli = 0;	// Initialement à 0
int nbreSlotMdpRempli = 0;	// Initialement à 0
int CalculExecution = 0; 	// Cette variable est un booléen qui permet de savoir si des calculs sont en cours d'exécution


// Initialisation du mutex et des 2 sémaphores pour protéger la ressource tab_hash
pthread_mutex_t mutex_hash;
sem_t empty_hash; 
sem_t full_hash; 

// Déclaration d'une structure représentant un hash  
typedef struct hash
{
	char hash[32]; // 1 hash = 32 bytes et 1char = 1 byte
}hash;

// Déclaration d'un tableau de pointeurs de hash
hash** tab_hash;



// Initialisation du mutex et des 2 sémaphores pour protéger la ressource tab_mdp
pthread_mutex_t mutex_mdp;
sem_t empty_mdp; 
sem_t full_mdp; 

// Déclaration d'un tableau de pointeurs de mots de passe 
char** tab_mdp;

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
		node* previous;	// noeud précédent

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
		return 0;
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
	printf("Fin printList() \n");
	return 0;
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







/*--------------------------------------------------------------*/

/** La fonction reverse_hash() est le consommateur du 1er producteur-consommateur et prend un hash contenu dans la ressource @tab_hash et calcul son inverse avec reversehash() et insère son résultat dans la ressource @tab_mdp.
	@pre - 
	@post - retourne EXIT_SUCCESS si l'ajout dans @tab_mdp du résultat de reversehash() s'est réalisé avec succès, EXIT_FAILURE sinon
*/
void* reverse_hash()
{
	while(!fin_de_lecture || nbreSlotHashRempli)
	// Tant que la lecture du fichier n'est pas finie ou que la ressouce @tab_hash n'est pas vide.
	{
		CalculExecution++;
		char* mdp = (char*) malloc(sizeof(char)*LENPWD);
		if(mdp == NULL)
		{
			fprintf(stderr, "Erreur allocation mémoire mdp dans affiche_hash\n");
			return (void*) EXIT_FAILURE;
		}

		uint8_t* hash;
		// début section critique
		sem_wait(&full_hash);
		pthread_mutex_lock(&mutex_hash);
		
		int conditionArret = 0; // Condition nécessaire pour arrêter de parcourir le tableau une fois qu'un slot rempli a été trouvé
		for(int i=0; i<N && !conditionArret; i++)
		{
			if(*(tab_hash+i)!=NULL) // cas où la case est remplie
			{
				conditionArret = 1;
				hash = (uint8_t*) *(tab_hash+i);
				*(tab_hash+i)=NULL;
				nbreSlotHashRempli--;
			}
		}

		pthread_mutex_unlock(&mutex_hash);
		sem_post(&empty_hash);
		// fin section critique

		printf("début reversehash() \n");
		if( reversehash(hash, mdp, LENPWD) ) // cas où reversehash() a trouvé le mdp originel
		{
			printf("\nle hash affiché grace à reverse_hash est %s\n\n", mdp);
			
			// début section critique
			printf("Début section critique producteur reversehash() \n");
			sem_wait(&empty_mdp);
			pthread_mutex_lock(&mutex_mdp);

			// Chercher de la place dans le tableau pour ajouter
			int place_trouvee = 0;
			for(int i=0; i<N  && !place_trouvee; i++)
			{
				if(*(tab_mdp+i)==NULL) //cas où la case est vide
				{
					place_trouvee = 1;
					*(tab_mdp+i) = mdp;
					printf("L'adresse dans tab_mdp est : %p à l'indice %d (reverse consommateur)\n", *(tab_mdp+i), i);
					
					nbreSlotMdpRempli++;
				}
			}

			
			// fin section critique
			printf("Fin section critique producteur reversehash\n");
			pthread_mutex_unlock(&mutex_mdp);
			sem_post(&full_mdp);
		}
		else // cas où reversehash() n'a pas trouvé le mdp originel
		{
			printf("Pas de mot de passe en clair trouvé pour ce hash\n ");
		}
		CalculExecution--;
	}
	printf("fin reverse_hash() \n");
	return EXIT_SUCCESS;
}


/*-------------------Lecture de fichier ----------------------------*/

/** La fonction lectureFichier() lis par 32 bytes le(s) fichier(s) binaire(s) dans @fichiersEntree et remplit au fur et à mesure la ressource @tab_hash de hash. 
	@pre - 
	@post   - retourne EXIT_SUCCESS si la lecture et l'ajout s'est réalisé avec succès, EXIT_FAILURE sinon
*/
void *lectureFichier()
{
	for(int i=0; i<nbreFichiersEntree;i++)
	{
		fin_de_lecture = 0;

		// Déterminer le nombre maximal de threads de calcul
		char* fichier = (char*) malloc(sizeof(fichiersEntree[i])); 
		if(fichier==NULL)
		{
			fprintf(stderr, "Erreur allocation mémoire pour fichier\n");
			return (void*) EXIT_FAILURE;
		}
		strcpy(fichier,fichiersEntree[i]); 
		
		struct stat stat_fichier;
		if(stat(fichier, &stat_fichier)==-1)
		{
			fprintf(stderr, "Erreur stat fichier\n");
			return (void*) EXIT_FAILURE;
		}
		int nbre_de_hash = (stat_fichier.st_size)/32;
		if(nbre_de_hash<nbreThreadsCalcul) // Si le nbre threads de calcul > nbre de hash, alors, nbre threads de calcul = nbre hash
		{
			nbreThreadsCalcul = nbre_de_hash;
			N = nbreThreadsCalcul*2;
		}

		// Ouvrir le ième fichier binaires
		int fd = open((char*)fichier, O_RDONLY);
		if(fd ==-1)
		{
			fprintf(stderr, "Erreur d'ouverture dans lectureFichier() \n");
			return (void*) EXIT_FAILURE;
		}

		hash* ptr = (hash*) malloc(sizeof(hash));
		if(ptr==NULL)
		{
			fprintf(stderr, "Erreur malloc allocation mémoire ptr dans lectureFichier()\n");
			close(fd);
			return (void*) EXIT_FAILURE;
		}

		int r = read(fd, ptr,sizeof(hash));
		if(r==-1)
		{
			fprintf(stderr, "Erreur de lecture dans lectureFichier() \n");
			free(ptr);
			close(fd);
			return (void*) EXIT_FAILURE;
		}

		while(!fin_de_lecture) //tant que la lecture du fichier binaire @fichier n'est pas terminée
		{
			//Début section critique
			sem_wait(&empty_hash);
			pthread_mutex_lock(&mutex_hash);

			// Chercher de la place dans le tableau pour ajouter
			int place_trouvee = 0;
			for(int i=0; i<N  && !place_trouvee; i++)
			{
				if(*(tab_hash+i)==NULL) //si la case est vide
				{
					place_trouvee=1;
					hash* ptrhash = (hash*) malloc(sizeof(hash));
					if(ptrhash == NULL)
					{
						fprintf(stderr, "Erreur malloc allocation mémoire ptrhash dans lectureFichier() \n");
						close(fd);
						free(ptr);
						return (void*) EXIT_FAILURE;
					}
					memcpy(ptrhash, ptr, sizeof(hash));
					*(tab_hash+i)=ptrhash;
					printf("L'adresse dans tab_hash est : %p à l'indice %d (lecture)\n", *(tab_hash+i), i);
					
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
				// pthread_mutex_lock(&mutex_hash); //Me - utilité ?
				printf("le nombre de bytes restant est %d\n", r);
				fin_de_lecture=1;
				// pthread_mutex_unlock(&mutex_hash); //Me - utilité ?
			}
		}
		free(ptr);
		close(fd);
		printf("Fin de la lecture du %d ème fichier dans lectureFichier()\n",i+1);
	}

	printf("Fin de la lecture lectureFichier()\n");
	return EXIT_SUCCESS;
}

/*--------------------------MAIN------------------------------------*/


/** La fonction main est la fonction principale de notre programme:
	@pre - argc = nombre d'arguments passés lors de l'appel de l'exécutable
		- argv = tableau de pointeurs reprenant les arguments donnés par l'exécutable
	@post - exécute les différentes instructions du code
		- retourn EXIT_FAILURE si une erreur s'est produite et affiche sur la sortie stderr un détail de l'erreur
*/
int main(int argc, char *argv[]) 
{
	clock_t begin = clock(); // Démarrer le chronomètre
	printf("\n \t\t\t Interprétation des commandes \n");
	int opt;
	int index = 1; // index des fichiers binaires
	while (index<argc) // Tant que toutes les options n'ont pas été vérifées
	{
		opt = getopt(argc, argv, "t:o:c");
		switch (opt) 
		{
			case 't':
				nbreThreadsCalcul = atoi(optarg); // conversion du tableau de caractères en int
				N = nbreThreadsCalcul*2;
				printf("-t spécifié : nombre de threads de calcul = %d et nombre de slot = %d;\n",nbreThreadsCalcul, N);
				index+=2;
				break;
			case 'c':
				critereVoyelles = 0;
				printf("-c spécifié : critère de sélection = occurence des consonnes ;\n");
				index++;
				break;
			case 'o':
				sortieStandard = 0;
				fichierSortie = optarg;
				printf("-o spécifié : %s\n",fichierSortie);
				index+=2;
				break;
			default: 
				nbreFichiersEntree = argc - index;
				fichiersEntree = (char**) malloc(nbreFichiersEntree * sizeof(argv[index]));
				if(fichiersEntree == NULL)
				{	
					fprintf(stderr,"Erreur allocation de mémoire pour @fichiersEntree dans interprétation des commandes");
					return EXIT_FAILURE;
				}
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

	// Initialisation du mutex et des sémaphores pour tab_hash
	
	if(pthread_mutex_init(&mutex_hash, NULL) !=0)// cas où pthread_mutex_init() a planté 
	{
		fprintf(stderr, "Erreur pthread_mutex_init() du mutex @mutex_hash\n");
		return EXIT_FAILURE;
	}
	if(sem_init(&empty_hash, 0,N) != 0)// cas où sem_init() a planté 
	{
		fprintf(stderr, "Erreur sem_init() du sémaphore empty_hash : \n");
		return EXIT_FAILURE;
	}
	if(sem_init(&full_hash, 0, 0) != 0)// cas où sem_init() a planté 
	{
		fprintf(stderr, "Erreur sem_init() du sémaphore full_hash : \n");
		return EXIT_FAILURE;
	}

	// Création du tableau de pointeurs contenant les hash
	tab_hash = (struct hash**) malloc( N*sizeof(hash*));
	if(tab_hash==NULL)
	{
		fprintf(stderr, "Erreur malloc allocation mémoire pour tab_hash\n");
		return EXIT_FAILURE;
	}
	// Initialisation des cellules du tableaux d'adresse à NULL pour savoir vérifier si une case est NULL.
	for(int i=0; i<N; i++)
	{
		*(tab_hash+i)=NULL;
	}

	// Création du tableau de pointeurs contenant les mdp
	tab_mdp = (char**) malloc((size_t) N*sizeof(char*));
	if(tab_mdp==NULL)
	{
		fprintf(stderr, "Erreur malloc allocation mémoire pour tab_mdp\n");
		return EXIT_FAILURE;
	}

	// Initialisation des cellules du tableaux d'adresse à NULL pour savoir vérifier si une case est NULL.
	for(int i=0; i<N; i++)
	{
		*(tab_mdp+i)=NULL;
	}


	// Initialisation du mutex et des semaphores pour tab_mdp
	
	pthread_mutex_init(&mutex_mdp, NULL);

	if(sem_init(&empty_mdp, 0, N) != 0)// cas où sem_init a planté 
	{
		fprintf(stderr, "Erreur sem_init du sémaphore empty_mdp \n");
		return EXIT_FAILURE;
	}

	if(sem_init(&full_mdp, 0, 0) != 0)// cas où sem_init a planté 
	{
		fprintf(stderr, "Erreur sem_init du sémaphore full_mdp\n");
		return EXIT_FAILURE;
	}

	
	// Création de la liste chainée
	head = (node**) malloc(sizeof(node*)); // head est un pointeur vers la tête de la liste chaînée
	if(head == NULL)
	{
		fprintf(stderr, "Erreur malloc allocation mémoire pour head\n");
		return EXIT_FAILURE;
	}


	//Initialisation des threads
	pthread_t lecteur;
	pthread_t calculateur[nbreThreadsCalcul];
	pthread_t comparateur;

	if(pthread_create(&lecteur, NULL, &lectureFichier, (void*) NULL) !=0) // cas où pthread_create() a planté
	{
		fprintf(stderr, "Erreur pthread_create 1\n");
		return EXIT_FAILURE;
	}

	int err;
	// Boucle pour la création de plusieurs threads pour reversehash()
	for(int i=0; i<nbreThreadsCalcul; i++)
	{
		err = pthread_create(&calculateur[i], NULL, &reverse_hash, (void*)NULL);
		if(err!=0) // cas où pthread_create() a planté
		{
			fprintf(stderr, "Erreur pthread_create 2\n");
			return EXIT_FAILURE;
		}
	}
 
	if(pthread_create(&comparateur, NULL, &insert_mdp, (void*)NULL)!=0) // cas où pthread_create() a planté
	{
		fprintf(stderr, "Erreur pthread_create 2\n");
		return EXIT_FAILURE;
	}



	pthread_join(lecteur,NULL);

	// Boucle pour la création de plusieurs threads pour la lecture
	for(int i=0; i<nbreThreadsCalcul; i++)
	{
		// Me - y a pas ca aussi : pthread_join(producteur,NULL); ?
		pthread_join(calculateur[i],NULL);
	}

	pthread_join(comparateur,NULL);

	free(tab_hash);
	free(tab_mdp);

	if(printList(head) == -1)
	{
		printf("Erreur dans printList() \n");
		return EXIT_FAILURE;
	}

	clock_t end = clock(); // Arrêter le chronomètre
	double temps = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Le programme a pris %lf secondes à s'exécuter. \n",temps);
        printf ("\n \n \nFin du programme...\n");
        return EXIT_SUCCESS;

}
