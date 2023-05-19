
PROJECT = qb
HEADERS = qb.h
OBJ = piping.c QBFunctions.c QBStreamClient.c 
C11 = gcc -g -D_POSIX_C_SOURCE=200809L -std=c11 -Werror

qb-network : QBStreamClient.o QBFunctions.o Piping.o globals.o 
	$(C11) -o qb \
	QBStreamClient.o QBFunctions.o Piping.o globals.o -lm

QBStreamClient.o: QBStreamClient.c QBFunctions.c Piping.c globals.c $(HEADERS)
	$(C11) -c QBStreamClient.c

QBFunctions.o: QBFunctions.c Piping.c globals.c $(HEADERS)
	$(C11) -c QBFunctions.c

Piping.o: Piping.c globals.c $(HEADERS)
	$(C11) -c Piping.c

globals.o: globals.c $(HEADERS)
	$(C11) -c globals.c 

	

clean:
	rm -f $(PROJECT) $(OBJ) *.temp
