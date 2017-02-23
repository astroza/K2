	.file	"test.c"
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.text
.Ltext0:
	.comm	t1,8,8
	.comm	dst,1,1
	.section	.rodata
.LC0:
	.string	"WAIT: Task=%d Line=%d\n"
	.align 8
.LC1:
	.string	"Llego mensaje desde %hhu data=%hhu\n"
	.text
.globl task0
	.type	task0, @function
task0:
.LFB0:
	.file 1 "test.c"
	.loc 1 9 0
	.cfi_startproc
	pushq	%rbp
.LCFI0:
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
.LCFI1:
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	.loc 1 10 0
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	-24(%rbp), %rax
	movq	$0, (%rax)
	cmpq	$0, -8(%rbp)
	je	.L2
	movq	-8(%rbp), %rax
	jmp	*%rax
.L2:
.LBB2:
	.loc 1 13 0
	movl	$0, %edi
	call	time
	movq	%rax, t1(%rip)
.L3:
	.loc 1 15 0
	leaq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	kpl_msg_recv
	testb	%al, %al
	jne	.L4
	movq	-24(%rbp), %rax
	movq	$.L3, (%rax)
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	kpl_task_get_tid
	movsbl	%al,%ecx
	movl	$.LC0, %eax
	movl	$15, %edx
	movl	%ecx, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	jmp	.L7
.L4:
	.loc 1 16 0
	movq	-16(%rbp), %rax
	movzbl	2(%rax), %eax
	movzbl	%al, %edx
	movq	-16(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al,%ecx
	movl	$.LC1, %eax
	movl	%ecx, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
.L6:
	.loc 1 17 0
	movl	$0, %edi
	call	time
	movq	t1(%rip), %rdx
	subq	%rdx, %rax
	cmpq	$4, %rax
	jg	.L7
	movq	-24(%rbp), %rax
	movq	$.L6, (%rax)
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	kpl_task_get_tid
	movsbl	%al,%ecx
	movl	$.LC0, %eax
	movl	$17, %edx
	movl	%ecx, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	nop
.L7:
.LBE2:
	.loc 1 19 0
	leave
	ret
	.cfi_endproc
.LFE0:
	.size	task0, .-task0
	.section	.rodata
.LC2:
	.string	"Enviado"
	.text
.globl task1
	.type	task1, @function
task1:
.LFB1:
	.loc 1 22 0
	.cfi_startproc
	pushq	%rbp
.LCFI2:
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
.LCFI3:
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	.loc 1 23 0
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	-24(%rbp), %rax
	movq	$0, (%rax)
	cmpq	$0, -8(%rbp)
.L9:
	.loc 1 24 0
	movzbl	dst(%rip), %eax
	movsbl	%al,%eax
	movl	$88, %esi
	movl	%eax, %edi
	call	kpl_msg_send
	testb	%al, %al
	jne	.L10
	movq	-24(%rbp), %rax
	movq	$.L9, (%rax)
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	kpl_task_get_tid
	movsbl	%al,%ecx
	movl	$.LC0, %eax
	movl	$24, %edx
	movl	%ecx, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf
	jmp	.L12
.L10:
	.loc 1 25 0
	movl	$.LC2, %edi
	call	puts
.L12:
	.loc 1 27 0
	leave
	ret
	.cfi_endproc
.LFE1:
	.size	task1, .-task1
.globl init0
	.type	init0, @function
init0:
.LFB2:
	.loc 1 30 0
	.cfi_startproc
	pushq	%rbp
.LCFI4:
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
.LCFI5:
	.cfi_def_cfa_register 6
	.loc 1 31 0
	movl	$task0, %edi
	call	kpl_task_create
	movb	%al, dst(%rip)
	.loc 1 32 0
	movl	$task1, %edi
	call	kpl_task_create
	.loc 1 33 0
	leave
	ret
	.cfi_endproc
.LFE2:
	.size	init0, .-init0
	.section	.ctors,"aw",@progbits
	.align 8
	.quad	init0
	.section	.rodata
.LC3:
	.string	"Test"
	.text
.globl init1
	.type	init1, @function
init1:
.LFB3:
	.loc 1 36 0
	.cfi_startproc
	pushq	%rbp
.LCFI6:
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
.LCFI7:
	.cfi_def_cfa_register 6
	.loc 1 37 0
	movl	$.LC3, %edi
	call	puts
	.loc 1 38 0
	leave
	ret
	.cfi_endproc
.LFE3:
	.size	init1, .-init1
	.section	.ctors
	.align 8
	.quad	init1
	.text
.Letext0:
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST0:
	.quad	.LFB0-.Ltext0
	.quad	.LCFI0-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI0-.Ltext0
	.quad	.LCFI1-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI1-.Ltext0
	.quad	.LFE0-.Ltext0
	.value	0x2
	.byte	0x76
	.sleb128 16
	.quad	0x0
	.quad	0x0
.LLST1:
	.quad	.LFB1-.Ltext0
	.quad	.LCFI2-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI2-.Ltext0
	.quad	.LCFI3-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI3-.Ltext0
	.quad	.LFE1-.Ltext0
	.value	0x2
	.byte	0x76
	.sleb128 16
	.quad	0x0
	.quad	0x0
.LLST2:
	.quad	.LFB2-.Ltext0
	.quad	.LCFI4-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI4-.Ltext0
	.quad	.LCFI5-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI5-.Ltext0
	.quad	.LFE2-.Ltext0
	.value	0x2
	.byte	0x76
	.sleb128 16
	.quad	0x0
	.quad	0x0
.LLST3:
	.quad	.LFB3-.Ltext0
	.quad	.LCFI6-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI6-.Ltext0
	.quad	.LCFI7-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI7-.Ltext0
	.quad	.LFE3-.Ltext0
	.value	0x2
	.byte	0x76
	.sleb128 16
	.quad	0x0
	.quad	0x0
	.file 2 "/usr/include/stdint.h"
	.file 3 "../include/msg.h"
	.file 4 "../include/port.h"
	.file 5 "../include/task.h"
	.file 6 "/usr/include/bits/types.h"
	.file 7 "/usr/include/time.h"
	.section	.debug_info
	.long	0x29b
	.value	0x2
	.long	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.long	.LASF27
	.byte	0x1
	.long	.LASF28
	.long	.LASF29
	.quad	.Ltext0
	.quad	.Letext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.long	.LASF3
	.byte	0x2
	.byte	0x25
	.long	0x38
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF0
	.uleb128 0x3
	.byte	0x2
	.byte	0x5
	.long	.LASF1
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF2
	.uleb128 0x2
	.long	.LASF4
	.byte	0x2
	.byte	0x31
	.long	0x5f
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.long	.LASF5
	.uleb128 0x3
	.byte	0x2
	.byte	0x7
	.long	.LASF6
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.long	.LASF7
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF8
	.uleb128 0x5
	.byte	0x3
	.byte	0x3
	.byte	0x5
	.long	0xae
	.uleb128 0x6
	.string	"src"
	.byte	0x3
	.byte	0x6
	.long	0x2d
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x6
	.string	"dst"
	.byte	0x3
	.byte	0x7
	.long	0x2d
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x7
	.long	.LASF9
	.byte	0x3
	.byte	0x8
	.long	0x54
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.byte	0x0
	.uleb128 0x2
	.long	.LASF10
	.byte	0x3
	.byte	0x9
	.long	0x7b
	.uleb128 0x5
	.byte	0x4
	.byte	0x4
	.byte	0x6
	.long	0xde
	.uleb128 0x7
	.long	.LASF11
	.byte	0x4
	.byte	0x7
	.long	0x54
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x6
	.string	"msg"
	.byte	0x4
	.byte	0x8
	.long	0xae
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.byte	0x0
	.uleb128 0x2
	.long	.LASF12
	.byte	0x4
	.byte	0x9
	.long	0xb9
	.uleb128 0x2
	.long	.LASF13
	.byte	0x5
	.byte	0x6
	.long	0xf4
	.uleb128 0x8
	.long	.LASF30
	.byte	0x18
	.byte	0x5
	.byte	0x6
	.long	0x12b
	.uleb128 0x6
	.string	"pos"
	.byte	0x5
	.byte	0xa
	.long	0x14e
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x7
	.long	.LASF14
	.byte	0x5
	.byte	0xb
	.long	0x12b
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x7
	.long	.LASF15
	.byte	0x5
	.byte	0xd
	.long	0xde
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.byte	0x0
	.uleb128 0x2
	.long	.LASF16
	.byte	0x5
	.byte	0x7
	.long	0x136
	.uleb128 0x9
	.byte	0x8
	.long	0x13c
	.uleb128 0xa
	.byte	0x1
	.long	0x148
	.uleb128 0xb
	.long	0x148
	.byte	0x0
	.uleb128 0x9
	.byte	0x8
	.long	0xe9
	.uleb128 0xc
	.byte	0x8
	.uleb128 0xd
	.byte	0x8
	.byte	0x7
	.uleb128 0x2
	.long	.LASF17
	.byte	0x6
	.byte	0x95
	.long	0x4d
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF18
	.uleb128 0x2
	.long	.LASF19
	.byte	0x7
	.byte	0x4c
	.long	0x153
	.uleb128 0xe
	.byte	0x1
	.long	.LASF22
	.byte	0x1
	.byte	0x8
	.byte	0x1
	.quad	.LFB0
	.quad	.LFE0
	.long	.LLST0
	.long	0x1ea
	.uleb128 0xf
	.string	"t"
	.byte	0x1
	.byte	0x8
	.long	0x148
	.byte	0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x10
	.string	"pos"
	.byte	0x1
	.byte	0xa
	.long	0x14e
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x11
	.long	.LASF20
	.byte	0x1
	.byte	0xf
	.quad	.L3
	.uleb128 0x11
	.long	.LASF21
	.byte	0x1
	.byte	0x11
	.quad	.L6
	.uleb128 0x12
	.quad	.LBB2
	.quad	.LBE2
	.uleb128 0x10
	.string	"msg"
	.byte	0x1
	.byte	0xb
	.long	0x1ea
	.byte	0x2
	.byte	0x91
	.sleb128 -32
	.byte	0x0
	.byte	0x0
	.uleb128 0x9
	.byte	0x8
	.long	0xae
	.uleb128 0xe
	.byte	0x1
	.long	.LASF23
	.byte	0x1
	.byte	0x15
	.byte	0x1
	.quad	.LFB1
	.quad	.LFE1
	.long	.LLST1
	.long	0x23b
	.uleb128 0xf
	.string	"t"
	.byte	0x1
	.byte	0x15
	.long	0x148
	.byte	0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x10
	.string	"pos"
	.byte	0x1
	.byte	0x17
	.long	0x14e
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x11
	.long	.LASF24
	.byte	0x1
	.byte	0x18
	.quad	.L9
	.byte	0x0
	.uleb128 0x13
	.byte	0x1
	.long	.LASF25
	.byte	0x1
	.byte	0x1d
	.quad	.LFB2
	.quad	.LFE2
	.long	.LLST2
	.uleb128 0x13
	.byte	0x1
	.long	.LASF26
	.byte	0x1
	.byte	0x23
	.quad	.LFB3
	.quad	.LFE3
	.long	.LLST3
	.uleb128 0x14
	.string	"t1"
	.byte	0x1
	.byte	0x5
	.long	0x165
	.byte	0x1
	.byte	0x9
	.byte	0x3
	.quad	t1
	.uleb128 0x14
	.string	"dst"
	.byte	0x1
	.byte	0x6
	.long	0x2d
	.byte	0x1
	.byte	0x9
	.byte	0x3
	.quad	dst
	.byte	0x0
	.section	.debug_abbrev
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
	.uleb128 0x16
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x6
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x7
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x8
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x9
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xa
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xb
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xc
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0xd
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0xe
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xf
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x10
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x11
	.uleb128 0xa
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.byte	0x0
	.byte	0x0
	.uleb128 0x12
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.byte	0x0
	.byte	0x0
	.uleb128 0x13
	.uleb128 0x2e
	.byte	0x0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x14
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",@progbits
	.long	0x45
	.value	0x2
	.long	.Ldebug_info0
	.long	0x29f
	.long	0x170
	.string	"task0"
	.long	0x1f0
	.string	"task1"
	.long	0x23b
	.string	"init0"
	.long	0x257
	.string	"init1"
	.long	0x273
	.string	"t1"
	.long	0x288
	.string	"dst"
	.long	0x0
	.section	.debug_aranges,"",@progbits
	.long	0x2c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x8
	.byte	0x0
	.value	0x0
	.value	0x0
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.quad	0x0
	.quad	0x0
	.section	.debug_str,"MS",@progbits,1
.LASF21:
	.string	"lb_17"
.LASF28:
	.string	"test.c"
.LASF3:
	.string	"int8_t"
.LASF20:
	.string	"lb_15"
.LASF13:
	.string	"kpl_task"
.LASF9:
	.string	"data"
.LASF5:
	.string	"unsigned char"
.LASF30:
	.string	"_kpl_task"
.LASF22:
	.string	"task0"
.LASF8:
	.string	"long unsigned int"
.LASF6:
	.string	"short unsigned int"
.LASF17:
	.string	"__time_t"
.LASF11:
	.string	"busy"
.LASF1:
	.string	"short int"
.LASF7:
	.string	"unsigned int"
.LASF18:
	.string	"char"
.LASF4:
	.string	"uint8_t"
.LASF19:
	.string	"time_t"
.LASF14:
	.string	"main"
.LASF23:
	.string	"task1"
.LASF27:
	.string	"GNU C 4.4.4"
.LASF10:
	.string	"kpl_msg"
.LASF24:
	.string	"lb_24"
.LASF15:
	.string	"port"
.LASF29:
	.string	"/home/felipe/ElectroLinux/k2.exp2/System/core"
.LASF12:
	.string	"kpl_port"
.LASF25:
	.string	"init0"
.LASF26:
	.string	"init1"
.LASF2:
	.string	"long int"
.LASF0:
	.string	"signed char"
.LASF16:
	.string	"kpl_task_main"
	.ident	"GCC: (Debian 4.4.4-7) 4.4.4"
	.section	.note.GNU-stack,"",@progbits
