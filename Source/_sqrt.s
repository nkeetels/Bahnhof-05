.global _sqrt
.align 2
.thumb_func
_sqrt:
swi 0x8
bx lr