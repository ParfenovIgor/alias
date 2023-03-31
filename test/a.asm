global main
extern malloc
extern free

section .data
chars db "a", "b", "c"

section .text
main:

mov eax, 4
mov ebx, 1
mov ecx, chars
push 2
mov edx, [esp - 4]
pop esi
int 80h

mov eax,1            
mov ebx,0            
int 80h