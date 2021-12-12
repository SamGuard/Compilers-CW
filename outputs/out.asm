.data
args: .space 128 # Allocate 128 bytes for arguemnts
func_def: .space 1024 # Allocate 1024 bytes for functions
.text
.globl	main
main:
la $t0, factorial
sw $t0, func_def
la $t0, main
sw $t0, func_def
jal main0
add $4, $2, $0
li $v0, 1
syscall
li $v0, 10
syscall
factorial:
addi $sp, $sp, -8
la $8, args
lw $9, 0($8)
sw $9, 0($29)
lw $8, 0($29)
add $9, $0, 0
seq $10, $8, $9
sw $10, 4($29)
lw $8, 4($29)
beqz $8, _L2
addi $sp, $sp, 0
add $2, $0, 1
add $29, $29, 0
add $29, $29, 8
add $29, $29, 0
jr $ra 
addi $sp, $sp, 0
j _L1
_L2:
addi $sp, $sp, -12
sw $31, 8($29)
addi $sp, $sp, -4
lw $8, 16($29)
add $9, $0, 1
sub $10, $8, $9
sw $10, 0($29)
la $8, args
lw $9, 0($29)
sw $9, 0($8)
jal factorial
sw $2, 8($29)
addi $sp, $sp, 4
lw $31, 8($29)
lw $8, 12($29)
lw $9, 4($29)
mult $8, $9
mflo $10
sw $10, 0($29)
lw $2, 0($29)
add $29, $29, 12
add $29, $29, 8
add $29, $29, 0
jr $ra 
addi $sp, $sp, 12
_L1:
main0:
addi $sp, $sp, -8
sw $31, 4($29)
addi $sp, $sp, 0
la $8, args
add $9, $0, 10
sw $9, 0($8)
jal factorial
sw $2, 0($29)
addi $sp, $sp, 0
lw $31, 4($29)
lw $2, 0($29)
add $29, $29, 8
add $29, $29, 0
add $29, $29, 4
jr $ra 
