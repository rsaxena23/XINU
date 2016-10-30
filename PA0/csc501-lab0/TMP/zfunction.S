.section .data

.section .text

.globl zfunction
zfunction:
    pushl %ebp
	movl  %esp, %ebp
	movl  8(%ebp), %eax
	andl  $0xfffff00f, %eax
	shl   $8, %eax
	leave
	ret

