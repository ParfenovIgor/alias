{
    /*func print(a int) {
        asm {
            jmp afterbuffer1
            buffer1 db "%d", 0xA, 0x0
            afterbuffer1:
            push dword [ebp + 8]
            push buffer1
            call printf
            add esp, 0x8
        }
    }

    func addition(a int, b int, c ptr 1 1) {
        func helper(a int, c ptr 1 1) {
            def b int
            b := a + $c
            c <- b
        }
        c <- 0
        call helper(a, c)
        call helper(b, c)
    }*/

    include {inc.al}

    d := d + 2

    /*def a int ; a := 3
    def b int ; b := 31
    def c ptr ; c := alloc(1)

    call print(a)

    call addition(a, b, c)
    a := $c
    call print(a)
    
    a := 5

    while (a) {
        call print(a)
        a := a + -1
    }

    free(c)*/
}