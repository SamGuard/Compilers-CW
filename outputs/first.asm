.text
.globl  main
main:
addi $sp, $sp, -12
add $8, $0, 1
sw $8, 0($29)
add $8, $0, 2
sw $8, 4($29)
lw $8, 0($29)
lw $9, 4($29)
add $10, $8, $9
sw $10, 8($29)
add $8, $0, 0
sw $8, 0($29)
addi $sp, $sp, 12
li $v0, 10
syscall