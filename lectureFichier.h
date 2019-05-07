#ifndef _LECTURE_FICHIER
#define _LECTURE_FICHIER
typedef struct hash
{
	char hash[32];
}hash;

extern int fin_de_lecture;
extern int N;
extern int nbreThreadsCalcul;
extern int nbreFichiersEntree;
extern char** fichiersEntree;
extern int nbreThreadsCalcul;
extern int nbreSlotHashRempli;
extern hash** tab_hash;

void *lectureFichier();
#endif
