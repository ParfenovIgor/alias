{
    def a 1 // 0 -> int 1 -> ptr
    a := alloc(5)
    def c 1
    c
     :=
    a +
    3
    def b 0
    b := c + 1
    a := b + 0
    assume (a = c + 1)
}
