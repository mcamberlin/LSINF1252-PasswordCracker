#include <stdio.h>

const char consonnes[] = {'b','c','d','f','g','h','j','k','l','m','n','p','q','r','s','t','v','w','x','z'};
const char voyelles[] = {'a','e','i','o','u','y'};
 

/** La fonction count_consonants() compte le nombre de consommes dans une chaine de caractères
	@pre - monString = un pointeur vers une chaîne de caractères dont on souhaite compter le nombre de consonnes
	@post - retourne le nombre de consonnes dans monString

*/
int count_consonants(char* monString)
{
	int nbreConsonnes = 0;
	int j=0;	
	for(int i=0;*(monString+i) !='\0';i++) // tant que on a pas vérifié chaque lettre de monString
	{
		j=0;
		while(j<20 && *(monString+i) != consonnes[j])
		{
			j++;	
		}
		if(*(monString+i) == consonnes[j])
		{
			nbreConsonnes++;
		}
	}
	return nbreConsonnes;
}

/** La fonction count_vowels() compte le nombre de voyelles dans une chaine de caractères
	@pre - monString = un pointeur vers une chaîne de caractères dont on souhaite compter le nombre de voyelles
	@post - retourne le nombre de voyelles dans monString

*/
int count_vowels(char* monString)
{
	int nbreVoyelles = 0;
	int j=0;
	for(int i=0;*(monString+i) != '\0';i++)// tant que on a pas vérifié chaque lettre de monString
	{
		j=0;
		//printf("*(monString+i) : %c \n",*(monString+i));
		while(j<6 && *(monString+i) != voyelles[j])
		{
			//printf(" voyelles[j] : %c , *(monString+i) : %c \n",voyelles[j],*(monString+i));
			j++;
		}
		if(voyelles[j] == *(monString+i))
		{
			nbreVoyelles++;
		}
	}
	return nbreVoyelles;
}

int main(int argc, char *argv[]) 
{
	// tests
	char monString1[] = "fermier";
	int nbreVoyelles1 =  count_vowels(monString1);
	int nbreConsonnes1 = count_consonants(monString1);
      
    


	printf("Il y a %d voyelles dans %s \n",nbreVoyelles1,monString1);
	printf("Il y a %d consonnes dans %s \n",nbreConsonnes1,monString1);
}

