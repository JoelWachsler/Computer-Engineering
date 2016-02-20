  # labwork.S
  # Written 2015 by F Lundevall
  # Edited 2016-01-22 by Joel Wachsler
  # Skeleton file for IS1200/IS1500 lab 1 2015-2016.
  # The macros PUSH and POP are in the public domain.

.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	    \reg,0($sp)
.endm

.macro	POP reg
	lw	    \reg,0($sp)
	addi	$sp,$sp,4
.endm

.macro  return
	jr $ra
	nop		# delay slot filler
.endm

.macro  loadConvReplace val
	la 		$a0, (\val)	# Pass the value as reference
	jal hexasc			# Call the function
	nop
    sb 		$v0, ($t0)	# Save char
    addi 	$t0, $t0, 1	# Go one byte forward
.endm

.macro insertChar char
    li		$t5, \char      # Save char in register
	sb 		$t5, ($t0)		# Insert char
	addi 	$t0, $t0, 1		# Go one byte forward
.endm

hexasc:
	andi 	$a0, $a0, 0xf	# Get the 4 least significant bits
	bgt		$a0, 9, greater
	nop

	# We didn't branch which means this item is lesser than 10
	# add 0x30 to make it an ascii char which represents the
	# same number
	addi 	$v0, $a0, 0x30
	return

	# The number's greater than 0x9 which means we have to
	# convers it to a character which is 0x41 numbers up subtracted
	# by 0xa which is 0x37
	greater:
		addi	$v0, $a0, 0x37
		return

# Creates a delay for the "clock"
# $a0 is the delay in ms
.global delay
delay:
	while:
		bge 	$zero, $a0, while_end	# Branch if 0 >= ms
		nop
		addi	$a0, $a0, -1			# decrement ms

		# The following foor loop should take 1 ms
		li $t0, 0	# $t0 = i
		for:
			bge	$t0, 11320, for_end		# Branch if i >= 1400
			nop
			addi $t0, $t0, 1			# increment i

			j for
		for_end:

		j while
	while_end:

	return

# Converts $a1 to a "clock" string at $a0
.global time2string
time2string:
	# $a0 is the destination, save it
	la $t0, ($a0)
	# the 16 lsb in $a1 is time-info organized as four NBCD-coded
	# digits of 4 bits each
	andi 	$t1, $a1, 0xf000	# Get the first minute
	srl		$t1, $t1, 0xc		# Shift 12 bits to the right
	andi 	$t2, $a1, 0x0f00	# Get the second minute
	srl		$t2, $t2, 0x8		# Shift 8 bits to the right
	andi 	$t3, $a1, 0x00f0	# Get the first second
	srl		$t3, $t3, 0x4		# Shift 4 bits to the right
	andi 	$t4, $a1, 0x000f	# Get the second second

	PUSH $ra	# Save $ra to the stack so this function can return later

	# Get the ascii represenation
	# Add minutes
	loadConvReplace $t1
	loadConvReplace $t2

	# Add colon
    insertChar 0x3a
	# Add seconds
	loadConvReplace $t3
	loadConvReplace $t4

	# Add null terminator
    insertChar 0x00

	POP $ra	# Return $ra to its original value
	return

.global enable_interrupt
enable_interrupt:
	ei

	return

