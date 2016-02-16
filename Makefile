CC=gcc
OPT=-Wall -std=c99 -lthread

machine.o : machine.h machine.c
	${CC} ${OPT} -c machine.c

robot_alimentation.o : robot_alimentation.c robot_alimentation.h
	${CC} ${OPT} -c robot_alimentation.c

robot_retrait.o : robot_retrait.c robot_retrait.h
	${CC} ${OPT} -c robot_retrait.c

superviseur.o : superviseur.c superviseur.h
	${CC} ${OPT} -c superviseur.c

clean :
	rm *.o
