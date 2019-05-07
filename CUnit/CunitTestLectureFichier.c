/*
	Fichier CUnitTestLectureFichier est le fichier qui teste le fonctionnement de la fonction lectureFichier()

	Auteurs:
		- CAMBERLIN Merlin
		- PISVIN Arthur
	Version: 06-05-19 - 

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

#include <CUnit.h>

/** La fonctio test_lectureFichier_true() vérifie que la valeur est de retour est non-nulle (true).
*/
void test_lectureFichier_true(void)
{	
	int lectureFichier()	
	CU_ASSERT(true);
}

void test_assert_2_not_equal_minus_1(void)
{
  CU_ASSERT_NOT_EQUAL(2, -1);
}

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


