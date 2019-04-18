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
int critereVoyelles = 1;
int sortieStandard = 1;
int nbrFichiersEntree = 0;
int fin_de_lecture = 1;       

//Initialisation du mutex et des 2 sémaphores 
pthread_mutex_t mutex_hash;
sem_t empty_hash; 
sem_t full_hash; 


//Déclaration d'une structure représentant un hash de 32bytes 
typedef struct hash{
	char hash[32]; //(1char = 1 byte)
}hash;


//declaration de tab_hash contenant les hash apres la lecture (1 hash = 32 bytes)
hash** tab_hash;


/*-------------------Lecture de fichier ----------------------------*/

/** La fonction lectureFichier a comme rôle de lire par 32 bytes le fichier "fichier" et de remplir dans tab_hash 
	@pre - fichier = pointeur vers le fichier à lire
	@post   - return 0 si tout est ok
		- return -1 si on peut pas ouvrir ou lire le fichier
		- return -2 si erreur malloc
*/
void *lectureFichier(void * fichier)
{
	//for(int i=0; i<1; i++) pour plusieurs fichiers changer
	//{
	printf("debut lectureFichier\n");
	printf("FIXONS LA CHOSE : taille de hash-> %ld     taile de 32 char -> %ld\n", sizeof(hash), 32*sizeof(char));

	int fd = open((char*)fichier, O_RDONLY); //pour plusieurs fichiers mettre file[i]
	if(fd ==-1)
	{
		return NULL;
	}
	
	hash* ptr = (hash*) malloc(32);
	// printf("taille de ptr : %ld\n", sizeof(ptr));
	if(ptr==NULL)
	{
		printf("erreur malloc\n");
	}

	int r = read(fd, ptr,32*sizeof(char));
	printf("lectureFichier : adresse de ptr = %p    taille de char : %ld\n", ptr, sizeof(char));
	if(r==-1)
	{
		printf("erreur de lecture car r==-1\n");
		free(ptr);
		close(fd);
		return NULL;
	}
	printf("lectureFichier : le contenu de ptr apres read est : %s   r=%d\n", (char*) ptr,r);
	while(fin_de_lecture) //tant qu'on est pas au bout du fichier
	{
		printf("debut while lecture\n");
		printf("dans le tableau : %p\n", *(tab_hash));
		sem_wait(&empty_hash);
		pthread_mutex_lock(&mutex_hash);
		printf("debut section critique");
		//section critique : on chercher de la place dans le tableau et on ajoute
		int place_trouvee = 1;
		for(int i=0; i<2*nbreThreadsCalcul  && place_trouvee; i++) //cherche la place dispo
		{
			printf("lectureFichier : dans boucle for i=%d \n",i);
			printf("lectureFichier : adresse dans tabhash avant le if : %p  OK\n",*(tab_hash+i));
			if(*(tab_hash+i)==NULL) //si la case est vide
			{
				printf("Dans if\n");
				hash* ptrhash = (hash*) malloc(sizeof(hash));
				printf("ptrhash : %ld  =?  ptr : %ld\n", sizeof(*ptrhash), sizeof(*ptr));
				printf("memcpy fuck you \n");
				memcpy( ptrhash, ptr, 32*sizeof(char));
				printf("MEME CHOSE ? -> %d\n", memcmp((void*) ptrhash, (void*) ptr, 32*sizeof(char)));
				printf("ptrhash : %s",ptrhash->hash);
				*(tab_hash+i)=ptrhash;
				printf("dans le tableau : %p     ptrhash : %p\n", *(tab_hash+i), ptrhash);
				place_trouvee=0;
				printf("HASH DEDANS\n");
			}
		}
		printf("dans le tableau : %p\n", *(tab_hash));
		printf("debut dodo");
		sleep(3);
		printf("fin dodo\n");
		pthread_mutex_unlock(&mutex_hash);
		sem_post(&full_hash);
		//lecture du hash suivant
		int r = read(fd, ptr, 32*sizeof(char));
		printf("lectureFichier : le contenu de ptr apres read est : %s   r=%d\n\n", (char*) ptr,r);
		if(r<32)
		{
			fin_de_lecture=0;
		}
		printf("dans le tableau : %p\n", *(tab_hash));
	}
	//}
	free(ptr);
	printf("fin lecture\n");
	close(fd);
	return NULL;
}




//fonction test consommateur
void* affiche_hash(void* param)
{
	printf("debut affiche\n");
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


/*--------------------------------------------------------------*/


/** La fonction main est la fonction principale de notre programme:
	@pre - argc = nombre d'arguments passés lors de l'appel de l'exécutable
		- argv = tableau de pointeurs reprenant les arguments donnés par l'exécutable
	@post - exécute les différentes instructions du code

*/
int main(int argc, char *argv[]) {

// ./cracker [-t NTHREADS] [-c] [-o FICHIEROUT] FICHIER1 [FICHIER2 ... FICHIERN]

	/* 1e étape : 
			1.1 lecture des arguments de la commande de l'exécutable [FAIT]
			1.2 ouverture des fichiers binaires [En cours - Merlin]
			1.3 Initialisation un thread par type de fichier
	*/
	
	// considération du cas dans lequel les arguments de l'exécutable sont valides.
	for (int i=1; i < argc; i++) 
	{

		if( strstr(argv[i],arg_t) != NULL) // cas où argument -t spécifié
		{
			nbreThreadsCalcul = atoi(argv[i+1]); // conversion du tableau de caractères en int ! risque d'erreur
			i+=1;
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
	printf("	---------	--------	--------	--------	");

	/*
De Coninck Quentin : """ La partie la plus importante à paralléliser n'est pas la lecture des fichiers,
 et si vous justifiez correctement pourquoi votre implémentation dévie du design initial, c'est bon. 
La gestion de plusieurs sources en parallèle peut être vue comme une optimisation. Ne considérez donc 
ce point qu'à la fin de votre projet, lorsque celui-ci fonctionne correctement et parallélise bien 
avec les threads de calcul. """
	*/

	for(int i=argc-nbrFichiersEntree; i<nbrFichiersEntree;i++) // tant que tous les fichiers n'ont pas été lu
	{
		int ouverture = open(argv[i],O_RDONLY);
		if(ouverture ==-1) // cas où open plante
		{
			printf("Erreur open dans l'ouverture du fichier");
			return EXIT_FAILURE;
		}
		//2. lire le fichier
		size_t nbreOctetsHash = (size_t) 32;
		void* buf = malloc(nbreOctetsHash);
		if(buf == NULL)  //cas où malloc plante
		{
			printf("Erreur malloc dans la réservation de mémoire du buffer");
			free(buf);
			return EXIT_FAILURE;
		}

		ssize_t lecture = read(ouverture, &buf, nbreOctetsHash); 

		if(lecture == -1) // cas où read plante
		{
			printf("Erreur open dans l'ouverture du fichier");
			//3. Fermer le fichier
			if(close(ouverture) !=0)  // cas où close plante
			{
				printf("Erreur close dans la fermeture du fichier");
				free(buf);
				return EXIT_FAILURE;
			}
			free(buf);
			return EXIT_FAILURE;
		
		}
		while(lecture == nbreOctetsHash) // tant que read se deroule bien
		{
			// creation d'un thread

			lecture = read(ouverture, &buf,nbreOctetsHash);            
		}

		//3. Fermer le fichier
		if(close(ouverture) !=0) // cas où close plante
		{    
			printf("Erreur close dans la fermeture du fichier");
			free(buf);
			return EXIT_FAILURE;
		}
	}





	/* 2e étape : création du tableau contenant les hash
	*/

	//Initialisation du mutex et des sémaphores
	pthread_mutex_init(&mutex_hash, NULL);
	int N = 2*nbreThreadsCalcul; // Initialisation de la taille du buffer
	sem_init(&empty_hash, 0,N);
	sem_init(&full_hash, 0, 0);


	//Création du tableau contenant les hash
	tab_hash = (struct hash**) malloc( (size_t) N*sizeof(hash));
	if(tab_hash==NULL)
	{
		printf("err malloc tab_hash");
		return -1;
	}
	printf("adresse de tabhash : %p\ntaille de struct_hash : %d\n\n",tab_hash,(int) sizeof(hash));
	//ATTENTION : c'est un tableau d'adresse, pas de hash (c'est subtil)
	//initialise tout à zéro :
	for(int i=0; i<N; i++)
	{
		*(tab_hash+i)=NULL;
	}



	char* fichier = (char*) malloc(sizeof(argv[1]));
	strcpy(fichier,argv[1]);
	printf("le fichier à lire est : %s\n",fichier); //malloc car utilisé par d'autre thread

	/*
	printf("debut fonction lectureFichier\n");
	lectureFichier( argv[1]);
	affiche_hash();

	*/

	//creation des thread
	printf("initialisation des threads");
	pthread_t producteur;
	pthread_t consommateur;


	int err;
	printf("creation thread1\n");
	err = pthread_create(&producteur, NULL, &lectureFichier, (void*) fichier);
	if(err !=0)
	{
		printf("err thread1\n");
		return -1;
	}
	err = pthread_create(&consommateur, NULL, &affiche_hash, (void*)NULL);
	if(err!=0)
	{
		printf("err thread2");
		return -1;
	}

	printf("join threads producteur\n");
	pthread_join(producteur,NULL);
	printf("join, thread consommateur\n");
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
