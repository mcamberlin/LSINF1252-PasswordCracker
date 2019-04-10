/*
	Fichier cracker.c réprésente le code dans son intégralité

	Auteurs:
		- CAMBERLIN Merlin
		- PISVIN Arthur
	Version: 10-09-19

	Commandes à indiquer dans le compilateur:
		- cd "~/Documents/LSINF1252-PasswordCracker-Gr118-2019"
		- gcc -o cracker cracker.c
		- ./cracker arg1 arg2 arg3

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

// tête de la fonction : ./cracker [-t NTHREADS] [-c] [-o FICHIEROUT] FICHIER1 [FICHIER2 ... FICHIERN]

	/* 1ere étape : lecture des fichiers binaires
	Initialisation un thread par type de fichier
	Un pour ceux provenant d'internet
	Un pour ceux provenant du disque dur
	...
	*/


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

	if( strstr(&argv,arg_t) == NULL) //char *strstr(const char *haystack, const char *needle)
	{
		
	}
	 

	/* 2e étape : création du tableau contenant les hash (taille : il avait dit +/- 2 fois le nombre de
	threads de calcul) DO NOT FORGET -> 2 sémaphores et 1 mutex
	*/

	/* 3e étape : les threads de calculs de reverse
	*/

	/* 4e étapes : création du tableau contenant les MdP (taille : même que pour les hach ?)
	*/

	/* 5e étape : comparaison du MdP qui sort du tableau avec les autres MdP dans une liste chainées.
	Si plus petit nombre d'occurence dans le nouveau MdP : on ne fait rien
	Si même nombre d'occurence : on ajoute le MdP à la liste chainées
	Si plus grand nombre d'occurence : on supprime la liste chainée existance pour la remplacer par le
	nouveau MdP
	*/

	/* 6e étape : quand tous les threads ont fini de s'executer, affiche sur stdout ou écrit dans
	FICHIEROUT la liste chainée qu'il reste
	*/

        printf ("Hello, World!\n"); 
        return EXIT_SUCCESS; 
}
