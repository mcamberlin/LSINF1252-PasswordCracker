/*
	Fichier cracker.c réprésente le code dans son intégralité

	Auteurs:
		- CAMBERLIN Merlin
		- PISVIN Arthur
	Version: 10-09-19

	Commandes à indiquer dans le compilateur:
		- cd ~/Documents/LSINF1252-PasswordCracker-Gr118-2019
		- gcc -o cracker cracker.c
		- ./cracker arg1 arg2 arg3

	Commandes git:
		- git status
		- git add monfichier
		- git commit -m "blablabla" monfichier
		- git commit -i -m "blablabla" monfichier // en cas de fusion de conflits
		- git push
	Remarques:
		- Ne pas oublier de faire les docstring à chaque fois
*/



#include <stdio.h>  
#include <stdlib.h>
#include <string.h> // include pour utiliser la fonction strstr() semblalbe à contains()


/** La fonction main est la fonction principale de notre programme:
	@pre - argc = nombre d'arguments passés lors de l'appel de l'exécutable
		- argv = tableau de pointeurs reprenant les arguments donnés par l'exécutable
	@post - exécute les différentes instructions du code

*/
int main(int argc, char *argv[]) {

// tête de la fonction : ./cracker [-t NTHREADS] [-c] [-o FICHIEROUT] FICHIER1 [FICHIER2 ... FICHIERN]

	/* 1e étape : lecture des fichiers binaires
			Initialisation un thread par type de fichier
			Un pour ceux provenant d'internet
			Un pour ceux provenant du disque dur
			...
	*/


	// 1.1 lecture des arguments de la commande de l'exécutable
	
	//Arguments des fonctions
	char arg_t[] = "-t";
	char arg_c[] = "-c";
	char arg_o[] = "-o";

	//Valeurs par défaut
	int nbreThreadCalcul = 1;

	printf("%d-1 arguments spécifié(s) \n",argc);

	for (int i=1; i < argc; i++) 
	{
		if( strstr(argv[i],arg_t) != NULL)
		{// cas où l'exécutable spécifie l'argument -t
			printf("On a spécifié -t \n");
			nbreThreadCalcul = (int) *argv[2];
			printf("Le nombre de thread de calcul est : %d \n",nbreThreadCalcul);
		}   
		
	}	
 

	/*fichier = fopen (argv[1], "r");  On ouvre le fichier dont le chemin est accessible via argv[1] 
	*/





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

        printf ("\n \n \nFin du programme...\n"); 
        return EXIT_SUCCESS; 
}
