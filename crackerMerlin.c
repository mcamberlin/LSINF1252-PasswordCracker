#include <stdio.h> // pour utiliser fopen()
#include <stdlib.h>
#include <string.h> // pour utiliser la fonction strstr() semblable à contains()
#include <unistd.h>  // pour utiliser read(), close(), write()
#include <sys/types.h> // pour utiliser open()
#include <sys/stat.h> // pour utiliser open()
#include <fcntl.h>   // pour utiliser open()

/** La fonction printList() affiche la liste chainée remplie des 
	@pre - 
	@post - 0 si l'affichage de la liste chaînée s'est réalisée avec succès, -1 sinon

*/
/*
int printList(node** head)
{
	if(head == NULL) // argument vide
	{
		fprintf(stderr, "**head non spécifié dans printList() \n");
		return -1;
	}
	if(*head == NULL) // liste vide
	{
		fprintf(stderr, "La liste chaînée de mdp est vide \n");
		return 0;
	}

	if(sortieStandard == 1) // si il faut écrire sur la sortie standard
	{
		node* runner = *head;
		while(runner != NULL)
		{
			printf("%s \n",runner->mdp);
			runner = runner->next;
		}
		return 0;
	}
	else // si il faut écrire dans le fichier @fichierSortie
	{

		int fd = open(fichierSortie,O_RDONLY|O_CREAT,O_RDONLY);
		if(fd ==-1)// cas ou open a planté
		{
		    return -1;
		}
		
		node* runner = *head;

		void* buf;
		int err;
		while(runner != NULL)
		{
			buf = malloc(sizeof(runner->mdp));
			if(buf == NULL) // cas où malloc a planté
			{
				fprintf(stderr, "Erreur allocation de mémoire pour @buf dans printList() \n");
				return -1;
			}
			strcpy(buf,runner->mdp);
			err = (int) write(fd, buf, sizeof(runner->mdp));
			if(err == -1)
			{
				fprintf(stderr, "Erreur dans write() \n");
				return -1;
			}
			//printf("%s \n",runner->mdp);
			runner = runner->next;
			free(buf);
		}

		if(close(fd) ==-1)
		{
			fprintf(stderr, "Erreur fermeture du fichier dans printList() \n");
		    	return -1;
		}
		printf("Fin printList() \n");
		return 0;
	}
}	
*/
int main(int argc, char *argv[]) 
{
	int fd = open(argv[1],O_RDONLY|O_CREAT,O_RDONLY);
	if(fd ==-1)// cas ou open a planté
	{
	    return -1;
	}
	int z = 23;
	strcpy(buf,&z);

	int err = (int) write(fd, buf, sizeof(int));
	if(err == -1)
	{
		fprintf(stderr, "Erreur dans write() \n");
		return -1;
	}

	free(buf);


	if(close(fd) ==-1)
	{
		fprintf(stderr, "Erreur fermeture du fichier dans printList() \n");
	    	return -1;
	}
	printf("Fin printList() \n");
	return EXIT_SUCCESS;
}
