	.equ READ_CORE_TIMER, 11
	.equ RESET_CORE_TIMER, 12 
	.equ PUT_CHAR, 3
	.equ PRINT_INT, 6
	.data
	.text
	.globl main
main: 	li 	$t0, 0 			# counter=0
while: 					# while (1)
	
	li	$a0, 100
	jal	delay			# delay(100) // wait 100 ns
	
	move	$t1, $v0
	addi	$t0, $t0, 1		# counter++
	move	$a0, $t0
	li	$a1, 0x0004000A
	li	$v0, PRINT_INT		# printInt(counter++, 10 | 4 << 16);	
	syscall
	li	$a0, '\r'
	li	$v0, PUT_CHAR
	syscall				# putChar('\r')
	
	j	while
endw1:
	li	$v0, 0
	jr 	$ra 			#


delay:
	mul	$t0, $a0, 200000
	li	$v0, RESET_CORE_TIMER
	syscall

delay_w:	
	li	$v0, READ_CORE_TIMER
	syscall
	blt	$v0, $t0, delay_w
	jr	$ra
		
