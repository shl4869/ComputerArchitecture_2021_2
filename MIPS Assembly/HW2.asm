# 2071035 Lee Somin ComputerArchitecture HW2 part II
# HW2.asm ? program printing sum of 1 to n using iteration and recursion
# Registers used:
# $v0 - syscall parameter, input from consol and return value.
# $a0 - syscall parameter --  the string to print / input to the function
# $s0 - the number n that is wanted to get sum from 1 to n
# $s1, $s2 - the result of summation from iterative function and recursive function

	.text 		# instructions follow this line
main:			# start
# Print the initial string
	la $a0, init_print	# load the address of init_print into $a0
	li $v0, 4	# 4 is the print_string syscall
	syscall		# do the syscall
	
	li $v0, 5	# 5 is for getting a integer from consol
	syscall		# do the syscall
	move $s0, $v0	# move the number read into $s0
	move $a0, $s0	# move the number in $s0 to $a0

# Call iterative function
	jal iterative	# call subroutine iterative
	nop		# branch delay slot
	move $s1, $v0	# move the result to $s1
	
# Call recursive function
	jal recursive	# call subroutine recursive
	nop		# branch delay slot
	move $s2, $v0	#move the result to $s2
	
# Print the result of iteration	
	la $a0, print_iter	# load the address of print_iter into $a0
	li $v0, 4	# 4 is the print_string syscall
	syscall		# do the syscall
	add $a0,$s0,$zero	# load $s0 to $a0
	li $v0, 1	# 1 is the print_int syscall
	syscall		# do the syscall
	la $a0,equal	# load the address of equal into $a0
	li $v0, 4	# 4 is the print_string syscall
	syscall		# do the syscall
	add $a0,$s1,$zero	# load $s1 to $a0
	li $v0, 1	# 1 is the print_int syscall
	syscall		# do the syscall
	
# Print the result of recursion
	la $a0, print_recur	# load the address of print_recur into $a0
	li $v0, 4	# 4 is the print_string syscall
	syscall		# do the syscall
	add $a0,$s0,$zero	# load $s0 to $a0
	li $v0, 1	# 1 is the print_int syscall
	syscall		# do the syscall
	la $a0,equal	# load the address of equal into $a0
	li $v0, 4	# 4 is the print_string syscall
	syscall		# do the syscall
	add $a0,$s2,$zero	# load $s2 to $a0
	li $v0, 1	# 1 is the print_int syscall
	syscall		# do the syscall

# Print the last line	
	la $a0, done	# load the address of done into $a0
	li $v0, 4	# $4 is the print_string syscall
	syscall		# do the syscall
# Exit the program
	li $v0, 10	# 10 is the exit syscall
	syscall		# do the syscall

# Iterative Function	
iterative:
	addi $sp,$sp,-8	# adjust stack for 3 items
	sw $s0,4($sp)	# save $s0 in stack
	sw $s1,0($sp)	# save $s1 in stack
	add $s0,$a0,$zero	# load $a0 to $s0
	add $s1, $zero, $zero	# i = 0
	add $v0, $zero, $zero	# initialize the result as 0
itL1: 
	addi $s1,$s1,1	# i = i+1
	add $v0,$v0,$s1	# add the number to result
	beq $s1,$s0,itL2	# exit loop if i == $s0
	j itL1		# next iteration of loop
itL2:
	lw $s1, 0($sp)	# restore saved $s1
	lw $s0, 4($sp)	# restore saved $s0
	addi $sp, $sp, 8	# pop 2 items from stack
	jr $ra		# return to return address

# Recursive Function	
recursive:
	addi $sp, $sp, -8	# adjust stack for 2 items
	sw $ra, 4($sp)	# save the return address
	sw $a0, 0($sp)	# save argument
	slti $t0,$a0,1	# test for n < 1
	beq $t0,$zero,reL1	# if not, go to reL1
	add $v0,$zero,$zero	# if so, result is 0
	addi $sp, $sp,8	# pop 2 items from stack
	jr $ra		# return to return address
reL1:
	addi $a0,$a0,-1	# n-1
	jal recursive	#call the recursive function
	nop		# branch delay slot
	lw $a0,0($sp)	#restore original n
	lw $ra, 4($sp)	#restore return address
	addi $sp, $sp,8	# pop 2 items from stack
	add $v0,$a0,$v0	# add to get result
	jr $ra		# return to return address
	
# Data for the program
	.data		# Variable declarations follow this line
init_print:		# data for initial print(student information, input instruction)
	.ascii "[HW #2 Part II] StudentID 2071035: Somin Lee\n\n"
	.asciiz "Enter a positive integer: "
print_iter:		# data for printing iteration result
	.asciiz "\n(Iterative) Sum from 1 to "
print_recur:		# data for printing recursive result
	.asciiz "\n(Rucursive) Sum from 1 to "
equal:			# data for printing character '='
	.asciiz " = "
done: 			# data for printing at the end of the program
	.asciiz "\n\nDone..."
# end HW2.asm