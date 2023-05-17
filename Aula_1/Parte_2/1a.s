  	.equ getChar, 2
	.equ putChar, 3
	.data
	.text
	.globl main
main:

do:

	li	$v0, getChar
	syscall
	move	$t0, $v0		# c = getChar()
	addi	$t0, $t0, 1
	move	$a0, $t0
	li	$v0, putChar
	syscall				# putChar(c+1)
	beq	$t0, '\n', enddo	# while ( c != '\n' )
	j	do
	
enddo:
	li	$v0, 0			# return 0
	jr	$ra			# terminar o programa
