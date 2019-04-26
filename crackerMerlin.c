/*
	Fichier crackerMerlin.c réprésente le code dans son intégralité dans lequel la lecture se fait sans producteur consommateur

	Auteurs:
		- CAMBERLIN Merlin
		
	Version: 20-04-19 

	Commandes à indiquer dans le shell:
		- cd ~/Documents/LSINF1252-PasswordCracker-Gr118-2019
		- gcc -o cracker crackerMerlin.c -lpthread
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
*/



#include <stdio.h>  
#include <stdlib.h>
#include <string.h>    // pour utiliser strstr(), memcpy()
#include <unistd.h>    // pour utiliser read(), close()
#include <sys/types.h> // pour utiliser open()
#include <sys/stat.h>  // pour utiliser open()
#include <fcntl.h>     // pour utiliser open()
#include <ctype.h>     // pour isdigit()
#include <semaphore.h> 
#include <pthread.h>    

//#include <reverse.h>
//#include <sha256.h> 

/*--------------------------------------------------------------*/

//Arguments des fonctions
char arg_t[] = "-t";
char arg_c[] = "-c";
char arg_o[] = "-o";

//Valeurs par défaut
int nbreThreadsCalcul = 1;
int NSlot = 2; 			// Le nombre de slot du buffer
int critereVoyelles = 1;	//true
int sortieStandard = 1;		//true
int nbreFichiersEntree = 0;
int fin_de_lecture = 0;       	//false

//Initialisation du mutex et des 2 sémaphores 
pthread_mutex_t mutex_hash;
sem_t empty_hash; 
sem_t full_hash; 

//Déclaration d'une structure représentant un hash  
typedef struct hash{
	char hash[32]; // 1 hash = 32 bytes et 1char = 1 byte
}hash;

//Déclaration de tab_hash qui contient les hash après la lecture (1 hash = 32 char)
hash** tab_hash;




/*--------------------------------------------------------------*/
/** La fonction affiche_hash affiche le hash. Il s'agit du consommateur
	@pre - 
	@post - affiche le hash
*/
void* affiche_hash()
{
	printf("Le consommateur est en cours d'exécution\n");
	
	//reversehash();
	sleep(2);

	printf("Le consommateur a fini son exécution\n");
	
	return NULL;
}


/*--------------------------------------------------------------*/
/*--------------------------------------------------------------*/

/** La fonction lectureFichier est la fonction qui a comme objectif de lire les fichiers binaires pour en extraire les hashs. Il s'agit du producteur.
	@pre - nomFichier = pointeur vers le nom du fichier binaire avec l'extension .bin
	@post - rempli tab_hash au fur et à mesure de la lecture
		- retourne NULL en cas d'erreur
		- retourne le pointeur vers le dernier hash ajouté dans tab_hash

*/
void *lectureFichier(void * nomFichier)
{
	
	//1. ouvrir le fichier
	int fd = open((char*) nomFichier,O_RDONLY);
	if(fd ==-1) // cas ou open plante
	{
		printf("Erreur d'ouverture dans lectureFichier\n");
	    	return NULL;
	}
	
	//2. lire le fichier
	hash* hash_tmp = (hash*) malloc(sizeof(hash)); // réserver de la mémoire pour mettre les hashs lus au fur et à mesure
	if(hash_tmp==NULL)
	{
		printf("Erreur malloc allocation mémoire monHash dans lectureFichier\n");
		close(fd);
		return NULL;
	}
	
	int i=0;
	while(read(fd,&(hash_tmp->hash),256)!=0)
	{	
		printf("Le hash temporaire est : %s \n\n",hash_tmp->hash);
		// Remplissage de tab_hash


		hash* monHash = (hash*) malloc(sizeof(hash));
		if(monHash == NULL)
		{
			printf("Erreur malloc allocation mémoire ptrhash dans lectureFichier\n");
			close(fd);
			free(monHash);
			free(hash_tmp);
			return NULL;
		}
		printf("Début section critique\n");
		//Début section critique
		sem_wait(&empty_hash);
		pthread_mutex_lock(&mutex_hash);
		
		
		memcpy(monHash, hash_tmp, 32);
		*(tab_hash+i) = monHash;
		printf("Le hash placé dans tab_hash est %s \n\n", (char*) ((*(tab_hash+i))->hash));
		
		printf("Fin section critique\n");
		// Fin section critique
		pthread_mutex_unlock(&mutex_hash);
		sem_post(&full_hash);
		
		
//printf("hash n° %d : %s \n\n",i,hash_tmp->hash);
		i++;
	}
		
	free(hash_tmp);
	close(fd);
	printf("lectureFichier terminé avec succès\n");
	return EXIT_SUCCESS;

}


/*--------------------------------------------------------------*/



/** La fonction main est la fonction principale de notre programme:
	@pre - argc = nombre d'arguments passés lors de l'appel de l'exécutable
		- argv = tableau de pointeurs reprenant les arguments donnés par l'exécutable
	@post - exécute les différentes instructions du code
		- retourne EXIT_FAILURE si une erreur se produit
*/
int main(int argc, char *argv[]) {

	/* 1e étape :  lecture des arguments de la commande de l'exécutable [FAIT]*/
	for (int i=1; i < argc; i++)
	// hypothèse selon laquelle la commande est exactement:
	// ./cracker [-t NTHREADS] [-c] [-o FICHIEROUT] FICHIER1 [FICHIER2 ... FICHIERN]
	{

		if( strstr(argv[i],arg_t) != NULL) // cas où argument -t spécifié
		{
			nbreThreadsCalcul = atoi(argv[i+1]); // conversion du tableau de caractères en int
			i+=1;
			NSlot = nbreThreadsCalcul*2;
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
			nbreFichiersEntree+=1;
			printf("- fichier(s) binaires d'entree = %s ;\n",argv[i]);
		}
	}
	
	
	//Initialisation du mutex et des sémaphores
	pthread_mutex_init(&mutex_hash, NULL);

	int err = sem_init(&empty_hash, 0,NSlot);
	if(err != 0)// cas où sem_init a planté 
	{
		printf("Erreur sem_init du sémaphore empty_hash\n");
		return EXIT_FAILURE;
	}

	err = sem_init(&full_hash, 0, 0);
	if(err != 0) // cas où sem_init a planté 
	{
		printf("Erreur sem_init du sémaphore full_hash\n");
		return EXIT_FAILURE;
	}



	//Création du tableau qui contient les hash
	tab_hash = (struct hash**) malloc( (size_t) NSlot*sizeof(hash)); // !!! tab_hash est un tableau d'adresses et pas de hash
	if(tab_hash==NULL)
	{
		printf("Erreur malloc allocation mémoire pour tab_hash\n");
		free(tab_hash);
		return EXIT_FAILURE;
	}


/* utile ?
	//Initialisation des cellules du tableaux d'adresse à zéro
	for(int i=0; i<NSlot; i++)
	{
		*(tab_hash+i)=NULL;
	}	
	
*/	
	char* nomFichier = (char*) malloc(sizeof(argv[1]));
	strcpy(nomFichier,argv[1]);

	
	
	//Initialisation des threads
	pthread_t producteur;
	pthread_t consommateur;

	err = pthread_create(&producteur, NULL, &lectureFichier, (void*) nomFichier);
	if(err !=0) // cas où pthread_create a planté
	{
		printf("Erreur pthread_create producteur\n");
		return EXIT_FAILURE;
	}

	// Lancement des threads de calculs
	for(int i=0; i<nbreThreadsCalcul;i++)
	{
		err = pthread_create(&consommateur, NULL, &affiche_hash, (void*)NULL);
		if(err!=0) // cas où pthread_create a planté
		{
			printf("Erreur pthread_create consommateur\n");
			return EXIT_FAILURE;
		}
	}

	for(int i=0; i<1; i++)
	{
		printf("join producteur\n");
		pthread_join(producteur,NULL);
		printf("join consommateur\n");
		pthread_join(consommateur,NULL);
	}


	
        printf ("\n \n \nFin du programme...\n");
        return EXIT_SUCCESS;

}

/*--------------------------------------------------------------*/

