.data
args: .space 128 # Allocate 128 bytes for arguemnts
func_def: .space 1024 # Allocate 1024 bytes for functions
closures: .space 1024 # Space to put closures
exit_message: .asciiz "Program exited with code: "
.text
.globl	main
main:
la $t0, func_def
la $t1, main0
sw $t1, 0($t0)
la $8, closures # Load closure space address
add $8, $8, $18
add $9, $0, 0 # Put offset into register
sw $9, 0($8) # Set offset for closure
sw $16, 16($8) # Set stack pointer for closure
sw $18, 0($28) # Set func main definition
add $18, 8 # Increment closure counter
li $v0, 9
li $a0, 1024
syscall
add $16, $0, $v0
jal main0
add $s0, $2, $0 # Save exit value
la $a0, exit_message # Print exit message
li $v0, 4
syscall
add $4, $s0, $0 # Print exit value
li $v0, 1
syscall
li $v0, 10
syscall
j _L1 # Skip function as this is the definiton
main0:
add $16, $16, -96 # Create new scope
add $8, $0, 1
sw $8, 0($16) # Set a
add $8, $0, 2
sw $8, 4($16) # Set b
add $8, $0, 3
sw $8, 8($16) # Set c
add $8, $0, 25
sw $8, 12($16) # Set d
add $8, $0, 1
add $9, $0, 1
sne $10, $8, $9 # Maths and Logic
sw $10, 16($16) # Maths and Logic
lw $8, 16($16) # getArg: _T, memory offset: 4
beqz $8, _L2 # Branch if 0
add $16, $16, 0 # Move scope down
add $2, $0, 1
add $16, $16, 0 # Return scope to original position
add $16, $16, 20 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
add $16, $16, 0 # Move scope up
_L2:
lw $8, 0($16) # getArg: a, memory offset: 0
lw $9, 4($16) # getArg: b, memory offset: 1
add $10, $8, $9 # Maths and Logic
sw $10, 24($16) # Maths and Logic
lw $8, 24($16) # getArg: _T, memory offset: 6
add $9, $0, 3
sne $10, $8, $9 # Maths and Logic
sw $10, 20($16) # Maths and Logic
lw $8, 20($16) # getArg: _T, memory offset: 5
beqz $8, _L3 # Branch if 0
add $16, $16, 0 # Move scope down
add $2, $0, 2
add $16, $16, 0 # Return scope to original position
add $16, $16, 28 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
add $16, $16, 0 # Move scope up
_L3:
lw $8, 0($16) # getArg: a, memory offset: 0
lw $9, 4($16) # getArg: b, memory offset: 1
sub $10, $8, $9 # Maths and Logic
sw $10, 32($16) # Maths and Logic
add $8, $0, 0
add $9, $0, 1
sub $10, $8, $9 # Maths and Logic
sw $10, 36($16) # Maths and Logic
lw $8, 32($16) # getArg: _T, memory offset: 8
lw $9, 36($16) # getArg: _T, memory offset: 9
sne $10, $8, $9 # Maths and Logic
sw $10, 28($16) # Maths and Logic
lw $8, 28($16) # getArg: _T, memory offset: 7
beqz $8, _L4 # Branch if 0
add $16, $16, 0 # Move scope down
add $2, $0, 3
add $16, $16, 0 # Return scope to original position
add $16, $16, 40 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
add $16, $16, 0 # Move scope up
_L4:
lw $8, 4($16) # getArg: b, memory offset: 1
lw $9, 8($16) # getArg: c, memory offset: 2
mult $8, $9 # Multiplication/Division
mflo $10 # Multiplication/Division
sw $10, 44($16) # Multiplication/Division
lw $8, 44($16) # getArg: _T, memory offset: 11
add $9, $0, 6
sne $10, $8, $9 # Maths and Logic
sw $10, 40($16) # Maths and Logic
lw $8, 40($16) # getArg: _T, memory offset: 10
beqz $8, _L5 # Branch if 0
add $16, $16, 0 # Move scope down
add $2, $0, 4
add $16, $16, 0 # Return scope to original position
add $16, $16, 48 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
add $16, $16, 0 # Move scope up
_L5:
lw $8, 8($16) # getArg: c, memory offset: 2
lw $9, 4($16) # getArg: b, memory offset: 1
div $8, $9 # Multiplication/Division
mflo $10 # Multiplication/Division
sw $10, 52($16) # Multiplication/Division
lw $8, 52($16) # getArg: _T, memory offset: 13
add $9, $0, 1
sne $10, $8, $9 # Maths and Logic
sw $10, 48($16) # Maths and Logic
lw $8, 48($16) # getArg: _T, memory offset: 12
beqz $8, _L6 # Branch if 0
add $16, $16, 0 # Move scope down
add $2, $0, 5
add $16, $16, 0 # Return scope to original position
add $16, $16, 56 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
add $16, $16, 0 # Move scope up
_L6:
lw $8, 12($16) # getArg: d, memory offset: 3
lw $9, 8($16) # getArg: c, memory offset: 2
div $8, $9 # Modulo
mfhi $10 # Modulo
sw $10, 60($16) # Modulo
lw $8, 60($16) # getArg: _T, memory offset: 15
add $9, $0, 1
sne $10, $8, $9 # Maths and Logic
sw $10, 56($16) # Maths and Logic
lw $8, 56($16) # getArg: _T, memory offset: 14
beqz $8, _L7 # Branch if 0
add $16, $16, 0 # Move scope down
add $2, $0, 6
add $16, $16, 0 # Return scope to original position
add $16, $16, 64 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
add $16, $16, 0 # Move scope up
_L7:
lw $8, 4($16) # getArg: b, memory offset: 1
lw $9, 8($16) # getArg: c, memory offset: 2
sle $10, $8, $9 # Maths and Logic
sw $10, 68($16) # Maths and Logic
lw $8, 68($16) # getArg: _T, memory offset: 17
add $9, $0, 1
sne $10, $8, $9 # Maths and Logic
sw $10, 64($16) # Maths and Logic
lw $8, 64($16) # getArg: _T, memory offset: 16
beqz $8, _L8 # Branch if 0
add $16, $16, 0 # Move scope down
add $2, $0, 7
add $16, $16, 0 # Return scope to original position
add $16, $16, 72 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
add $16, $16, 0 # Move scope up
_L8:
lw $8, 4($16) # getArg: b, memory offset: 1
lw $9, 8($16) # getArg: c, memory offset: 2
slt $10, $8, $9 # Maths and Logic
sw $10, 76($16) # Maths and Logic
lw $8, 76($16) # getArg: _T, memory offset: 19
add $9, $0, 1
sne $10, $8, $9 # Maths and Logic
sw $10, 72($16) # Maths and Logic
lw $8, 72($16) # getArg: _T, memory offset: 18
beqz $8, _L9 # Branch if 0
add $16, $16, 0 # Move scope down
add $2, $0, 8
add $16, $16, 0 # Return scope to original position
add $16, $16, 80 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
add $16, $16, 0 # Move scope up
_L9:
lw $8, 4($16) # getArg: b, memory offset: 1
add $9, $0, 2
sle $10, $8, $9 # Maths and Logic
sw $10, 84($16) # Maths and Logic
lw $8, 84($16) # getArg: _T, memory offset: 21
add $9, $0, 1
sne $10, $8, $9 # Maths and Logic
sw $10, 80($16) # Maths and Logic
lw $8, 80($16) # getArg: _T, memory offset: 20
beqz $8, _L10 # Branch if 0
add $16, $16, 0 # Move scope down
add $2, $0, 9
add $16, $16, 0 # Return scope to original position
add $16, $16, 88 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
add $16, $16, 0 # Move scope up
_L10:
lw $8, 4($16) # getArg: b, memory offset: 1
add $9, $0, 3
sge $10, $8, $9 # Maths and Logic
sw $10, 92($16) # Maths and Logic
lw $8, 92($16) # getArg: _T, memory offset: 23
add $9, $0, 0
sne $10, $8, $9 # Maths and Logic
sw $10, 88($16) # Maths and Logic
lw $8, 88($16) # getArg: _T, memory offset: 22
beqz $8, _L11 # Branch if 0
add $16, $16, 0 # Move scope down
add $2, $0, 10
add $16, $16, 0 # Return scope to original position
add $16, $16, 96 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
add $16, $16, 0 # Move scope up
_L11:
add $2, $0, 0
add $16, $16, 96 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
_L1:
