.text
.globl	main
main:
addi $sp, $sp, -16
add $8, $0, 0
sw $8, 0($29)
add $8, $0, 100
sw $8, 4($29)
lw $8, 0($29)
lw $9, 4($29)
sub $10, $8, $9
sw $10, 12($29)
_L:
addi $sp, $sp, 16
li $v0, 10
syscall
