.text
.globl  main
main:
addi $sp, $sp, -16
add $8, $0, 1
sw $8, 0($29)
add $8, $0, 2
sw $8, 4($29)
lw $8, 0($29)
lw $9, 4($29)
add $10, $8, $9
sw $10, 8($29)
lw $8, 8($29)
add $8, $0, $8
sw $8, 0($29)
lw $8, 0($29)
add $10, $8, 1
sw $10, 12($29)
lw $8, 12($29)
add $8, $0, $8
sw $8, 0($29)
addi $sp, $sp, 16
li $v0, 10
syscall