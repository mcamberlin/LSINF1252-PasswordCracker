/*	Commandes à indiquer dans le shell:
		- cd ~/Documents/LSINF1252-PasswordCracker-Gr118-2019
		- gcc -o cracker cracker.c
		- ./cracker arg1 arg2 arg3
		- echo -n monString | sha256sum
*/
#include <stdio.h>  
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>  // pour utiliser read(), close()
#include <sys/types.h> // pour utiliser open()
#include <sys/stat.h> // pour utiliser open()
#include <fcntl.h>   // pour utiliser open()
#include <ctype.h>  //pour isdigit
#include <semaphore.h> //pour semaphore
#include <pthread.h>  //pour les threads 
#include <stdint.h>


//#include <reverse.h> //include pour utiliser la fonction reverse

/*--------------------------------------------------------------*/

//Arguments des fonctions
char arg_t[] = "-t";
char arg_c[] = "-c";
char arg_o[] = "-o";

//Valeurs par défaut
int nbreThreadsCalcul = 1;
int N = 2; 			// Le nombre de slot du buffer
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



/** La fonction main est la fonction principale de notre programme:
	@pre - argc = nombre d'arguments passés lors de l'appel de l'exécutable
		- argv = tableau de pointeurs reprenant les arguments donnés par l'exécutable
	@post - exécute les différentes instructions du code
		- retourn EXIT_FAILURE si une erreur se produit
*/
int main(int argc, char *argv[]) {

/* 1e étape :  lecture des arguments de la commande de l'exécutable [FAIT]*/
	for (int i=1; i < argc; i++) // hypothèse selon laquelle la commande est exactement ./cracker [-t NTHREADS] [-c] [-o FICHIEROUT] FICHIER1 [FICHIER2 ... FICHIERN]
	{

		if( strstr(argv[i],arg_t) != NULL) // cas où argument -t spécifié
		{
			nbreThreadsCalcul = atoi(argv[i+1]); // conversion du tableau de caractères en int
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
			nbreFichiersEntree+=1;
			printf("- fichier(s) binaires d'entree = %s ;\n",argv[i]);
		}
	}
	
	// lecture du fichier

	//1. ouvrir le fichier
	int fd = open(argv[1],O_RDONLY);
	if(fd ==-1) // cas ou open plante
	{
		printf("Erreur d'ouverture dans lectureFichier\n");
	    	return -1;
	}
	
	//2. lire le fichier
	hash* hash_tmp = (hash*) malloc(sizeof(hash)); // réserver de la mémoire pour mettre les 32 bytes qu'on va ouvrir
	if(hash_tmp==NULL)
	{
		printf("Erreur malloc allocation mémoire monHash dans lectureFichier\n");
		close(fd);
		return -2;
	}
	
	int i=1;
	while(read(fd,&(hash_tmp->hash),256)!=0)
	{	
		printf("hash n° %d : %s \n\n",i,hash_tmp->hash);
		i++;
	}
		
	printf("\n");
		
	//3. Fermer le fichier
	close(fd);
		
        printf ("\n \n \nFin du programme...\n");
        return EXIT_SUCCESS;

}

/*--------------------------------------------------------------*/

