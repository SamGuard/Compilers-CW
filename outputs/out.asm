.data
args: .space 128 # Allocate 128 bytes for arguemnts
func_def: .space 1024 # Allocate 1024 bytes for functions
.text
.globl	main
main:
la $t0, func_def
la $t1, main0
sw $t1, 0($t0)
la $t1, add1
sw $t1, 4($t0)
la $t1, add2
sw $t1, 8($t0)
add $sp, $sp, -12
add $8, $0, 4
sw $8, 0($29)
add $8, $0, 8
sw $8, 4($29)
add $8, $0, 12
sw $8, 8($29)
jal main0
add $sp, $sp, 12
add $4, $2, $0
li $v0, 1
syscall
li $v0, 10
syscall
j _L1
add1:
add $29, $29, -8
la $8, args
lw $9, 0($8)
sw $9, 0($29)
lw $8, 0($29)
add $9, $0, 1
add $10, $8, $9
sw $10, 4($29)
lw $2, 4($29)
add $29, $29, 8
jr $ra 
_L1:
j _L3
add2:
add $29, $29, -8
la $8, args
lw $9, 0($8)
sw $9, 0($29)
lw $8, 0($29)
add $9, $0, 2
add $10, $8, $9
sw $10, 4($29)
lw $2, 4($29)
add $29, $29, 8
jr $ra 
_L3:
j _L5
main0:
add $29, $29, -20
add $8, $0, 1
sw $8, 4($29)
lw $8, 4($29)
add $9, $0, 1
seq $10, $8, $9
sw $10, 8($29)
lw $8, 8($29)
beqz $8, _L7
add $29, $29, 0
lw $8, 24($29)
sw $8, 0($29)
add $29, $29, 0
j _L6
_L7:
add $29, $29, 0
lw $8, 20($29)
sw $8, 0($29)
add $29, $29, 0
_L6:
sw $31, 16($29)
add $29, $29, 0
la $8, args
lw $9, 4($29)
sw $9, 0($8)
lw $8, 0($29)
la $9, func_def
add $9, $9, $8
lw $9, 0($9)
jal $9
sw $2, 12($29)
add $29, $29, 0
lw $31, 16($29)
lw $2, 12($29)
add $29, $29, 20
jr $ra 
_L5:
