#include <CUnit/CUnit.h>

// Includes pour utiliser nos fonctions
#include "variables.h"
#include "insert.h" 		
#include "reverse_hash.h" 	
#include "lectureFichier.h"





void test_lectureFichier()
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


