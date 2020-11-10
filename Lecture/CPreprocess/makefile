all:
	# Stop after the preprocessing stage; do not run the compiler proper.
	gcc -E main.c -o main.i
	# Stop after the stage of compilation proper; do not assemble.
	gcc -S main.i -o main.s
	# Compile or assemble the source files, but do not link.
	gcc -c main.s -o main.o
	gcc main.o -o main
clean:
	rm -f main.i main.s main.o main main.exe

