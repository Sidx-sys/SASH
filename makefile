shell: main.o utils.o functions.o ls.o process.o
	mkdir -p obj
	gcc main.o utils.o functions.o ls.o process.o -o shell
	mv *.o ./obj
	chmod +x shell

main.o: ./src/main.c
	gcc -c ./src/main.c

utils.o: ./src/utils.c
	gcc -c ./src/utils.c

functions.o: ./src/functions.c
	gcc -c ./src/functions.c

ls.o: ./src/ls.c
	gcc -c ./src/ls.c

process.o: ./src/process.c
	gcc -c ./src/process.c

clean: 
	rm -r ./obj 
	rm ./shell