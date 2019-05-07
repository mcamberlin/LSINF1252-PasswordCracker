#ifndef _VARIABLE
#define _VARIABLE
// CONSTANTES
const int LENPWD = 16; // Nbre maximal de caractères dans les mots de passes originels
const char RETOUR_LIGNE = '\n';
const char CONSONNES[] = {'b','c','d','f','g','h','j','k','l','m','n','p','q','r','s','t','v','w','x','z'};
const char VOYELLES[] = {'a','e','i','o','u','y'};

// Initialisation du mutex et des 2 sémaphores pour protéger la ressource tab_mdp
pthread_mutex_t mutex_mdp;
sem_t empty_mdp; 
sem_t full_mdp; 


// Valeurs par défaut
/*
extern int fin_de_lecture;
extern int N;
extern int nbreThreadsCalcul;
extern int nbreFichiersEntree;
extern char** fichiersEntree;
extern int nbreThreadsCalcul;
extern int nbreSlotHashRempli;
extern hash** tab_hash;
*/
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


typedef struct node
{
	char mdp[16];
	struct node *next;
}node;

/*
extern int critereVoyelles;
extern int occurenceVoyelles;
extern int occurenceConsonnes;
*/
extern node** head;


/*
extern int CalculExecution;
extern int fin_de_lecture;
extern int nbreSlotHashRempli;

extern sem_t empty_hash;
extern sem_t full_hash;
extern pthread_mutex_t mutex_hash;

extern sem_t full_mdp;
extern sem_t empty_mdp;
extern pthread_mutex_t mutex_mdp;

extern char** tab_mdp;
extern int nbreSlotMdpRempli;
*/

typedef struct hash
{
	char hash[32];
}hash;

// Déclaration d'un tableau de pointeurs de hash
hash** tab_hash;

// Déclaration d'un tableau de pointeurs de mots de passe 
char** tab_mdp;

// Déclaration d'une la liste simplement chainée
node** head;

#endif






