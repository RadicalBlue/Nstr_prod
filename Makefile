CC=gcc
OPT=-Wall -lpthread -lrt -ggdb
All:Product_Auto

Product_Auto : main.c machine.o robot_alimentation.o robot_retrait.o superviseur.o erreur.o liste.o
	${CC} ${OPT} -o Product_Auto main.c -g machine.o robot_alimentation.o robot_retrait.o superviseur.o erreur.o liste.o

machine.o : machine.h machine.c
	${CC} ${OPT} -c machine.c

robot_alimentation.o : robot_alimentation.c robot_alimentation.h
	${CC} ${OPT} -c robot_alimentation.c

robot_retrait.o : robot_retrait.c robot_retrait.h
	${CC} ${OPT} -c robot_retrait.c

superviseur.o : superviseur.c superviseur.h
	${CC} ${OPT} -c superviseur.c

erreur.o : erreur.c erreur.h
	${CC} ${OPT} -c erreur.c

liste.o : liste.c liste.h
	${CC} ${OPT} -c liste.c

testListe : test.c liste.o erreur.o
	${CC} ${OPT} -o testListe test.c -g liste.o erreur.o

test_thread : test_thread.c
	${CC} ${OPT} -o test_thread test_thread.c

clean :
	rm *.o  Product_Auto testListe
