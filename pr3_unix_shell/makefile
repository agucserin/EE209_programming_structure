ish: main.o execution.o parsing.o dynarray.o
	gcc209 main.o execution.o dynarray.o parsing.o -o ish

main.o: main.c execution.h parsing.h dynarray.h
	gcc209 -c main.c

parsing.o: parsing.c parsing.h dynarray.h
	gcc209 -c parsing.c

execution.o: execution.c execution.h dynarray.h
	gcc209 -c execution.c

dynarray.o: dynarray.c dynarray.h
	gcc209 -c dynarray.c
