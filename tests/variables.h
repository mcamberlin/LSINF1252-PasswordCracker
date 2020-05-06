#ifndef _VARIABLE
#define _VARIABLE


typedef struct hash
{
	char hash[32];
}hash;


// CONSTANTES
extern const int LENPWD; // Nbre maximal de caractères dans les mots de passes originels
extern const char RETOUR_LIGNE;
extern const char CONSONNES[];
extern const char VOYELLES[];

typedef struct node
{
	char mdp[16];
	struct node *next;
}node;


extern int critereVoyelles;
extern int occurenceVoyelles;
extern int occurenceConsonnes;

extern node** head;

extern int sortieStandard;
extern char* fichierSortie;

extern int fin_de_lecture;
extern int N;
extern int nbreThreadsCalcul;
extern int nbreFichiersEntree;
extern char** fichiersEntree;

extern int CalculExecution;


extern sem_t empty_hash;
extern sem_t full_hash;
extern pthread_mutex_t mutex_hash;

extern sem_t full_mdp;
extern sem_t empty_mdp;
extern pthread_mutex_t mutex_mdp;

extern hash** tab_hash;
extern char** tab_mdp;

extern int nbreSlotMdpRempli;
extern int nbreSlotHashRempli;


#endif






