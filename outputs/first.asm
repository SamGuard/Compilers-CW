.text
.globl  main
main:
addi $sp, $sp, -12
sw 1, 0($sp)
sw 2, 1($sp)
lw $8, 0($sp)
lw $9, 1($sp)
add $10, $8, $9
sw 10, 2($sp)
sw 0, 0($sp)
addi $sp, $sp, 12
li 10, $v0
syscall