# makefile

program: main.o
	mpicc -std=c99 -lcrypt main.o -o main
	# mpicc -std=c99  main.o -o main
	mpirun -n 10 ./main

main.o: main.c
	mpicc  -std=c99 -c -lcrypt main.c
	# mpicc  -std=c99 -c main.c
val:
	valgrind mpirun -n  ./main

clean:
	rm -f *.o
	rm -f main
	rm main
	clear

