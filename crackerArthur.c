/*
	Fichier cracker.c réprésente le code dans son intégralité

	Auteurs:
		- CAMBERLIN Merlin
		- PISVIN Arthur
	Version: 10-09-19

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
#include <string.h> // pour utiliser la fonction strstr() semblablee à contains()
#include <unistd.h>  // pour utiliser read(), close()
#include <sys/types.h> // pour utiliser open()
#include <sys/stat.h> // pour utiliser open()
#include <fcntl.h>   // pour utiliser open()
#include <ctype.h>  //pour isdigit
#include <semaphore.h> //pour semaphore
#include <pthread.h>  //pour les threads 



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
	
	//Arguments des fonctions
	char arg_t[] = "-t";
	char arg_c[] = "-c";
	char arg_o[] = "-o";

	//Valeurs par défaut
	int nbreThreadsCalcul = 1; 
	int critereVoyelles = 1;
	int sortieStandard = 1;
	int nbrFichiersEntree = 0;

	printf("la taille du tableau est %d\n",argc);

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
			printf("- fichier(s) binaires d'entree = %s ;\n ",argv[i]);
			printf("%%%%%%%%%%%%%%%%%%");
		}
	}	
	printf("	---------	--------	--------	--------	\n");

	/*
De Coninck Quentin : """ La partie la plus importante à paralléliser n'est pas la lecture des fichiers,
 et si vous justifiez correctement pourquoi votre implémentation dévie du design initial, c'est bon. 
La gestion de plusieurs sources en parallèle peut être vue comme une optimisation. Ne considérez donc 
ce point qu'à la fin de votre projet, lorsque celui-ci fonctionne correctement et parallélise bien 
avec les threads de calcul. """
	*/


	

	//je declare ici les sem et le mutex mais il faudra surement pas le mettre la
	pthread_mutex_t mutex_hash;
	sem_t empty_hash;
	sem_t full_hash;

	pthread_mutex_init(&mutex_hash, NULL);
	sem_init(&empty_hash, 0, 2*nbreThreadsCalcul);
	sem_init(&full_hash, 0, 0);

	

	//creation du tableau qui contient les hash
	void** tab_hash = (void**) malloc( (size_t) 2*nbreThreadsCalcul*sizeof(void*));
	printf("adresse de tabhash : %p\n\n",tab_hash);
	//ATTENTION : c'est un tableau d'adresse, pas de hash (c'est subtil)
	//initialise tout à zéro :
	for(int i=0; i<2*nbreThreadsCalcul;i++)
	{
		*(tab_hash+i)=NULL;
		printf("l'adresse dans tab_hash est : %p   contenu de cette adresse %p\n\n", (tab_hash+i), *(tab_hash+i));
	}

	int fin_de_lecture = 1;
	char* fichier = (char*) malloc(sizeof(argv[1]));
	strcpy(fichier,argv[1]);
	printf("le fichier à lire est : %s\n",fichier); //malloc car utilisé par d'autre thread

	/*
		Lire 32 bytes du fichier filname et mettre dans tabhash qui est notre mutex
		return 0 si tout est ok
		return -1 si on peut pas ouvrir ou lire le fichier
		return -2 si erreur malloc

		argument : file le tableau de fichier à lire
	*/
	void* lectureFichier(void* param)
	{
		//for(int i=0; i<1; i++) pour plusieurs fichiers changer
		//{
		printf("debut lectureFichier\n");
		int fd = open(fichier, O_RDONLY); //pour plusieurs fichiers mettre file[i]
		if(fd==-1)
		{
			return NULL;
		}
		void* ptr;
		while(fin_de_lecture) //tant qu'on est pas au bout du fichier
		{
			printf("debut while lecture\n");
			ptr = (void*) malloc( (size_t) 256);
			printf("adresse de ptr = %p\n", ptr);
			if(ptr==NULL)
			{
				printf("erreur malloc");
			}
			int r = read(fd, ptr, (size_t)256);
			printf("le contenu de ptr apres read est : %s\n r=%d\n", (char*) ptr,r);
			if(r<=255)
			{
				printf("fin de lecture car r<=255\n");
				fin_de_lecture = 0;
				free(ptr);
				close(fd);
				return NULL;
			}
			if(r==-1)
			{
				printf("erreur de lecture car r==-1\n");
				free(ptr);
				close(fd);
				return NULL;
			}
			sem_wait(&empty_hash);
			pthread_mutex_lock(&mutex_hash);
			//section critique : on chercher de la place dans le tableau et on ajoute
			int place_trouvee = 1;
			for(int i=0; i<2*nbreThreadsCalcul  && place_trouvee; i++) //cherche la place dispo
			{
				printf("dans boucle for i=%d \n",i);
				printf("adresse dans tabhash avant le if : %p \n",*(tab_hash+i));
				if(*(tab_hash+i)==NULL) //si la case est vide : l'adresse est NULL
				{
					printf("AVANT L'ASSIGNATION : adresse dans tab_hash apres le if : %p\n valeur dans cette adresse : %s\n",*(tab_hash+i), (char*) *(tab_hash+i));
					*(tab_hash+i)= ptr;
					printf("%ld =? %ld\n", sizeof(tab_hash+i), sizeof(ptr));
					printf("APRES L'ASSIGNATION : adresse dans tab_hash apres le if : %p\n valeur dans cette adresse : %s\n\n",*(tab_hash+i), (char*) *(tab_hash+i));
					place_trouvee=0;
				}
			}
			pthread_mutex_unlock(&mutex_hash);
			sem_post(&full_hash);
		}
		//}
		printf("fin lecture\n");
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
				if(*(tab_hash+i)!=NULL) //si la case est remplie
				{
					printf("le hash est %p\n",*(tab_hash+i));
					free(*(tab_hash+i));
					*(tab_hash+i)=0;
				}
			}
			pthread_mutex_unlock(&mutex_hash);
			sem_post(&empty_hash);
		}
		return NULL;
	}

	/*
	printf("debut fonction lectureFichier\n");
	lectureFichier( argv[1]);
	affiche_hash();

	*/

	//creation des thread

	pthread_t producteur;
	pthread_t consommateur;


	int err=1;
	printf("creation thread1\n");
	err = pthread_create(&producteur, NULL, &lectureFichier, NULL);
	printf("apres creation thread\n");
	if(err !=0)
	{
		printf("err thread1\n");
		return -1;
	}
	printf("creation thread2");
	err = pthread_create(&consommateur, NULL, &affiche_hash, NULL);
	if(err!=0)
	{
		printf("err thread2");
		return -1;
	}

	for(int i = 0; i<1000000000000000000; i++){}

	printf("join threads\n");
	pthread_join(producteur,NULL);
	pthread_join(consommateur,NULL);


	

	/* 2e étape : création du tableau contenant les hash (taille : il avait dit +/- 2 fois le nombre de
	threads de calcul) DO NOT FORGET -> 2 sémaphores et 1 mutex
	*/

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

