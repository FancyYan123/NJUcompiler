.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
    li $v0, 4
    la $a0, _prompt
    syscall
    li $v0, 5
    syscall
    jr $ra
write:
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, _ret
    syscall
    move $v0, $0
    jr $ra
func:
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -400
lw $t8, 8($fp)
sw $t8, -4($fp)
lw $t8, 12($fp)
sw $t8, -8($fp)
lw $t8, 16($fp)
sw $t8, -12($fp)
lw $t8, 20($fp)
sw $t8, -16($fp)
lw $t8, 24($fp)
sw $t8, -20($fp)
lw $t1, -4($fp)
move $t0, $t1
sw $t0, -24($fp)
lw $t1, -8($fp)
move $t0, $t1
sw $t0, -28($fp)
lw $t1, -24($fp)
lw $t2, -28($fp)
add $t0, $t1, $t2
sw $t0, -32($fp)
lw $t1, -12($fp)
move $t0, $t1
sw $t0, -36($fp)
lw $t1, -32($fp)
lw $t2, -36($fp)
add $t0, $t1, $t2
sw $t0, -40($fp)
lw $t1, -16($fp)
move $t0, $t1
sw $t0, -44($fp)
lw $t1, -40($fp)
lw $t2, -44($fp)
add $t0, $t1, $t2
sw $t0, -48($fp)
lw $t1, -20($fp)
move $t0, $t1
sw $t0, -52($fp)
lw $t1, -48($fp)
lw $t2, -52($fp)
add $t0, $t1, $t2
sw $t0, -56($fp)
lw $t1, -56($fp)
move $t0, $t1
sw $t0, -60($fp)
lw $t1, -60($fp)
move $t0, $t1
sw $t0, -64($fp)
lw $t0, -64($fp)
move $v0, $t0
addi $sp, $sp, 400
lw $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
main:
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -400
li $t0, 1
sw $t0, -4($fp)
li $t0, 2
sw $t0, -8($fp)
li $t0, 3
sw $t0, -12($fp)
li $t0, 4
sw $t0, -16($fp)
li $t0, 5
sw $t0, -20($fp)
lw $t8, -20($fp)
sw $t8, 0($sp)
lw $t8, -16($fp)
sw $t8, 4($sp)
lw $t8, -12($fp)
sw $t8, 8($sp)
lw $t8, -8($fp)
sw $t8, 12($sp)
lw $t8, -4($fp)
sw $t8, 16($sp)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal func
move $t0, $v0
sw $t0, -44($fp)
lw $ra, 0($sp)
addi $sp, $sp, 4
lw $t1, -44($fp)
move $t0, $t1
sw $t0, -48($fp)
lw $t1, -48($fp)
move $t0, $t1
sw $t0, -52($fp)
lw $t0, -52($fp)
move $a0, $t0
sw $t0, -52($fp)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
li $t0, 0
sw $t0, -56($fp)
lw $t0, -56($fp)
move $v0, $t0
addi $sp, $sp, 400
lw $fp, 0($sp)
addi $sp, $sp, 4
jr $ra

