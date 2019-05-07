#ifndef _REVERSE_HASH
#define _REVERSE_HASH_
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


void* reverse_hash();
#endif

