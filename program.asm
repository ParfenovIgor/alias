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
; function definition
jmp _funend0
_fun0:
push ebp
mov ebp, esp
sub esp, 20
; block
; function definition
jmp _funend1
_fun1:
push ebp
mov ebp, esp
sub esp, 16
; block
; definition
sub esp, 4
; assignment
; addition
; identifier
mov eax, [ebp + 8]
mov [esp - 4], eax
sub esp, 4
; dereference
; identifier
mov eax, [ebp + 12]
mov [esp - 4], eax
mov eax, [esp - 4]
mov ebx, [eax]
mov [esp - 4], ebx
add esp, 4
mov eax, [esp - 4]
add eax, [esp - 8]
mov [esp - 4], eax
mov eax, [esp - 4]
mov [ebp + -4], eax
; movement
; identifier
mov eax, [ebp + -4]
mov [esp - 4], eax
mov eax, [esp - 4]
mov ebx, [ebp + 12]
mov [ebx], eax
leave
ret
_funend1:
; movement
; integer
mov [esp - 4], dword 0
mov eax, [esp - 4]
mov ebx, [ebp + 16]
mov [ebx], eax
; function call
push dword [ebp + 16]
push dword [ebp + 8]
call _fun1
add esp, 8
; function call
push dword [ebp + 16]
push dword [ebp + 12]
call _fun1
add esp, 8
leave
ret
_funend0:
leave
ret
