all:
	g++ main.cpp syntax.cpp validator.cpp compile.cpp state.cpp -o alias

asm:
	nasm -f elf32 program.asm -o program.o
	gcc -m32 program.o -no-pie -o program