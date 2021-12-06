.text
.globl	premain
premain:
jal main
li $v0, 10
syscall
addi $sp, $sp, -8
get1:
addi $sp, $sp, -4
   add $8, $0, 1
   sw $8, 0($29)
   addi $sp, $sp, 4
jr $ra 
main:
addi $sp, $sp, -8
   add $8, $0, 2
   sw $8, 0($29)
   sw $31, 4($29)
   addi $sp, $sp, 0
      jal get1
      addi $sp, $sp, 0
   lw $31, 4($29)
   addi $sp, $sp, 8
jr $ra 
addi $sp, $sp, 2
