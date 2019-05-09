/*
	Fichier testCUnit.c est le fichier qui teste le fonctionnement de la fonction lectureFichier()

	Auteurs:
		- CAMBERLIN Merlin
		- PISVIN Arthur
	Version: 9-05-19
	
	Commandes: 
		- echo -n oui | sha256sum | xxd -r -p - "output.bin"

	Arguments à spécifier:
		- -I${HOME}/local/include
		- -lcunit
		- -L${HOME}/local/lib

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
//#include <Cunit.h>


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



/** La fonction test_lectureFichier_true() vérifie que lectureFichier() ne retourne pas une erreur (void*) EXIT_FAILURE.
*/
void test_lectureFichier_true(void)
{	
	fichierTest1 = "test-input/02_6c_5.bin";
	fichiersEntree = &fichierTest1;
	CU_ASSERT_NOT_EQUAL(lectureFichier(),(void*) EXIT_FAILURE);
}

/** La fonction test_lectureFichier_insertion() vérifie que lectureFichier() remplit le premier buffer de 5 pointeurs. Pour le vérifier, on initialise les cellules du buffer à NULL et on vérifie ensuite que leurs valeurs a été modifiées.
*/
void test_lectureFichier_insertion(void)
{
	char fichierTest1[] = "test-input/02_6c_5.bin";
	fichiersEntree = &fichierTest1;
	hash** tab_hash = (hash**) malloc(5*sizeof(hash*));
	if(tab_hash == NULL)
	{
		return NULL;
	}
	for(int i=0; i<5;i++)
	{
		*(tab_hash+i)=NULL;
	}
	lectureFichier();

	for(int i=0; i<5;i++)
	{
		CU_ASSERT_NOT_EQUAL(*(tab_hash+i),NULL);
		free(*(tab_hash+i));			
	}
	free(tab_hash);
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





	if(CUE_SUCCESS != CU_initialize_registry()) // Initialisation du catalogue
	{
		return CU_get_error();
	}
	CU_pSuite pSuite = NULL;
	pSuite = CU_add_suite("Mes tests",NULL,NULL);
	if(pSuite == NULL)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}
}

/*
void test_string_equals(void)
{
  CU_ASSERT_STRING_EQUAL("string #1", "string #1");
}

void test_failure(void)
{
  CU_ASSERT(false);
}

void test_string_equals_failure(void)
{
  CU_ASSERT_STRING_EQUAL("string #1", "string #2");
}
*/

