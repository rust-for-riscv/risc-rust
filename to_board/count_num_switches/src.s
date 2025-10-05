

.global _start
_start:

la t0, 0xFF200000
la t1, 0xFF200040
li t3, 0
	
loop:

	call count_bit
	sw t3,(t0)
	
j loop

count_bit:
	lw t2,(t1)
	li t3, 0
	loop_count:
		beqz t2, exit_count
		andi t4, t2, 1
		beqz t4, shift
		
		j add_one
		
		shift:srli t2, t2, 1
	j loop_count
	
	add_one:
		addi t3, t3, 1
	j shift
	
exit_count: ret
	