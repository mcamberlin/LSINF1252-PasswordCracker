/*
	Fichier cracker.c réprésente le code dans son intégralité

	Auteurs:
		- CAMBERLIN Merlin
		- PISVIN Arthur
	Version: 05-05-19 - Implémentation de la lecture de plusieurs fichiers binaires d'entée.

	Commandes à indiquer dans le shell:
		- cd ~/Documents/LSINF1252-PasswordCracker-Gr118-2019
		- gcc cracker.c reverse.c sha256.c lectureFichier.c reverse_hash.c insert.c -o cracker -lpthread -Wall -Werror
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
#include <time.h> 		// pour chronométrer le temps d'exécution du programme

// Includes pour utiliser nos fonctions
#include "variables.h"
#include "insert.h" 		
#include "reverse_hash.h" 	
#include "lectureFichier.h"

const int LENPWD = 16; // Nbre maximal de caractères dans les mots de passes originels
const char RETOUR_LIGNE = '\n';
const char CONSONNES[] = {'b','c','d','f','g','h','j','k','l','m','n','p','q','r','s','t','v','w','x','z'};
const char VOYELLES[] = {'a','e','i','o','u','y'};


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

// Déclaration d'un tableau de pointeurs de hash
hash** tab_hash;



// Initialisation du mutex et des 2 sémaphores pour protéger la ressource tab_mdp
pthread_mutex_t mutex_mdp;
sem_t empty_mdp; 
sem_t full_mdp; 

// Déclaration d'un tableau de pointeurs de mots de passe 
char** tab_mdp;

// Déclaration d'une la liste simplement chainée
node** head;


/*--------------------------MAIN------------------------------------*/


/** La fonction main est la fonction principale de notre programme:
	@pre - argc = nombre d'arguments passés lors de l'appel de l'exécutable
		- argv = tableau de pointeurs reprenant les arguments donnés par l'exécutable
	@post - exécute les différentes instructions du code
		- retourn EXIT_FAILURE si une erreur s'est produite et affiche sur la sortie stderr un détail de l'erreur
*/
int main(int argc, char *argv[]) 
{

	int begin = time(NULL); // Démarrer le chronomètre
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
		pthread_join(calculateur[i],NULL);
	}

	pthread_join(comparateur,NULL);

	free(tab_hash);
	free(tab_mdp);
	free(fichiersEntree);

	if(printList(head) == -1)
	{
		printf("Erreur dans printList() \n");
		return EXIT_FAILURE;
	}	


	int end = time(NULL)-begin; // Arrêter le chronomètre
	printf("Le programme a pris %d secondes à s'exécuter. \n",end);
        printf ("\n \n \nFin du programme...\n");
        return EXIT_SUCCESS;

}
