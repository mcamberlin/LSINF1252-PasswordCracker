ComilateurC = GCC #type de compilateur choisi
FLAG1 = -Wall # Activer tous les warnings
FLAG2 = -Werror # Considérer tous les warnings comme des erreurs
FLAG_THREAD = -lpthread #Pour les threads

cracker: cracker.c variables.h insert.h reverse_hash.h lectureFichier.h
	gcc -o cracker cracker.c lectureFichier.c reverse_hash.c insert.c sha256.c reverse.c $(FLAG1) $(FLAG2) $(FLAG_THREAD)

test: testCUnit.c reverse.c sha256.c lectureFichier.c insert.c
	gcc testCUnit.c reverse.c sha256.c lectureFichier.c insert.c -o test -I${HOME}/local/include -lcunit -L${HOME}/local/lib -lpthread
	./test

clean:
	rm cracker
