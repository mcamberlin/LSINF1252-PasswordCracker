/*
	Fichier cracker.c réprésente le code dans son intégralité

	Auteurs:
		- CAMBERLIN Merlin
		- PISVIN Arthur
	Version: 18-04-19 - fusion de crackerArthur.c et cracker.c avec modifications apportées

	Commandes à indiquer dans le compilateur:
		- cd ~/Documents/LSINF1252-PasswordCracker-Gr118-2019
		- gcc -o cracker cracker.c
		- ./cracker arg1 arg2 arg3

	Commandes git:
		- git status
		- git add monfichier
		- git commit -m "blablabla" monfichier
		- git commit -i -m "blablabla" monfichier // en cas de fusion de conflits
		- git push
	Remarques:
		- Ne pas oublier de faire les docstring à chaque fois
		- A chaque malloc, ne pas oublier de free en cas d'erreur
*/


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


/*--------------------------------------------------------------*/

//Arguments des fonctions
char arg_t[] = "-t";
char arg_c[] = "-c";
char arg_o[] = "-o";

//Valeurs par défaut
int nbreThreadsCalcul = 1;
int N = 2* nbreThreadsCalcul; // Le nombre de slot du buffer
int critereVoyelles = 1;
int sortieStandard = 1;
int nbrFichiersEntree = 0;
int fin_de_lecture = 0;       

//Initialisation du mutex et des 2 sémaphores 
pthread_mutex_t mutex_hash;
sem_t empty_hash; 
sem_t full_hash; 


//Déclaration d'une structure représentant un hash  
typedef struct hash{
	char hash[32]; // 1 hash = 32 bytes et 1char = 1 byte
}hash;

/** La fonction affiche_hash affiche le hash
	@pre - 
	@post - affiche le hash
*/
void* affiche_hash()
{
	while(fin_de_lecture)
	{
		sem_wait(&full_hash);
		pthread_mutex_lock(&mutex_hash);
		for(int i=0; i<2*nbreThreadsCalcul; i++)
		{
			if(tab_hash[i]!=NULL) //si la case est remplie
			{
				printf("le hash affiché est %d\n",*(tab_hash[i]->hash));
				tab_hash[i]=NULL;
			}
		}
		pthread_mutex_unlock(&mutex_hash);
		sem_post(&empty_hash);
	}
	return NULL;
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
		return -1;
	}
	
	hash* ptr = (hash*) malloc(32);
	if(ptr==NULL)
	{
		printf("Erreur malloc allocation mémoire ptr dans lectureFichier\n");
		return -2;
	}

	int r = read(fd, ptr,32);
	if(r==-1)
	{
		printf("Erreur de lecture dans lectureFichier\n");
		free(ptr);
		close(fd);
		return -1;
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
					return -2;
				}
				memcpy( ptrhash, ptr, 32);
				memcmp((void*) ptrhash, (void*) ptr;
				*(tab_hash+i)=ptrhash;
				place_trouvee=0;
			}
		}
		
		printf("Dodo");
		sleep(3);
		printf("Réveil\n");

		// Fin section critique
		pthread_mutex_unlock(&mutex_hash);
		sem_post(&full_hash);

		//lecture du hash suivant
		r = read(fd, ptr, 32);
		if(r<32)
		{
			fin_de_lecture=1;
		}
	}
	free(ptr);
	close(fd);
	return 0;
}

/*--------------------------------------------------------------*/


/** La fonction main est la fonction principale de notre programme:
	@pre - argc = nombre d'arguments passés lors de l'appel de l'exécutable
		- argv = tableau de pointeurs reprenant les arguments donnés par l'exécutable
	@post - exécute les différentes instructions du code
		- retourn EXIT_FAILURE si une erreur se produit
*/
int main(int argc, char *argv[]) {

// ./cracker [-t NTHREADS] [-c] [-o FICHIEROUT] FICHIER1 [FICHIER2 ... FICHIERN]

/* 1e étape :  lecture des arguments de la commande de l'exécutable [FAIT]*/
	
	// considération du cas dans lequel les arguments de l'exécutable sont valides.
	for (int i=1; i < argc; i++) 
	{

		if( strstr(argv[i],arg_t) != NULL) // cas où argument -t spécifié
		{
			nbreThreadsCalcul = atoi(argv[i+1]); // conversion du tableau de caractères en int ! risque d'erreur
			i+=1;
			N = nbreThreadsCalcul*2;
			printf("-t spécifié : nombre de threads de calcul = %d ;\n",nbreThreadsCalcul);
		}
		if (strstr(argv[i],arg_c) != NULL) // cas où argument -c spécifié
		{
			critereVoyelles = 0;
			printf("-c spécifié : critère de sélection = occurence des consonnes ;\n");

		}		
		if (strstr(argv[i],arg_o) != NULL)// cas où argument -o spécifié
		{
			sortieStandard = 0;
			char* fichierSortie = (char*) malloc(sizeof(argv[i+1]));
			if(fichierSortie == NULL) // cas où malloc a planté
			{ 	
				free(fichierSortie);
				printf("Erreur malloc cas où argument -o spécifié");
				return EXIT_FAILURE;
			}
			printf("-o spécifié : %s\n",argv[i+1]);
			i+=1;
		}
		else
		{
			nbrFichiersEntree+=1;
			printf("- fichier(s) binaires d'entree = %s ;\n",argv[i]);
		}
	}	
	

/* 2e étape :  lecture des fichiers d'entree */

	//Initialisation du mutex et des sémaphores
	pthread_mutex_init(&mutex_hash, NULL);
	sem_init(&empty_hash, 0,N);
	sem_init(&full_hash, 0, 0);


	//Création du tableau contenant les hash
	tab_hash = (struct hash**) malloc( N*sizeof(hash));//ATTENTION : c'est un tableau d'adresse, pas de hash (c'est subtile)
	if(tab_hash==NULL)
	{
		printf("Erreur malloc allocation mémoire pour tab_hash");
		return EXIT_FAILURE;
	}
	
	//Initialisation des cellules du tableaux d'adresse à zéro
	for(int i=0; i<N; i++)
	{
		*(tab_hash+i)=NULL;
	}

	// Me - Quelle est l'utilité de la variable fichier ?

	//Malloc car utilisé par d'autre thread
	char* fichier = (char*) malloc(sizeof(argv[1])); // Me - le fichier d'entree ne se situe pas nécessairement à la premiere place du tableau argv:/
	strcpy(fichier,argv[1]);
		
	//for(int i=0; i<nbrFichiersEntree;i++)
	//{
		lectureFichier( argv[1]); // argv[i] pour plusieurs fichiers d'entree
		affiche_hash();
	//}
	


	//Initialisation des threads
	pthread_t producteur;
	pthread_t consommateur;


	int err = pthread_create(&producteur, NULL, &lectureFichier, (void*) fichier);
	if(err !=0) // cas où pthread_create plante
	{
		printf("Erreur pthread_create 1\n");
		return EXIT_FAILURE;
	}

	err = pthread_create(&consommateur, NULL, &affiche_hash, (void*)NULL);
	if(err!=0)
	{
		printf("Erreur pthread_create 2");
		return EXIT_FAILURE;
	}

	pthread_join(producteur,NULL);
	pthread_join(consommateur,NULL);


	/* 3e étape : les threads de calculs de reverse
	*/

	/* 4e étapes : création du tableau contenant les MdP (taille : même que pour les hach ?)
	*/

	/* 5e étape : comparaison du MdP qui sort du tableau avec les autres MdP dans une liste chainées.
	Si plus petit nombre d'occurence dans le nouveau MdP : on ne fait rien
	Si même nombre d'occurence : on ajoute le MdP à la liste chainées
	Si plus grand nombre d'occurence : on supprime la liste chainée existance pour la remplacer par le nouveau MdP
	*/

	/* 6e étape : quand tous les threads ont fini de s'executer, affiche sur stdout ou écrit dans
	FICHIEROUT la liste chainée qu'il reste
	*/
        printf ("\n \n \nFin du programme...\n");
        return EXIT_SUCCESS;

}
