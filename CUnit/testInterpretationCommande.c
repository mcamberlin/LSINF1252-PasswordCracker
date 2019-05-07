//#include <CUnit/CUnit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>  		
#include <sys/types.h> 		
#include <sys/stat.h> 		
#include <fcntl.h>   		

// gcc testInterpretationCommande.c -o test -lcunit -I${HOME}/local/include -L${HOME}/local/lib


// Valeurs par défaut
int nbreThreadsCalcul = 1;
int N = 2;			// Le nombre de slot du buffer
int critereVoyelles = 1;	// true
int sortieStandard = 1;		// true
int nbreFichiersEntree = 1;	// Entier comptant le nombre de fichiers binaires donnés en entrée
char* fichierSortie;		// Pointeur vers le nom du fichier de sortie si -o est spécifié
char** fichiersEntree; 		// Tableau de pointeurs vers les fichiers d'entrée

typedef struct arg
{
	int nbreThreadsCalcul ;
	int N;			
	int critereVoyelles;
	int sortieStandard;		
	int nbreFichiersEntree;	
	char* fichierSortie;		
	char** fichiersEntree;
}arg;

arg* interpretation_commande(int argc, char *argv[])
{
	arg* mesArguments = (arg*) malloc(sizeof(struct arg));
	if(mesArguments == NULL)
	{
		
	}

	int opt;
	int index = 1; // index des fichiers binaires
	while (index<argc) // Tant que toutes les options n'ont pas été vérifées
	{
		printf("test\n");
		opt = getopt(argc, argv, "t:o:c");
		switch (opt) 
		{
			
			case 't':
				printf("ici \n");
				nbreThreadsCalcul = atoi(optarg); // conversion du tableau de caractères en int
				printf("%d \n",nbreThreadsCalcul);
				mesArguments->nbreThreadsCalcul = nbreThreadsCalcul;
				N = nbreThreadsCalcul*2;
				mesArguments->N = N;
				index+=2;
				break;
			case 'c':
				printf("ici 2 \n");
				critereVoyelles = 0;
				mesArguments->critereVoyelles = 0;
				index++;
				break;
			case 'o':
				printf("ici 3 \n");
				sortieStandard = 0;
				mesArguments->sortieStandard = sortieStandard;
				fichierSortie = optarg;
				mesArguments->fichierSortie = fichierSortie;
				index+=2;
				break;
			default: 
				printf("ici 4 \n");
				nbreFichiersEntree = argc - index;
				mesArguments->nbreFichiersEntree = nbreFichiersEntree;
				fichiersEntree = (char**) malloc(nbreFichiersEntree * sizeof(argv[index]));
				if(fichiersEntree == NULL)
				{	
					
				}

				for(int i=0;index<=(argc-1);index++,i++)
				{
					*(fichiersEntree+i)=argv[index];
					
				}
				mesArguments->fichiersEntree = fichiersEntree;
				break;
		}
	}
	printf("%d \n",mesArguments->nbreThreadsCalcul);

	return mesArguments;
	
}



void test_interpretation_commandes()
{
	char *argv[] = {"-t", "200", "-o","fichierOut","-c","fichierBinaire1","fichierBinaire2","fichierBinaire3"};
	int argc = sizeof(argv) / sizeof(char*) - 1;

	arg* arguments = interpretation_commande(argc, argv);
	printf("%d \n",(arguments->N));
	//CU_ASSERT_EQUAL(200,arguments->nbreThreadsCalcul); // vérifie que les deux arguments sont identiques
	
}



int main(int argc, char *argv[]) 
{	
	test_interpretation_commandes();
	return (int) EXIT_SUCCESS;
}


