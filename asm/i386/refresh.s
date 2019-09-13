

#include "asmnames.h"

	.text

	.macro _enter	
	pushl %ebx
	pushl %ebp
	pushl %esi
	pushl %edi
	movl 20(%esp), %edi
	movl 24(%esp), %esi
	movl 28(%esp), %ebp
	movl 32(%esp), %ecx
	xorl %eax, %eax
	xorl %ebx, %ebx
	.endm

	.macro _leave
	popl %edi
	popl %esi
	popl %ebp
	popl %ebx
	ret
	.endm


	.globl refresh_1
refresh_1:
	_enter
	subl $4, %esi
	subl $4, %edi
	shrl $2, %ecx
.Lrefresh_1:
	movb 2(%esi,%ecx,4), %al
	movb 3(%esi,%ecx,4), %bl
	movb (%ebp, %eax), %dl
	movb (%esi,%ecx,4), %al
	movb (%ebp, %ebx), %dh
	movb 1(%esi,%ecx,4), %bl
	rorl $16, %edx
	movb (%ebp, %eax), %dl
	movb (%ebp, %ebx), %dh
	movl %edx, (%edi,%ecx,4)
	decl %ecx
	jnz .Lrefresh_1
	_leave

	.globl refresh_2
refresh_2:
	_enter
	subl $2, %esi
	subl $4, %edi
	shrl $1, %ecx
.Lrefresh_2:
	movb 1(%esi,%ecx,2), %al
	movb (%esi,%ecx,2), %bl
	movw (%ebp,%eax,2), %dx
	rorl $16, %edx
	movw (%ebp,%ebx,2), %dx
	movl %edx, (%edi,%ecx,4)
	decl %ecx
	jnz .Lrefresh_2
	_leave


	.globl refresh_4
refresh_4:
	_enter
	subl $2, %esi
	subl $8, %edi
	shrl $1, %ecx
.Lrefresh_4:
	movb (%esi,%ecx,2), %al
	movb 1(%esi,%ecx,2), %bl
	movl (%ebp,%eax,4), %edx
	movl %edx, (%edi,%ecx,8)
	movl (%ebp,%ebx,4), %edx
	movl %edx, 4(%edi,%ecx,8)
	decl %ecx
	jnz .Lrefresh_4
	_leave



	.globl refresh_1_2x
refresh_1_2x:
	_enter
	subl $2, %esi
	subl $4, %edi
	shrl $1, %ecx
.Lrefresh_1_2x:
	movb 1(%esi,%ecx,2), %al
	movb (%esi,%ecx,2), %bl
	movb (%ebp,%eax), %al
	movb %al, %dl
	movb %al, %dh
	movb (%ebp,%ebx), %bl
	rorl $16, %edx
	movb %bl, %dl
	movb %bl, %dh
	movl %edx, (%edi,%ecx,4)
	decl %ecx
	jnz .Lrefresh_1_2x
	_leave




	.globl refresh_2_2x
refresh_2_2x:
	_enter
	subl $2, %esi
	subl $8, %edi
	shrl $1, %ecx
.Lrefresh_2_2x:
	movb (%esi,%ecx,2), %al
	movb 1(%esi,%ecx,2), %bl
	movw (%ebp,%eax,2), %dx
	rorl $16, %edx
	movw (%ebp,%eax,2), %dx
	movl %edx, (%edi,%ecx,8)
	movw (%ebp,%ebx,2), %dx
	rorl $16, %edx
	movw (%ebp,%ebx,2), %dx
	movl %edx, 4(%edi,%ecx,8)
	decl %ecx
	jnz .Lrefresh_2_2x
	_leave



	.globl refresh_4_2x
refresh_4_2x:
	_enter
	subl $2, %esi
	subl $16, %edi
.Lrefresh_4_2x:
	movb (%esi,%ecx), %al
	movb 1(%esi,%ecx), %bl
	movl (%ebp,%eax,4), %edx
	movl %edx, (%edi,%ecx,8)
	movl %edx, 4(%edi,%ecx,8)
	movl (%ebp,%ebx,4), %edx
	movl %edx, 8(%edi,%ecx,8)
	movl %edx, 12(%edi,%ecx,8)
	subl $2, %ecx
	jnz .Lrefresh_4_2x
	_leave



	.globl refresh_4_4x
refresh_4_4x:
	_enter
	shll $4, %ecx
	addl %ecx, %edi
	shrl $5, %ecx
	subl $2, %esi
	subl $32, %edi
.Lrefresh_4_4x:
	movb (%esi,%ecx,2), %al
	movb 1(%esi,%ecx,2), %bl
	movl (%ebp,%eax,4), %edx
	movl %edx, (%edi)
	movl %edx, 4(%edi)
	movl %edx, 8(%edi)
	movl %edx, 12(%edi)
	movl (%ebp,%ebx,4), %edx
	movl %edx, 16(%edi)
	movl %edx, 20(%edi)
	movl %edx, 24(%edi)
	movl %edx, 28(%edi)
	subl $32, %edi
	decl %ecx
	jnz .Lrefresh_4_4x
	_leave






