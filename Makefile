ComilateurC = GCC #type de compilateur choisi
FLAG1 = -Wall # Activer tous les warnings
FLAG2 = -Werror # Considérer tous les warnings comme des erreurs
FLAG_THREAD = -lpthread #Pour les threads

cracker:
	echo "Cible make"
	$(CompilateurC) -o cracker craker.c reverse.c sha256.c $(FLAG_THREAD) $(FLAG1) $(FLAG2)

all: craker.c reverse.h sha256.h
	@echo "Cible make \n"
	

mon_executable: sha256.o reverse.o cracker.o
	$(CompilateurC) -o cracker sha256.o reverse.o cracker.o $(FLAG_THREAD) $(FLAG1) $(FLAG2) 

sha256.o: sha256.c
	$(CompilateurC) -o sha256.o -c sha256.c $(FLAG1) $(FLAG2) 


make: craker.c reverse.c sha256.c reverse.h sha256.h   	 	
	 

make tests:
	echo "Cible make tests"

make all:
	echo "Cible make all"

make clean:
	echo "Cible make clean"

