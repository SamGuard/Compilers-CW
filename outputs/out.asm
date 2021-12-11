.data
args: .space 128 # Allocate 128 bytes for arguemnts
.text
.globl	main
main:
jal main0
add $4, $2, $0
li $v0, 1
syscall
li $v0, 10
syscall
get1:
addi $sp, $sp, -12
   la $8, args
   lw $9, 0($8)
   sw $9, 0($29)
   add $8, $0, 1
   sw $8, 4($29)
   lw $8, 0($29)
   lw $9, 4($29)
   add $10, $8, $9
   sw $10, 8($29)
   lw $2, 8($29)
   addi $sp, $sp, 12
jr $ra 
main0:
addi $sp, $sp, -20
   add $8, $0, 2
   sw $8, 0($29)
   sw $31, 12($29)
   addi $sp, $sp, 0
      la $8, args
      lw $9, 0($29)
      sw $9, 0($8)
      jal get1
      sw $2, 8($29)
      addi $sp, $sp, 0
   lw $31, 12($29)
   lw $8, 8($29)
   sw $8, 4($29)
   lw $8, 0($29)
   lw $9, 4($29)
   add $10, $8, $9
   sw $10, 16($29)
   lw $8, 16($29)
   sw $8, 0($29)
   lw $2, 0($29)
   addi $sp, $sp, 20
jr $ra 
