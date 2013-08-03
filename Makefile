task:task.o taskfactory.o
	gcc -o task task.o taskfactory.o -g

task.o:task.c taskfactory.h
	gcc -c task.c -g

taskfactory.o:taskfactory.h taskfactory.c
	gcc -c taskfactory.c -g

clean:
	rm *.o task
