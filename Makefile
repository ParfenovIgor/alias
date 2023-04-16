all:
	g++ main.cpp syntax.cpp validator.cpp compile.cpp process.cpp settings.cpp -o calias

asm:
	nasm -f elf32 program.asm -o program.o
	gcc -m32 program.o -no-pie -o program