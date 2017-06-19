compiler = gcc
flags = -O3 -Wall -std=c99

main: main.o population.o chromosome.o functions.o
	$(compiler) $(flags) $^ -o $@

main.o: main.c chromosome.h global.h
	$(compiler) $(flags) -c main.c

population.o: population.c population.h chromosome.h global.h
	$(compiler) $(flags) -c population.c

chromosome.o: chromosome.c functions.h chromosome.h global.h
	$(compiler) $(flags) -c chromosome.c

functions.o: functions.c functions.h
	$(compiler) $(flags) -c functions.c

clean:
	rm -f main *.o
