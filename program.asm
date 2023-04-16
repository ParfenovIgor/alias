; program
global main
extern malloc
extern free
extern printf
section .text
main:
push ebp
mov ebp, esp
; block
; definition
sub esp, 4
; assignment
; integer
mov [esp - 4], dword 4
mov eax, [esp - 4]
mov [ebp + -4], eax
; assignment
; addition
; identifier
mov eax, [ebp + -4]
mov [esp - 4], eax
sub esp, 4
; integer
mov [esp - 4], dword 5
add esp, 4
mov eax, [esp - 4]
add eax, [esp - 8]
mov [esp - 4], eax
mov eax, [esp - 4]
mov [ebp + -4], eax
leave
ret
