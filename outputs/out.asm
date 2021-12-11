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
recurse:
addi $sp, $sp, -12
   la $8, args
   lw $9, 0($8)
   sw $9, 0($29)
   add $8, $0, 0
   sw $8, 4($29)
   lw $9, 0($29)
   add $8, $0, 1
   slt $10, $8, $9
   sw $10, 8($29)
   lw $8, 8($29)
   beqz $8, _L1
   addi $sp, $sp, -12
      sw $31, 8($29)
      addi $sp, $sp, 0
         jal recurse
         _L1:
         sw $2, 4($29)
         addi $sp, $sp, 0
      lw $31, 8($29)
      lw $8, 4($29)
      add $10, $8, 1
      sw $10, 0($29)
      lw $8, 0($29)
      sw $8, 16($29)
      addi $sp, $sp, 12
   lw $2, 4($29)
   addi $sp, $sp, 12
jr $ra 
main0:
addi $sp, $sp, -16
   add $8, $0, 5
   sw $8, 0($29)
   sw $31, 12($29)
   addi $sp, $sp, 0
      la $8, args
      lw $9, 0($29)
      sw $9, 0($8)
      jal recurse
      sw $2, 8($29)
      addi $sp, $sp, 0
   lw $31, 12($29)
   lw $8, 8($29)
   sw $8, 4($29)
   lw $2, 4($29)
   addi $sp, $sp, 16
jr $ra 
