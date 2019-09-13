


	.text

	.globl _sb_ready, _sb_frag

begin_locked_space:	

_sb_ready:	.long 0
_sb_frag:	.long 0
base:	.long 0
irq:	.long 0
dma:	.long 0

_sb_int_handler:
	pushl %eax
	pushl %edx
	movb $1, _sb_ready
	xorb $1, _sb_frag
	movl base, %edx
	addl $0xE, %edx
	inb %dx, %al
	movb $0x20, %al
	testb $8, irq
	jz .Lsb_lowint
	outb %al, $0xa0
.Lsb_lowint:	
	outb %al, $0x20
	popl %edx
	popl %eax
	iret

end_locked_space:	


_sb_write:
	pushl %edx
	pushl %eax
	movl base, %edx
	addl $0xC, %edx
.Lsb_write_loop:	
	inb %dx, %al
	testb $0x80, %al
	jnz .Lsb_write_loop
	popl %eax
	outb %al, %dx
	popl %edx
	ret

_sb_read:
	pushl %edx
	movl base, %edx
	addl $0xE, %edx
.Lsb_read_loop:	
	inb %dx, %al
	testb $0x80, %al
	jz .Lsb_read_loop
	subl $4, %edx
	inb %dx, %al
	popl %edx
	ret

	.globl _sb_init
_sb_init:
	movl 8(%esp), %edx
	movl 12(%esp), %eax
	movl %edx, irq
	movl %eax, dma
	movl 4(%esp), %edx
	movl %edx, base
	addl $0x6, %edx
	movb $1, %al
	outb %al, %dx
	pushl $1
	call _delay
	popl %eax
	movb $0xE1, %al
	call _sb_write
	call _sb_read
	movb %al, %ah
	call _sb_read
	ret



	.globl _sb1_setrate
_sb1_setrate:
	
	

	.globl _sb_begin
_sb_begin:
	












