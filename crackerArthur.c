
/*
	Fichier cracker.c réprésente le code dans son intégralité

	Auteurs:
		- CAMBERLIN Merlin
		- PISVIN Arthur
	Version: 01-05-19 - Ajout fonction count_vowels() et count_consonants() + création liste chainée

	Commandes à indiquer dans le shell:
		- cd ~/Documents/LSINF1252-PasswordCracker-Gr118-2019
		- gcc cracker.c reverse.c sha256.c -o cracker -lpthread 
		- ./cracker -t 5 test-input/02_6c_5.bin
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
#include <stdio.h> // pour utiliser fopen()
#include <stdlib.h>
#include <string.h> // pour utiliser la fonction strstr() semblable à contains()
#include <unistd.h>  // pour utiliser read(), close(), write()
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
int nbreSlotMdpRempli = 0;
char* fichierSortie;

// Déclaration d'un tableau de pointeurs contenant les noms des fichiers d'entrée
char** fichiersEntree; 

// Initialisation du mutex et des 2 sémaphores pour tab_hash
pthread_mutex_t mutex_hash;
sem_t empty_hash; 
sem_t full_hash; 

//Initialisation du mutex et des 2 sémaphores pour tab_mdp
pthread_mutex_t mutex_mdp;
sem_t empty_mdp; 
sem_t full_mdp; 


// Déclaration d'une structure représentant un hash  
typedef struct hash
{
	char hash[32]; // 1 hash = 32 bytes et 1char = 1 byte
}hash;

// Déclaration d'un tableau de pointeur de hash
hash** tab_hash;

// Déclaration d'un tableau de pointeur de mot de passe 
char** tab_mdp;


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
		if(j<20 && *(monString+i) == CONSONNES[j]) // j<20 sinon outOfBounds
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
		if(j<6 && *(monString+i) == VOYELLES[j]) // j<6 sinon outOfBounds
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

/** La fonction printList() affiche la liste chainée remplie des 
	@pre - 
	@post - 0 si l'affichage de la liste chaînée s'est réalisée avec succès, -1 sinon

*/
int printList(node** head)
{
	if(head == NULL) // argument vide
	{
		fprintf(stderr, "**head non spécifié dans printList() \n");
		return -1;
	}
	if(*head == NULL) // liste vide
	{
		fprintf(stderr, "La liste chaînée de mdp est vide \n");
		return 0;
	}

	if(sortieStandard == 1) // si il faut écrire sur la sortie standard
	{
		node* runner = *head;
		while(runner != NULL)
		{
			printf("%s \n",runner->mdp);
			runner = runner->next;
		}
		return 0;
	}
	else // si il faut écrire dans le fichier @fichierSortie
	{

		int fd = open(fichierSortie,O_RDONLY|O_CREAT,O_RDONLY);
		if(fd ==-1)// cas ou open a planté
		{
		    return -1;
		}
		
		node* runner = *head;

		void* buf;
		int err;
		while(runner != NULL)
		{
			buf = malloc(sizeof(runner->mdp));
			if(buf == NULL) // cas où malloc a planté
			{
				fprintf(stderr, "Erreur allocation de mémoire pour @buf dans printList() \n");
				return -1;
			}
			strcpy(buf,runner->mdp);
			err = (int) write(fd, buf, sizeof(runner->mdp));
			if(err == -1)
			{
				fprintf(stderr, "Erreur dans write() \n");
				return -1;
			}
			//printf("%s \n",runner->mdp);
			runner = runner->next;
			free(buf);
		}

		if(close(fd) ==-1)
		{
			fprintf(stderr, "Erreur fermeture du fichier dans printList() \n");
		    	return -1;
		}
		printf("Fin printList() \n");
		return 0;
	}
}	


/* ---------------------------------------------- */
// COMPARER LE NOMBRE D'OCCURENCES DU MDP AVEC CEUX DANS LA LISTE CHAINEE
int insertInList(char* mdp)
{
	if(critereVoyelles == 1) // cas ou le critère de sélection des mdp sont les voyelles
	{
		int vowels = count_vowels(mdp);
		if(vowels==occurenceVoyelles) // cas ou le mdp contient le meme nombre de voyelles que les précédents
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
			// 1.Libérer toutes la liste chainée
			if(freeLinkedList(head) ==-1)
			{

				return -1;
			}

			// Insertion du mdp en tête dans la liste chainée
			if(insert(head,mdp) ==-1)
			{
				return -1;
			}

		}
		//if(vowels<occurenceVoyelles)// cas ou le mdp contient moins de voyelles que les précédents
		//{ ne pas ajouter le mdp }


	}
	else // cas ou le critère de sélection des mdp sont les consonnes
	{
		int consonants = count_consonants(mdp);
		if(consonants==occurenceConsonnes) // cas ou le mdp contient le meme nombre de consonnes que les précédents
		{
			// Insertion du mdp en tête dans la liste chainée
			if(insert(head,mdp) ==-1)
			{
				return -1;
			}
		}
		printf("Il y a %d consonnes dans %s \n",consonants,mdp);
		if(consonants>occurenceConsonnes)
		{
			occurenceConsonnes = consonants;
			// 1.Libérer toutes la liste chainée
			if(freeLinkedList(head) ==-1)
			{
				return -1;
			}

			// Insertion du mdp en tête dans la liste chainée
			if(insert(head,mdp) ==-1)
			{
				return -1;
			}

		}
		//if(consonants<occurenceConsonnes)// cas ou le mdp contient moins de consonnes que les précédents
		//{ ne pas ajouter le mdp }
	}
	return 0;
}



/*--------------------------------------------------------------*/

/** La fonction comparateur est le consommateur du producteur reverse hash
	@pre - 
	@post - ajuste la liste chaînée
*/
void* insert_mdp()
{
	while(!fin_de_lecture || nbreSlotHashRempli || nbreSlotMdpRempli)
	{

		char* mdp;

		sem_wait(&full_mdp);
		pthread_mutex_lock(&mutex_mdp);

		printf("section critique insert_mdp\n");

		int conditionArret=1;

		for(int i=0; i<N && conditionArret; i++)
		{
			if(*(tab_mdp+i)!=NULL) //si la case est remplie
			{
				mdp = (char*) *(tab_mdp+i);
				*(tab_mdp+i)=NULL;
				nbreSlotMdpRempli--;
				conditionArret = 0;
			}
		}

		pthread_mutex_unlock(&mutex_mdp);
		sem_post(&empty_mdp);

		if(insertInList(mdp)==-1)
		{
			fprintf(stderr, "Erreur insertion mot de passe dans tab_mdp\n");
			return (void*) EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}







/*--------------------------------------------------------------*/

/** La fonction affiche_hash est le consommateur du producteur-consommateur
	@pre - 
	@post - lis les fichiers binaires en entrée, lance reversehash(), ajuste la liste chaînée
*/
void* reverse_hash()
{
	while(!fin_de_lecture || nbreSlotHashRempli)
	// Tant que la lecture du fichier n'est pas finie ou que le buffer n'est pas vide.
	{
		char* mdp = (char*) malloc(sizeof(char)*LENPWD);
		if(mdp == NULL)
		{
			fprintf(stderr, "Erreur allocation mémoire mdp dans affiche_hash\n");
			return (void*) EXIT_FAILURE;
		}

		uint8_t* hash;

		sem_wait(&full_hash);
		pthread_mutex_lock(&mutex_hash);
		//début section critique
		int conditionArret = 1; // condition nécessaire pour sortir de la boucle for une fois que le pointeur du slot a ete copié dans hash
		for(int i=0; i<2*nbreThreadsCalcul && conditionArret; i++)
		{
			if(*(tab_hash+i)!=NULL) //si la case est remplie
			{
				hash = (uint8_t*) *(tab_hash+i);
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
			printf("\nle hash affiché grace à reverse_hash est %s\n\n", mdp);

			sem_wait(&empty_mdp);
			printf("place trouvée dans tab_mdp\n");
			pthread_mutex_lock(&mutex_mdp); // REMPLACER HASH PAR MDP

			printf("section critique producteur reversehash\n");
			//Début section critique

			// Chercher de la place dans le tableau pour ajouter
			int place_trouvee = 1;
			for(int i=0; i<N  && place_trouvee; i++)
			{
				if(*(tab_mdp+i)==NULL) //si la case est vide
				{
					*(tab_mdp+i) = mdp;
					printf("L'adresse dans tab_mdp est : %p à l'indice %d (lecture)\n", *(tab_mdp+i), i);
					place_trouvee = 0;
					nbreSlotMdpRempli++;
				}
			}

			// Fin section critique
			pthread_mutex_unlock(&mutex_mdp);
			sem_post(&full_mdp);

		}
		else // cas ou reversehash n'a pas su trouvé le mdp originel
		{
			printf("\npas de mot de passe trouvé pour ce hash\n ");
		}

	}
	printf("fin affiche_hash \n");
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


	char* fichier = (char*) malloc(sizeof(argv[index-1])); // Me - le fichier d'entree ne se situe pas nécessairement à la premiere place du tableau argv
	if(fichier==NULL)
	{
		fprintf(stderr, "Erreur allocation mémoire pour nomFichier\n");
		return EXIT_FAILURE;
	}
	strcpy(fichier,argv[index-1]); // CHANGER POUR PLUSIEURS FICHIERS

	/*
	Si le nombre de thread de calcul est supérieur au nombre de hash, on fixe le nombre de thread de calcul au nombre de hash
	*/
	struct stat stat_fichier;
	if(stat(fichier, &stat_fichier)==-1)
	{
		fprintf(stderr, "Erreur stat fichier\n");
	}
	int nbre_de_hash = (stat_fichier.st_size)/32;
	if(nbre_de_hash<nbreThreadsCalcul)
	{
		nbreThreadsCalcul = nbre_de_hash;
		N = nbreThreadsCalcul*2;
	}

	/*
	Initialisation du mutex et des sémaphores pour tab_hash
	*/
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

	/*
	Création du tableau contenant les hash
	*/
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


	printf("avant initialisation tab_mdp\n");
	/*
	Création du tableau contenant les mdp
	*/
	printf("taille du tab_mdp = %ld\n", N*sizeof(char)*LENPWD);



	tab_mdp = (char**) malloc((size_t) N*sizeof(char)*LENPWD);
	if(tab_mdp==NULL)
	{
		fprintf(stderr, "Erreur malloc allocation mémoire pour tab_mdp\n");
		return EXIT_FAILURE;
	}
	printf("apres declaration de tab_hash\n");
	//Initialisation des cellules du tableaux d'adresse à NULL pour savoir vérifier si une case est NULL.
	for(int i=0; i<N; i++)
	{
		*(tab_mdp+i)=NULL;
		printf("dans tab_mdp : %s\n", *(tab_mdp+i));
	}
	printf("apres initialisation tab_mdp\n");


	/*
	Initialisation du mutex et des semaphores pour tab_mdp
	*/
	pthread_mutex_init(&mutex_mdp, NULL);
	printf("apres initialisation mutex mdp");

	err = sem_init(&empty_mdp, 0, N);
	if(err != 0)// cas où sem_init a planté 
	{
		fprintf(stderr, "Erreur sem_init du sémaphore empty_mdp \n");
		return EXIT_FAILURE;
	}

	err = sem_init(&full_mdp, 0, 0);
	if(err != 0)// cas où sem_init a planté 
	{
		fprintf(stderr, "Erreur sem_init du sémaphore full_mdp\n");
		return EXIT_FAILURE;
	}

	/* Me - Il faudra prendre en compte par la suite que il peut y avoir plusieurs fichiers d'entrée dont chaque pointeur est stocké dans fichiersEntree */

	printf("avant malloc head");
	// Création de la liste chainée
	head = (node**) malloc(sizeof(node*)); // head est un pointeur vers la tete de la liste chaînée
	if(head == NULL)
	{
		fprintf(stderr, "Erreur malloc allocation mémoire pour head\n");
		return EXIT_FAILURE;
	}
	printf("apres malloc head\n");






	//Initialisation des threads
	pthread_t lecteur;
	pthread_t calculateur[nbreThreadsCalcul];
	pthread_t comparateur;


	err = pthread_create(&lecteur, NULL, &lectureFichier, (void*) fichier);
	if(err !=0) // cas où pthread_create a planté
	{
		fprintf(stderr, "Erreur pthread_create 1\n");
		return EXIT_FAILURE;
	}


	// Boucle pour la creation de plusieurs threads pour reversehash
	for(int i=0; i<nbreThreadsCalcul; i++)
	{
		err = pthread_create(&calculateur[i], NULL, &reverse_hash, (void*)NULL);
		if(err!=0) // cas où pthread_create a planté
		{
			fprintf(stderr, "Erreur pthread_create 2\n");
			return EXIT_FAILURE;
		}
	}


	err = pthread_create(&comparateur, NULL, &insert_mdp, (void*)NULL);
	if(err!=0) // cas où pthread_create a planté
	{
		fprintf(stderr, "Erreur pthread_create 2\n");
		return EXIT_FAILURE;
	}



	pthread_join(lecteur,NULL);

	// Boucle pour la creation de plusieurs threads pour la lecture
	for(int i=0; i<nbreThreadsCalcul; i++)
	{
		// Me - y a pas ca aussi : pthread_join(producteur,NULL); ?
		pthread_join(calculateur[i],NULL);
	}

	pthread_join(comparateur,NULL);

	free(tab_hash);

	/* 6e étape : quand tous les threads ont fini de s'executer, affiche sur stdout ou écrit dans
	FICHIEROUT la liste chainée qu'il reste
	*/
	printf("Début printList() \n");
	printList(head);
        printf ("\n \n \nFin du programme...\n");
        return EXIT_SUCCESS;

}
