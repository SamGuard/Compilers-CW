.data
args .space 128 # Allocate 128 bytes for arguemnts
.text
.globl	premain
premain:
jal main
li $v0, 10
syscall
get1:
addi $sp, $sp, -4
   add $8, $0, 1
   sw $8, 0($29)
   addi $sp, $sp, 4
jr $ra 
main:
addi $sp, $sp, -16
   add $8, $0, 2
   sw $8, 0($29)
   sw $31, 12($29)
   addi $sp, $sp, 0
      jal get1
      sw $2, 8($29)
      addi $sp, $sp, 0
   lw $31, 12($29)
   lw $8, 8($29)
   sw $8, 4($29)
   addi $sp, $sp, 16
jr $ra 
