# 2071035 Lee Somin ComputerArchitecture HW1 part II
# HW1.asm -- program printing out student ID, name, and sentence. 
#Registers used:
# $v0 - syscall parameter and return value.
# $a0 - syscall parameter -- the string to print.
	.text			# instructions follow this line
main: 				# start execution at main
	la $a0, student_info	# lead the address of student_info into #$a0
	li $v0, 4		# 4 is the print_string syscall
	syscall			# do the syscall
# Exit the program
	li $v0, 10		# 10 is the exit syscall
	syscall			# do the syscall

# Data for the program:
	.data			# Variable declarations follow this line
student_info:	.ascii "Student ID: "	# The string "Student ID: "
		.ascii "2071035 "	# The numbers "2071035 "
		.byte 0x53		# hex for "S"
		.byte 0x6F		# hex for "o"
		.byte 0x6D		# hex for "m"
		.byte 0x69		# hex for "i"
		.byte 0x6E		# hex for "n"
		.ascii " Lee"		# The string "Lee"
		.ascii "\n"		# A new line 
		.asciiz "I want to learn more about computer hardware and lower level language."	# String of sentence. asciiz null-terminates by itself. 
#end HW1.asm
