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

/*-------------------Lecture de fichier ----------------------------*/

/** La fonction lectureFichier() lis par 32 bytes le(s) fichier(s) binaire(s) dans @fichiersEntree et remplit au fur et à mesure la ressource @tab_hash de hash. 
	@pre - 
	@post   - retourne EXIT_SUCCESS si la lecture et l'ajout s'est réalisé avec succès, EXIT_FAILURE sinon
*/
void *lectureFichier()
{
	for(int i=0; i<nbreFichiersEntree;i++)
	{
		fin_de_lecture = 0;

		
		char* fichier = (char*) malloc(sizeof(fichiersEntree[i])); 
		if(fichier==NULL)
		{
			fprintf(stderr, "Erreur allocation mémoire pour fichier\n");
			return (void*) EXIT_FAILURE;
		}
		strcpy(fichier,fichiersEntree[i]); 
		
		// Déterminer le nombre maximal de threads de calcul
		struct stat stat_fichier;
		if(stat(fichier, &stat_fichier)==-1)
		{
			fprintf(stderr, "Erreur stat fichier\n");
			return (void*) EXIT_FAILURE;
		}
		int nbre_de_hash = (stat_fichier.st_size)/32;
		if(nbre_de_hash<nbreThreadsCalcul) // Si le nbre threads de calcul > nbre de hash, alors, nbre threads de calcul = nbre hash
		{
			nbreThreadsCalcul = nbre_de_hash;
			N = nbreThreadsCalcul*2;
		}

		// Ouvrir le ième fichier binaires
		int fd = open((char*)fichier, O_RDONLY);
		if(fd ==-1)
		{
			fprintf(stderr, "Erreur d'ouverture dans lectureFichier() \n");
			return (void*) EXIT_FAILURE;
		}

		hash* ptr = (hash*) malloc(sizeof(hash));
		if(ptr==NULL)
		{
			fprintf(stderr, "Erreur malloc allocation mémoire ptr dans lectureFichier()\n");
			close(fd);
			return (void*) EXIT_FAILURE;
		}

		int r = read(fd, ptr,sizeof(hash));
		if(r==-1)
		{
			fprintf(stderr, "Erreur de lecture dans lectureFichier() \n");
			free(ptr);
			close(fd);
			return (void*) EXIT_FAILURE;
		}

		while(!fin_de_lecture) //tant que la lecture du fichier binaire @fichier n'est pas terminée
		{
			//Début section critique
			sem_wait(&empty_hash);
			pthread_mutex_lock(&mutex_hash);

			// Chercher de la place dans le tableau pour ajouter
			int place_trouvee = 0;
			for(int i=0; i<N  && !place_trouvee; i++)
			{
				if(*(tab_hash+i)==NULL) //si la case est vide
				{
					place_trouvee=1;
					hash* ptrhash = (hash*) malloc(sizeof(hash));
					if(ptrhash == NULL)
					{
						fprintf(stderr, "Erreur malloc allocation mémoire ptrhash dans lectureFichier() \n");
						close(fd);
						free(ptr);
						return (void*) EXIT_FAILURE;
					}
					memcpy(ptrhash, ptr, sizeof(hash));
					*(tab_hash+i)=ptrhash;
					printf("L'adresse dans tab_hash est : %p à l'indice %d (lecture)\n", *(tab_hash+i), i);
					
					nbreSlotHashRempli++;
				}
			}

			// Fin section critique
			pthread_mutex_unlock(&mutex_hash);
			sem_post(&full_hash);

			//lecture du hash suivant
			r = read(fd, ptr, sizeof(hash));
			if(r==0)
			{
				// pthread_mutex_lock(&mutex_hash); //Me - utilité ?
				printf("le nombre de bytes restant est %d\n", r);
				fin_de_lecture=1;
				// pthread_mutex_unlock(&mutex_hash); //Me - utilité ?
			}
		}
		free(ptr);
		close(fd);
		printf("Fin de la lecture du %d ème fichier dans lectureFichier()\n",i+1);
	}

	printf("Fin de la lecture lectureFichier()\n");
	return EXIT_SUCCESS;
}
