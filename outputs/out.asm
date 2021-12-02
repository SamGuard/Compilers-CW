.text
.globl	main
main:
addi $sp, $sp, -4
_L0:
addi $sp, $sp, -8
   add $8, $0, 100000
   sw $8, 0($29)
   add $8, $0, 1
   sw $8, 4($29)
   addi $sp, $sp, -8
      _L1:
      lw $9, 8($29)
      add $8, $0, 1
      slt $10, $8, $9
      sw $10, 0($29)
      lw $8, 0($29)
      beqz $8, _L2
      lw $8, 8($29)
      lw $9, 12($29)
      sub $10, $8, $9
      sw $10, 4($29)
      lw $8, 4($29)
      sw $8, 8($29)
      j _L1
      _L2:
      addi $sp, $sp, 8
   addi $sp, $sp, 8
addi $sp, $sp, 1
li $v0, 10
syscall
