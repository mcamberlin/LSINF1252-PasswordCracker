FLAG1 = -Wall # Activer tous les warnings
FLAG2 = -Werror # Considérer tous les warnings comme des erreurs
FLAG_THREAD = -lpthread #Pour les threads

cracker: src/cracker.c src/variables.h src/insert.h src/reverse_hash.h src/lectureFichier.h
	gcc -o cracker src/cracker.c src/lectureFichier.c src/reverse_hash.c src/insert.c src/sha256.c src/reverse.c $(FLAG1) $(FLAG2) $(FLAG_THREAD)

tests: cracker
	./cracker -t 2 tests/02_6c_5.bin
	./cracker -t 100 tests/01_4c_1k.bin
	./cracker -t 3 tests/nosTests.bin
	rm cracker

clean:
	rm cracker -f
	rm test -f
