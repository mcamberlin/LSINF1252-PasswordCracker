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
		- A chaque malloc, ne pas oublier de free en cas d'erreur
*/


#include <stdio.h>  
#include <stdlib.h>
#include <string.h> // pour utiliser la fonction strstr() semblablee à contains()
#include <unistd.h>  // pour utiliser read(), close()
#include <sys/types.h> // pour utiliser open()
#include <sys/stat.h> // pour utiliser open()
#include <fcntl.h>   // pour utiliser open()


/** La fonction main est la fonction principale de notre programme:
	@pre - argc = nombre d'arguments passés lors de l'appel de l'exécutable
		- argv = tableau de pointeurs reprenant les arguments donnés par l'exécutable
	@post - exécute les différentes instructions du code

*/
int main(int argc, char *argv[]) {

// ./cracker [-t NTHREADS] [-c] [-o FICHIEROUT] FICHIER1 [FICHIER2 ... FICHIERN]

	/* 1e étape : 
			1.1 lecture des arguments de la commande de l'exécutable [FAIT]
			1.2 ouverture des fichiers binaires [En cours - Merlin]
			1.3 Initialisation un thread par type de fichier
	*/
	
	//Arguments des fonctions
	char arg_t[] = "-t";
	char arg_c[] = "-c";
	char arg_o[] = "-o";

	//Valeurs par défaut
	int nbreThreadsCalcul = 1; 
	int critereVoyelles = 1;
	int sortieStandard = 1;
	int nbrFichiersEntree = 0;

	// considération du cas dans lequel les arguments de l'exécutable sont valides.
	for (int i=1; i < argc; i++) 
	{

		if( strstr(argv[i],arg_t) != NULL) // cas où argument -t spécifié
		{
			nbreThreadsCalcul = atoi(argv[i+1]); // conversion du tableau de caractères en int ! risque d'erreur
			i+=1;
			printf("-t spécifié : nombre de threads de calcul = %d ;\n",nbreThreadsCalcul);
		}   
		else if (strstr(argv[i],arg_c) != NULL) // cas où argument -c spécifié
		{
			critereVoyelles = 0;
			printf("-c spécifié : critère de sélection = occurence des consonnes ;\n");

		}		
		else if (strstr(argv[i],arg_o) != NULL)// cas où argument -o spécifié
		{
			sortieStandard = 0;
			char* fichierSortie = (char*) malloc(sizeof(argv[i+1]));
			if(fichierSortie == NULL) // cas où malloc a planté
			{ 	
				free(fichierSortie);
				printf("Erreur malloc cas où argument -o spécifié");
				return EXIT_FAILURE;
			}
			printf("-o spécifié : %s\n",argv[i+1]);
			i+=1;
		}
		else
		{
			nbrFichiersEntree+=1;
			printf("- fichier(s) binaires d'entree = %s ;\n",argv[i]);
		}
	}	
	printf("	---------	--------	--------	--------	");

	/*
De Coninck Quentin : """ La partie la plus importante à paralléliser n'est pas la lecture des fichiers,
 et si vous justifiez correctement pourquoi votre implémentation dévie du design initial, c'est bon. 
La gestion de plusieurs sources en parallèle peut être vue comme une optimisation. Ne considérez donc 
ce point qu'à la fin de votre projet, lorsque celui-ci fonctionne correctement et parallélise bien 
avec les threads de calcul. """
	*/

	for(int i=argc-nbrFichiersEntree; i<nbrFichiersEntree;i++) // tant que tous les fichiers n'ont pas été lu
	{
		int ouverture = open(argv[i],O_RDONLY); 
 
		if(ouverture ==-1) // cas où open plante
		{
			printf("Erreur open dans l'ouverture du fichier");
			return EXIT_FAILURE;
		}
		
		//2. lire le fichier
		size_t nbreOctetsHash = (size_t) 32;
		void* buf = malloc(nbreOctetsHash);
		if(buf == NULL)  //cas où malloc plante
		{
			printf("Erreur malloc dans la réservation de mémoire du buffer");
			free(buf);
			return EXIT_FAILURE;
		}
		
		ssize_t lecture = read(ouverture, &buf, nbreOctetsHash); 

		if(lecture == -1) // cas où read plante
		{
			printf("Erreur open dans l'ouverture du fichier");
			//3. Fermer le fichier
			if(close(ouverture) !=0)  // cas où close plante
			{
				printf("Erreur close dans la fermeture du fichier");
				free(buf);
				return EXIT_FAILURE;
			}
			free(buf);
			return EXIT_FAILURE;
		
		}
		while(lecture == nbreOctetsHash) // tant que read se deroule bien
		{
			// creation d'un thread

			lecture = read(ouverture, &buf,nbreOctetsHash);            
		}

		//3. Fermer le fichier
		if(close(ouverture) !=0) // cas où close plante
		{    
			printf("Erreur close dans la fermeture du fichier");
			free(buf);
			return EXIT_FAILURE;
		}
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
	Si plus grand nombre d'occurence : on supprime la liste chainée existance pour la remplacer par le nouveau MdP
	*/

	/* 6e étape : quand tous les threads ont fini de s'executer, affiche sur stdout ou écrit dans
	FICHIEROUT la liste chainée qu'il reste
	*/
        printf ("\n \n \nFin du programme...\n"); 
        return EXIT_SUCCESS; 
}
