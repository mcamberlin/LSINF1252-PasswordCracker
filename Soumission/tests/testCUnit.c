/*
	Fichier testCUnit.c est le fichier qui teste le fonctionnement de la fonction lectureFichier(). Il utilise en entrée le fichier binaire ouinon.bin qui contient le hash de non et de oui dans cet ordre

	Auteurs:
		- CAMBERLIN Merlin
		- PISVIN Arthur
	Version: 9-05-19
	
	Commandes: 
		- echo -n oui | sha256sum | xxd -r -p - "output.bin"
		- gcc testCUnit.c reverse.c sha256.c lectureFichier.c reverse_hash.c insert.c -o test -I${HOME}/local/include -lcunit -L${HOME}/local/lib -lpthread
		- cat output2.bin output.bin > outputFinal.bin

	Commandes git:
		- git status
		- git add monfichier
		- git commit -m "blablabla" monfichier
		- git commit -i -m "blablabla" monfichier // en cas de fusion de conflits
		- git push
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


#include <CUnit/CUnit.h>
#include "CUnit/Basic.h"


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









int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

char* fichierTest1 = "ouinon.bin";



/** La fonction test_lectureFichier_true() lance lectureFichier() et vérifie qu'elle ne retourne pas une erreur.
*/
/*void test_lectureFichier_true(void)
{	
	tab_hash = (hash**) malloc(5*sizeof(hash*));
	if(tab_hash == NULL)
	{
		return;
	}
	for(int i=0; i<5;i++)
	{
		*(tab_hash+i)=NULL;
	}
	printf("le fichier à lire est %s\n", fichierTest1);
	fichiersEntree = &fichierTest1;


	CU_ASSERT_EQUAL(lectureFichier(),(void*) EXIT_SUCCESS);
}
*/

/** La fonction test_lectureFichier_insertion() vérifie que lectureFichier() remplit le premier buffer de 2 pointeurs. Pour le vérifier, on initialise les cellules du buffer à NULL et on vérifie ensuite que leurs valeurs ont été modifiées.
*/
void test_lectureFichier_insertion(void)
{
	fichiersEntree = &fichierTest1;
	tab_hash = (hash**) malloc(2*sizeof(hash*));
	if(tab_hash == NULL)
	{
		return;
	}
	for(int i=0; i<2;i++)
	{
		*(tab_hash+i)=NULL;
	}
	lectureFichier();

	CU_ASSERT_NOT_EQUAL(*(tab_hash),NULL);
	CU_ASSERT_NOT_EQUAL(*(tab_hash+1),NULL);
}


/** La fonction test_reverse_hash()  remplit le deuxième buffer de 2 pointeurs vers les 2 mdps. Pour le vérifier, on initialise les cellules du buffer à NULL et on vérifie ensuite que les valeurs valent oui et non respectivement.
*/
void test_reverse_hash(void)
{
	tab_mdp = (char**) malloc(2*sizeof(char*));
	if(tab_mdp == NULL)
	{
		return;
	}
	*(tab_mdp+0) = "non";
	*(tab_mdp+0) = "oui";

	for(int i=0; i<2;i++)
	{
		*(tab_mdp+i)=NULL;
	}
	reverse_hash();
	printf("le mot de passe 1 est %s\n", *(tab_mdp+0));
	printf("le mot de passe 2 est %s\n", *(tab_mdp+1));	

	CU_ASSERT_STRING_EQUAL(*(tab_mdp+0),"non");
	CU_ASSERT_STRING_EQUAL(*(tab_mdp+1),"oui");
	free(*(tab_hash+0));
	free(*(tab_hash+1));
	free(tab_hash);
}

/** La fonction test_insert() vide le deuxième buffer de 2 mdps pour remplir une liste chainée . Pour le vérifier, on initialise les cellules du deuxième buffer et on initialise la tête de lecture de la liste chaînée. Et on vérifie que la liste chainée contient oui 
*/
void test_insert(void)
{
	/*tab_mdp = (char**) malloc(2*sizeof(char*));
	if(tab_mdp == NULL)
	{
		return;
	}
	*(tab_mdp+0) = "non";
	*(tab_mdp+1) = "oui";*/

	

	head = (node**) malloc(sizeof(char*));
	if(head == NULL)
	{
		return;
	}

	insert_mdp();

	CU_ASSERT_STRING_EQUAL((*head)->mdp,"oui");	
	free(tab_mdp);
}





int main(int argc, char *argv[])
{

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



	CU_pSuite pSuite = NULL;
	if(CU_initialize_registry()!=CUE_SUCCESS) 
	{
		return CU_get_error();
	}
	
	pSuite = CU_add_suite("Mon catalogue", init_suite, clean_suite);
	if(NULL == pSuite)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}



	//4. Ajout des tests à la suite de tests
	if(CU_add_test(pSuite,"test lecture fichier", test_lectureFichier_insertion)==NULL ||
	CU_add_test(pSuite,"test reverse_hash", test_reverse_hash)==NULL ||
	CU_add_test(pSuite,"test insert", test_insert)==NULL)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}


	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();

	CU_basic_show_failures(CU_get_failure_list());
	
	CU_cleanup_registry();
	printf("\n");
	return CU_get_error();

	
}

