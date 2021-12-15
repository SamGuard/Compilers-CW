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
la $t1, getFuncFunc
sw $t1, 4($t0)
la $t1, getFunc
sw $t1, 8($t0)
la $t1, f
sw $t1, 12($t0)
la $8, closures # Load closure space address
add $8, $8, $18
add $9, $0, 4 # Put offset into register
sw $9, 0($8) # Set offset for closure
sw $16, 16($8) # Set stack pointer for closure
sw $18, 0($28) # Set func getFuncFunc definition
add $18, 8 # Increment closure counter
la $8, closures # Load closure space address
add $8, $8, $18
add $9, $0, 0 # Put offset into register
sw $9, 0($8) # Set offset for closure
sw $16, 16($8) # Set stack pointer for closure
sw $18, 4($28) # Set func main definition
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
getFuncFunc:
add $16, $16, -20 # Create new scope
add $8, $0, 4
sw $8, 0($16) # Set a
la $8, closures # Load closure space address
add $8, $8, $18
add $9, $0, 8 # Put offset into register
sw $9, 0($8) # Set offset for closure
sw $16, 16($8) # Set stack pointer for closure
sw $18, 4($16) # Set func getFunc definition
add $18, 8 # Increment closure counter
j _L3 # Skip function as this is the definiton
getFunc:
add $16, $16, -12 # Move scope down
sw $30, 0($16) # Save previous chain location into memory
add $8, $0, 2
sw $8, 4($16) # Set b
la $8, closures # Load closure space address
add $8, $8, $18
add $9, $0, 12 # Put offset into register
sw $9, 0($8) # Set offset for closure
sw $16, 16($8) # Set stack pointer for closure
sw $18, 8($16) # Set func f definition
add $18, 8 # Increment closure counter
j _L5 # Skip function as this is the definiton
f:
add $16, $16, -8 # Move scope down
sw $30, 0($16) # Save previous chain location into memory
add $19, $30, $0# Move previous chain location into FPS register
lw $19, 0($19)# Getting Variable from different chain
lw $8, 0($19) # getArg: a, memory offset: 0
lw $9, 4($30) # getArg: b, memory offset: 1
add $10, $8, $9 # Maths and Logic
sw $10, 4($16) # Maths and Logic
lw $2, 4($16) # getArg: _T, memory offset: 1
add $16, $16, 8 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
_L5:
lw $2, 8($16) # getArg: f, memory offset: 2
add $16, $16, 12 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
_L3:
sw $31, 12($16) # Save return address
sw $17, 16($16) # Save memory pointer
add $16, $16, 0 # Move scope down
lw $8, 4($16) # getArg: getFunc, memory offset: 1
la $9, closures # Load closure space address
add $9, $9, $8 # Move pointer to closure index
lw $10, 0($9) # Load offset into register
lw $30, 16($9) # Set frame pointer
la $8, func_def # Load func_def address
add $8, $8, $10 # Add offset to it
lw $8, 0($8) # Load function definition
add $17, $16, $0 # Copy stack pointer to save it
add $2, $0, 9 # sbreak
add $4, $0, 1024 # Amount to allocate
syscall 
add $16, $2, $0 # Move stack pointer to new address
jal $8
sw $2, 8($16) # Load return value
add $16, $16, 0 # Move scope up
lw $31, 12($16) # getArg: RA, memory offset: 3
lw $17, 16($16) # getArg: MP, memory offset: 4
lw $2, 8($16) # getArg: _T, memory offset: 2
add $16, $16, 20 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
_L1:
j _L7 # Skip function as this is the definiton
main0:
add $16, $16, -28 # Create new scope
sw $31, 8($16) # Save return address
sw $17, 12($16) # Save memory pointer
add $16, $16, 0 # Move scope down
lw $8, 0($28) # getArg: getFuncFunc, memory offset: 0
la $9, closures # Load closure space address
add $9, $9, $8 # Move pointer to closure index
lw $10, 0($9) # Load offset into register
lw $30, 16($9) # Set frame pointer
la $8, func_def # Load func_def address
add $8, $8, $10 # Add offset to it
lw $8, 0($8) # Load function definition
add $17, $16, $0 # Copy stack pointer to save it
add $2, $0, 9 # sbreak
add $4, $0, 1024 # Amount to allocate
syscall 
add $16, $2, $0 # Move stack pointer to new address
jal $8
sw $2, 4($16) # Load return value
add $16, $16, 0 # Move scope up
lw $31, 8($16) # getArg: RA, memory offset: 2
lw $17, 12($16) # getArg: MP, memory offset: 3
lw $8, 4($16) # getArg: _T, memory offset: 1
sw $8, 0($16) # Set f
sw $31, 20($16) # Save return address
sw $17, 24($16) # Save memory pointer
add $16, $16, 0 # Move scope down
lw $8, 0($16) # getArg: f, memory offset: 0
la $9, closures # Load closure space address
add $9, $9, $8 # Move pointer to closure index
lw $10, 0($9) # Load offset into register
lw $30, 16($9) # Set frame pointer
la $8, func_def # Load func_def address
add $8, $8, $10 # Add offset to it
lw $8, 0($8) # Load function definition
add $17, $16, $0 # Copy stack pointer to save it
add $2, $0, 9 # sbreak
add $4, $0, 1024 # Amount to allocate
syscall 
add $16, $2, $0 # Move stack pointer to new address
jal $8
sw $2, 16($16) # Load return value
add $16, $16, 0 # Move scope up
lw $31, 20($16) # getArg: RA, memory offset: 5
lw $17, 24($16) # getArg: MP, memory offset: 6
lw $2, 16($16) # getArg: _T, memory offset: 4
add $16, $16, 28 # Return scope to original position
add $16, $17, $0 # Restore previous stack pointer
jr $ra  # Return
_L7:
