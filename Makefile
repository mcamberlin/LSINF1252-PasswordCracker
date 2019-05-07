ComilateurC = GCC
FLAG1 = -Wall # Activer tous les warnings
FLAG2 = -Werror # Considérer tous les warnings comme des erreurs
FLAG_THREAD = -lpthread

cracker:
	gcc -o cracker cracker.c reverse.c sha256.c -Wall -Werror -lpthread

all: craker.c reverse.h sha256.h
	@echo "Cible make \n"
	$(CompilateurC) -o cracker craker.c reverse.c sha256.c $(FLAG_THREAD) $(FLAG1) $(FLAG2)  
	
#make tests:
#	echo "Cible make tests \n"
	
#make all:
#	echo "Cible make all\n"
	
#make clean:
#	echo "Cible make clean\n"


