.data
args: .space 128 # Allocate 128 bytes for arguemnts
func_def: .space 1024 # Allocate 1024 bytes for functions
exit_message: .asciiz "Program exited with code: "
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
add $8, $0, 267
sw $8, 0($28)
add $8, $0, 4
sw $8, 4($28)
add $8, $0, 8
sw $8, 8($28)
add $8, $0, 12
sw $8, 12($28)
li $v0, 9
li $a0, 1024
syscall
add $16, $0, $v0
jal main0
la $a0, exit_message
li $v0, 4
syscall
add $4, $2, $0
li $v0, 1
syscall
li $v0, 10
syscall
j _L1
add1:
add $16, $16, -8
la $8, args
lw $9, 0($8)
sw $9, 0($16)
lw $8, 0($16)
add $9, $0, 1
add $10, $8, $9
sw $10, 4($16)
lw $2, 4($16)
add $16, $16, 8
add $16, $17, $0
jr $ra 
_L1:
j _L3
add2:
add $16, $16, -16
la $8, args
lw $9, 0($8)
sw $9, 0($16)
sw $31, 8($16)
sw $17, 12($16)
add $16, $16, -12
sw $31, 4($16)
sw $17, 8($16)
add $16, $16, 0
la $8, args
lw $9, 12($16)
sw $9, 0($8)
lw $8, 4($28)
la $9, func_def
add $9, $9, $8
lw $9, 0($9)
add $17, $16, $0
add $2, $0, 9
add $4, $0, 1024
syscall 
add $16, $2, $0
jal $9
sw $2, 0($16)
add $16, $16, 0
lw $31, 4($16)
lw $17, 8($16)
la $8, args
lw $9, 0($16)
sw $9, 0($8)
lw $8, 4($28)
la $9, func_def
add $9, $9, $8
lw $9, 0($9)
add $17, $16, $0
add $2, $0, 9
add $4, $0, 1024
syscall 
add $16, $2, $0
jal $9
sw $2, 16($16)
add $16, $16, 12
lw $31, 8($16)
lw $17, 12($16)
lw $2, 4($16)
add $16, $16, 16
add $16, $17, $0
jr $ra 
_L3:
j _L5
main0:
add $16, $16, -28
add $8, $0, 1
sw $8, 4($16)
lw $8, 4($16)
add $9, $0, 1
seq $10, $8, $9
sw $10, 8($16)
lw $8, 8($16)
beqz $8, _L7
add $16, $16, 0
lw $8, 8($28)
sw $8, 0($16)
add $16, $16, 0
j _L6
_L7:
add $16, $16, 0
lw $8, 4($28)
sw $8, 0($16)
add $16, $16, 0
_L6:
sw $31, 20($16)
sw $17, 24($16)
add $16, $16, 0
la $8, args
lw $9, 4($16)
sw $9, 0($8)
lw $8, 0($16)
la $9, func_def
add $9, $9, $8
lw $9, 0($9)
add $17, $16, $0
add $2, $0, 9
add $4, $0, 1024
syscall 
add $16, $2, $0
jal $9
sw $2, 16($16)
add $16, $16, 0
lw $31, 20($16)
lw $17, 24($16)
lw $8, 16($16)
lw $9, 0($28)
add $10, $8, $9
sw $10, 12($16)
lw $2, 12($16)
add $16, $16, 28
add $16, $17, $0
jr $ra 
_L5:
