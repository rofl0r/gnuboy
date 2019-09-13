 //
 // Derived from free code by Martynas Kunigelis,
 // who is in no way associated with this project.
 //

 //
 // Keyboard interrupt hooking example.
 // The author is not responsible for any
 // damage this code could do to you or your property.
 //
 //            Martynas Kunigelis, 1996/13/05
 //            e-mail:  Martynas.Kunigelis@vm.ktu.lt
 //
		.file	"keyboard.s"
 //
 // externals (optional):
 //
		.extern ___djgpp_base_address
		.extern ___djgpp_ds_alias
 		.extern	___djgpp_dos_sel
 //
 // public functions and variables:
 //
		.global _keyboard_buffer
		.global _keyboard_buffer_pos
		.global _keyboard_init
		.global _keyboard_close
		.global _keyboard_chain

		.text
		
		.align 	4

locking_region_start:

_keyboard_buffer:	.space 	0x200, 0
_keyboard_buffer_pos:	.long	0

old_vector:
old_vector_ofs:	.long	0
old_vector_sel:	.word	0
chain_flag:	.long 	1


		.align	4

handler_procedure:

 //
 // .. will be called every time a key is pressed/released
 //                    
		pushl	%eax
		pushl	%edx
		pushw	%ds
 //
 // Load DS with our data selector
 //
 		movw	%cs:___djgpp_ds_alias, %ds
 //
 // Read the scancode from keyboard port and update keyboard_map
 //
		movl	_keyboard_buffer_pos, %edx
		cmpl	$0x200, %edx
		je	skip_buffering
		inb	$0x60, %al
		cmpb	$0xE0, %al
		jne	normal_key
		cmpl	$0x1FF, %edx
		je	skip_buffering
normal_key:
		movb	%al, _keyboard_buffer(%edx)
		incl	%edx
		movl	%edx, _keyboard_buffer_pos
skip_buffering:

 //
 // Chain if flag is set, otherwise do what's necessary and return
 //
		cmpl	$0, chain_flag
		jne	handler_chain
 //
 // Acknowledge keyboard and interrupt contollers
 //
		inb	$0x61, %al
		orb	$0x80, %al
		outb	%al, $0x61
		andb	$0x7f, %al
		outb	%al, $0x61
		movb	$0x20, %al
		outb	%al, $0x20

		popw	%ds
		popl	%edx
		popl	%eax
		sti
		iret

		.align	4

handler_chain:  popw	%ds
		popl	%edx
		popl	%eax
		ljmp	*%cs:(old_vector)

locking_region_end:


		.align	4
_keyboard_init:

 //
 // int keyboard_init(void);
 //
 // Initializes the keyboard handler and hooks the keyboard interrupt.
 // Returns -1 on failure, zero on success
 //
		pushl	%esi
		pushl	%edi
		pushl	%ebx
 //
 // First, we need to lock the handler and memory it touches, so
 // it doesn't get swapped out to disk.
 //
		leal	locking_region_start, %ecx
		leal	locking_region_end, %edi
		subl	%ecx, %edi
		addl	___djgpp_base_address, %ecx
		shldl	$16, %ecx, %ebx		// ecx -> bx:cx
		shldl	$16, %edi, %esi         // edi -> si:di
		movw    $0x0600, %ax		// lock linear region
		int	$0x31
		jc	init_error
 //
 // Now we need to save the old interrupt vector, so we can restore
 // it later and also to know where to jump if chaining.
 //
		movw	$0x0204, %ax    	// get pm int vector
		movb	$0x09, %bl
		int	$0x31
		movw	%cx, old_vector_sel
		movl	%edx, old_vector_ofs
 //
 // Make sure we chain after initialization.
 //
		movl	$1, chain_flag
 //
 // Set the interrupt vector to point to our handler.
 //
		movw	%cs, %cx    		
		leal	handler_procedure, %edx
		movb	$0x09, %bl
		movw	$0x0205, %ax    	// set pm int vector
		int	$0x31
 //*
 //* Actually we would have to unlock the locked region on failure
 //* here. But since most programs would exit with an error message
 //* in such case, there's no need to worry.
 //*

init_error:

 //
 // This sets EAX to -1 if CF is set and to 0 atherwise
 //
		movl	$0, %eax
		sbbl	$0, %eax
		
		popl	%ebx
		popl	%edi
		popl	%esi
		ret


		.align 4
_keyboard_close:

 //
 // void keyboard_close(void);
 //
 // Shuts the keyboard handler down.
 //
		pushl	%esi
		pushl	%edi
		pushl	%ebx
 //
 // Unlock the region we locked at initialization
 // 
		leal	locking_region_start, %ecx
		leal	locking_region_end, %edi
		subl	%ecx, %edi
		addl	___djgpp_base_address, %ecx
		shldl	$16, %ecx, %ebx
		shldl	$16, %edi, %esi
		movw	$0x0601, %ax  		// unlock linear region
		int	$0x31                   
 //
 // Restore the interrupt vector to its previous value
 //
		movw	old_vector_sel, %cx
		movl	old_vector_ofs, %edx
		movb	$0x09, %bl
		movw	$0x0205, %ax            // set pm int vector
		int	$0x31

		popl	%ebx
		popl	%edi
		popl	%esi
		ret
 //
 // void keyboard_chain(int toggle);
 //
		.align	4
_keyboard_chain:
		cmpl	$0, 4(%esp)
		je	chain_off
chain_on:

 //
 // Set the chain_flag and clear BIOS shift/ctrl/alt status bits:
 //
		movl	$1, chain_flag

		push	%es
		movw	___djgpp_dos_sel, %es
		andb	$0xf0, %es:0x417
		pop	%es
		jmp	chain_done
chain_off:
		movl	$0, chain_flag
chain_done:     ret



 //
 //  "Hey pig, nothing's turning out the way I planned."
 //  - Nine Inch Nails
 //
