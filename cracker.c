/*
	Fichier cracker.c réprésente le code dans son intégralité
	
	Auteurs: 
		- CAMBERLIN Merlin
		- PISVIN Arthur
	Version: 10-09-19

	Commandes à indiquer dans le compilateur:
		- cd "~/Documents/LSINF1252-PasswordCracker-Gr118-2019"
		- gcc -o cracker cracker.c
		- ./cracker

	Commandes git:
		- git status
		- git add monfichier
		- git commit -m "blablabla" monfichier
		- git push
	Remarques:
		- Ne pas oublier de faire les docstring à chaque fois
*/



#include <stdio.h>  
#include <stdlib.h>
#include <string.h> // include pour utiliser la fonction strstr() semblalbe à contains()



//Arguments des fonctions
#define arg_t = "-t';
#define arg_c = "-c";
#define arg_o = "-o";


/** La fonction main est la fonction principale de notre programme:
	@pre - argc = nombre d'arguments passés lors de l'appel de l'exécutable
		- argv = tableau de pointeurs reprenant les arguments donnés par l'exécutable
	@post - exécute les différentes instructions du code
	
*/
int main(int argc, char *argv[]) {

// 1ere étape : lecture des fichiers binaires

	// 1.1 lecture des arguments de la commande de l'exécutable:
	if(argc ==1)
	{
		printf("Aucun argument de la fonction n'a été donné \n");
	}
	else
	{
		printf("argc: %d \n",argc);
		printf("Le nom de l'exécutable est : %s \n",argv[0]);
		for (int i=1; i < argc; i++) 
		{
			printf("Argument %d : %s \n", i+1, argv[i]);
		}	
	}
	
	int nbrThreadCalcul = 1; // par défaut, il vaut 1
	int occurenceVoyelles = 1; // par défaut, le critère de sélection est sur le nombre d'occurences des voyelles
	char[] nomFichierSortie;

	if( strstr(&argv,arg_t) == NULL)
	{
		
	}
	
	

	

	
	if(strstr(,) == NULL)
	{// cas ou aucune

	} //char *strstr(const char *haystack, const char *needle)

	
	
	// Initialisation un thread par type de fichier
	// Un pour ceux provenant d'internet
	// Un pour ceux provenant du disque dur
	// ...

	/** La fonction contains vérifie si le str2 est contenu dans le str1
		@pre - str1 = pointeur vers une chaîne de caractères 
			- str2 = pointeur vers une chaîne de caractères plus petite que str1
		@post - retourne 1 si str2 est compris dans str1
			retourne 0 sinon
	*/	
	
        return EXIT_SUCCESS; 
}
