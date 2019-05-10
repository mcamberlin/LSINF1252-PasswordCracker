// Includes
#include <stdio.h> 		// pour utiliser fopen(), fputs(), fclose()
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>  		// pour utiliser read(), close()
#include <sys/types.h> 		// pour utiliser open()
#include <sys/stat.h> 		// pour utiliser open()
#include <fcntl.h>   		// pour utiliser open()
#include <semaphore.h> 		// pour les sémaphores
#include <pthread.h>  		// pour les threads 
#include "sha256.h"
#include "variables.h"
#include "reverse.h"
#include "lectureFichier.h"
#include "insert.h"


/** La fonction reverse_hash() est le consommateur du 1er producteur-consommateur et prend un hash contenu dans la ressource @tab_hash et calcul son inverse avec reversehash() et insère son résultat dans la ressource @tab_mdp.
	@pre - 
	@post - retourne EXIT_SUCCESS si l'ajout dans @tab_mdp du résultat de reversehash() s'est réalisé avec succès, EXIT_FAILURE sinon
*/
void* reverse_hash()
{
	while(!fin_de_lecture || nbreSlotHashRempli)
	// Tant que la lecture du fichier n'est pas finie ou que la ressouce @tab_hash n'est pas vide.
	{
		
		
		char* mdp = (char*) malloc(sizeof(char)*16);
		if(mdp == NULL)
		{
			fprintf(stderr, "Erreur allocation mémoire mdp dans affiche_hash\n");
			return (void*) EXIT_FAILURE;
		}

		uint8_t* hash;
		// début section critique
		sem_wait(&full_hash);
		pthread_mutex_lock(&mutex_hash);
		
		int conditionArret = 0; // Condition nécessaire pour arrêter de parcourir le tableau une fois qu'un slot rempli a été trouvé
		for(int i=0; i<N && !conditionArret; i++)
		{
			if(*(tab_hash+i)!=NULL) // cas où la case est remplie
			{
				conditionArret = 1;
				hash = (uint8_t*) *(tab_hash+i);
				//printf("Le pointeur de hash dans reverse_hash est %p \n",*(tab_hash+i));
				//free(*(tab_hash+i));
				*(tab_hash+i)=NULL;
				nbreSlotHashRempli--;
			}
		}

		pthread_mutex_unlock(&mutex_hash);
		sem_post(&empty_hash);
		// fin section critique

		printf("début reversehash() \n");
		pthread_mutex_lock(&mutex_hash);
		CalculExecution++;
		pthread_mutex_unlock(&mutex_hash);
		
		if( reversehash(hash, mdp, 16) ) // cas où reversehash() a trouvé le mdp originel
		{
			pthread_mutex_lock(&mutex_hash);
			printf("calculExecution-- il vaut %d\n", CalculExecution);
			CalculExecution--;
			pthread_mutex_unlock(&mutex_hash);
			printf("le hash affiché grace à reverse_hash est %s\n\n", mdp);
			
			// début section critique
			printf("Début section critique producteur reversehash() \n");
			sem_wait(&empty_mdp);
			pthread_mutex_lock(&mutex_mdp);

			// Chercher de la place dans le tableau pour ajouter
			int place_trouvee = 0;
			for(int i=0; i<N  && !place_trouvee; i++)
			{
				if(*(tab_mdp+i)==NULL) //cas où la case est vide
				{
					place_trouvee = 1;
					*(tab_mdp+i) = mdp;
					printf("L'adresse dans tab_mdp est : %p à l'indice %d (reverse consommateur)\n", *(tab_mdp+i), i);
					
					nbreSlotMdpRempli++;
				}
			}

			
			// fin section critique
			pthread_mutex_unlock(&mutex_mdp);
			sem_post(&full_mdp);
			printf("Fin section critique producteur reversehash\n");
		}
		else // cas où reversehash() n'a pas trouvé le mdp originel
		{
			pthread_mutex_lock(&mutex_hash);
			printf("calculExecution--\n");
			CalculExecution--;
			pthread_mutex_unlock(&mutex_hash);
			printf("Pas de mot de passe en clair trouvé pour ce hash\n ");
		}
		//freeter ici
		
	}
	printf("fin reverse_hash() \n");
	return EXIT_SUCCESS;
}

