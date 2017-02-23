	.file	"task.c"
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.text
.Ltext0:
	.local	tasks_base
	.comm	tasks_base,120,32
	.local	cur_task
	.comm	cur_task,8,8
.globl kpl_task_get_tid
	.type	kpl_task_get_tid, @function
kpl_task_get_tid:
.LFB0:
	.file 1 "task.c"
	.loc 1 26 0
	.cfi_startproc
	pushq	%rbp
.LCFI0:
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
.LCFI1:
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	.loc 1 27 0
	movq	-8(%rbp), %rdx
	movl	$tasks_base, %eax
	movq	%rdx, %rcx
	subq	%rax, %rcx
	movq	%rcx, %rax
	movq	%rax, %rdx
	sarq	$3, %rdx
	movabsq	$-6148914691236517205, %rax
	imulq	%rdx, %rax
	.loc 1 28 0
	leave
	ret
	.cfi_endproc
.LFE0:
	.size	kpl_task_get_tid, .-kpl_task_get_tid
.globl kpl_task_create
	.type	kpl_task_create, @function
kpl_task_create:
.LFB1:
	.loc 1 36 0
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
	.loc 1 39 0
	movl	$0, %eax
	call	_task_get_free_slot
	movq	%rax, -8(%rbp)
	.loc 1 41 0
	cmpq	$0, -8(%rbp)
	jne	.L4
	.loc 1 42 0
	movl	$-1, %eax
	jmp	.L5
.L4:
	.loc 1 44 0
	movq	-8(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rdx, 8(%rax)
	.loc 1 45 0
	movq	-8(%rbp), %rax
	movq	$0, (%rax)
	.loc 1 47 0
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	kpl_task_get_tid
.L5:
	.loc 1 48 0
	leave
	ret
	.cfi_endproc
.LFE1:
	.size	kpl_task_create, .-kpl_task_create
.globl kpl_task_exit
	.type	kpl_task_exit, @function
kpl_task_exit:
.LFB2:
	.loc 1 55 0
	.cfi_startproc
	pushq	%rbp
.LCFI4:
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
.LCFI5:
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, %eax
	movb	%al, -20(%rbp)
	.loc 1 56 0
	movsbl	-20(%rbp),%eax
	movl	%eax, %edi
	call	_task_get
	movq	%rax, -8(%rbp)
	.loc 1 58 0
	cmpq	$0, -8(%rbp)
	je	.L9
	.loc 1 59 0
	movq	-8(%rbp), %rax
	movq	$0, 8(%rax)
	.loc 1 60 0
	movq	-8(%rbp), %rax
	movb	$0, 16(%rax)
.L9:
	.loc 1 62 0
	leave
	ret
	.cfi_endproc
.LFE2:
	.size	kpl_task_exit, .-kpl_task_exit
.globl kpl_task_get_port
	.type	kpl_task_get_port, @function
kpl_task_get_port:
.LFB3:
	.loc 1 70 0
	.cfi_startproc
	pushq	%rbp
.LCFI6:
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
.LCFI7:
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, %eax
	movb	%al, -20(%rbp)
	.loc 1 71 0
	movsbl	-20(%rbp),%eax
	movl	%eax, %edi
	call	_task_get
	movq	%rax, -8(%rbp)
	.loc 1 73 0
	movq	-8(%rbp), %rax
	movq	8(%rax), %rax
	testq	%rax, %rax
	je	.L11
	.loc 1 74 0
	movq	-8(%rbp), %rax
	addq	$16, %rax
	jmp	.L12
.L11:
	.loc 1 76 0
	movl	$0, %eax
.L12:
	.loc 1 77 0
	leave
	ret
	.cfi_endproc
.LFE3:
	.size	kpl_task_get_port, .-kpl_task_get_port
.globl kpl_task_get_current
	.type	kpl_task_get_current, @function
kpl_task_get_current:
.LFB4:
	.loc 1 83 0
	.cfi_startproc
	pushq	%rbp
.LCFI8:
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
.LCFI9:
	.cfi_def_cfa_register 6
	.loc 1 84 0
	movq	cur_task(%rip), %rax
	movq	%rax, %rdi
	call	kpl_task_get_tid
	.loc 1 85 0
	leave
	ret
	.cfi_endproc
.LFE4:
	.size	kpl_task_get_current, .-kpl_task_get_current
.globl main
	.type	main, @function
main:
.LFB5:
	.loc 1 88 0
	.cfi_startproc
	pushq	%rbp
.LCFI10:
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
.LCFI11:
	.cfi_def_cfa_register 6
.L20:
	.loc 1 90 0
	movq	$tasks_base, cur_task(%rip)
	jmp	.L17
.L19:
	.loc 1 93 0
	movq	cur_task(%rip), %rax
	movq	8(%rax), %rax
	testq	%rax, %rax
	je	.L18
	.loc 1 94 0
	movq	cur_task(%rip), %rax
	movq	8(%rax), %rdx
	movq	cur_task(%rip), %rax
	movq	%rax, %rdi
	call	*%rdx
	.loc 1 95 0
	movl	$1, %edi
	movl	$0, %eax
	call	sleep
.L18:
	.loc 1 92 0
	movq	cur_task(%rip), %rax
	addq	$24, %rax
	movq	%rax, cur_task(%rip)
.L17:
	.loc 1 91 0
	movq	cur_task(%rip), %rdx
	movl	$tasks_base+120, %eax
	.loc 1 90 0
	cmpq	%rax, %rdx
	jb	.L19
	.loc 1 98 0
	jmp	.L20
	.cfi_endproc
.LFE5:
	.size	main, .-main
	.type	_task_get_free_slot, @function
_task_get_free_slot:
.LFB6:
	.loc 1 104 0
	.cfi_startproc
	pushq	%rbp
.LCFI12:
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
.LCFI13:
	.cfi_def_cfa_register 6
	pushq	%rbx
	.loc 1 107 0
	movl	$0, %ebx
	.cfi_offset 3, -24
	jmp	.L23
.L26:
	.loc 1 108 0
	movsbl	%bl,%eax
	movslq	%eax,%rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	tasks_base+8(%rax), %rax
	testq	%rax, %rax
	jne	.L24
	.loc 1 109 0
	movsbq	%bl,%rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	addq	$tasks_base, %rax
	jmp	.L25
.L24:
	.loc 1 107 0
	addl	$1, %ebx
.L23:
	cmpb	$4, %bl
	jle	.L26
	.loc 1 111 0
	movl	$0, %eax
.L25:
	.loc 1 112 0
	popq	%rbx
	leave
	ret
	.cfi_endproc
.LFE6:
	.size	_task_get_free_slot, .-_task_get_free_slot
	.type	_task_get, @function
_task_get:
.LFB7:
	.loc 1 115 0
	.cfi_startproc
	pushq	%rbp
.LCFI14:
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
.LCFI15:
	.cfi_def_cfa_register 6
	movl	%edi, %eax
	movb	%al, -4(%rbp)
	.loc 1 116 0
	cmpb	$0, -4(%rbp)
	js	.L29
	cmpb	$4, -4(%rbp)
	jle	.L30
.L29:
	.loc 1 117 0
	movl	$0, %eax
	jmp	.L31
.L30:
	.loc 1 119 0
	movsbq	-4(%rbp),%rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	addq	$tasks_base, %rax
.L31:
	.loc 1 120 0
	leave
	ret
	.cfi_endproc
.LFE7:
	.size	_task_get, .-_task_get
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
.LLST4:
	.quad	.LFB4-.Ltext0
	.quad	.LCFI8-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI8-.Ltext0
	.quad	.LCFI9-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI9-.Ltext0
	.quad	.LFE4-.Ltext0
	.value	0x2
	.byte	0x76
	.sleb128 16
	.quad	0x0
	.quad	0x0
.LLST5:
	.quad	.LFB5-.Ltext0
	.quad	.LCFI10-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI10-.Ltext0
	.quad	.LCFI11-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI11-.Ltext0
	.quad	.LFE5-.Ltext0
	.value	0x2
	.byte	0x76
	.sleb128 16
	.quad	0x0
	.quad	0x0
.LLST6:
	.quad	.LFB6-.Ltext0
	.quad	.LCFI12-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI12-.Ltext0
	.quad	.LCFI13-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI13-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x2
	.byte	0x76
	.sleb128 16
	.quad	0x0
	.quad	0x0
.LLST7:
	.quad	.LFB7-.Ltext0
	.quad	.LCFI14-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI14-.Ltext0
	.quad	.LCFI15-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI15-.Ltext0
	.quad	.LFE7-.Ltext0
	.value	0x2
	.byte	0x76
	.sleb128 16
	.quad	0x0
	.quad	0x0
	.file 2 "/usr/include/stdint.h"
	.file 3 "../include/msg.h"
	.file 4 "../include/port.h"
	.file 5 "../include/task.h"
	.section	.debug_info
	.long	0x33d
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
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF17
	.uleb128 0xd
	.byte	0x8
	.byte	0x7
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF18
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF19
	.uleb128 0xe
	.byte	0x1
	.long	.LASF20
	.byte	0x1
	.byte	0x19
	.byte	0x1
	.long	0x2d
	.quad	.LFB0
	.quad	.LFE0
	.long	.LLST0
	.long	0x19c
	.uleb128 0xf
	.long	.LASF22
	.byte	0x1
	.byte	0x19
	.long	0x148
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0x0
	.uleb128 0xe
	.byte	0x1
	.long	.LASF21
	.byte	0x1
	.byte	0x23
	.byte	0x1
	.long	0x2d
	.quad	.LFB1
	.quad	.LFE1
	.long	.LLST1
	.long	0x1de
	.uleb128 0xf
	.long	.LASF14
	.byte	0x1
	.byte	0x23
	.long	0x12b
	.byte	0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x10
	.long	.LASF22
	.byte	0x1
	.byte	0x25
	.long	0x148
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0x0
	.uleb128 0x11
	.byte	0x1
	.long	.LASF31
	.byte	0x1
	.byte	0x36
	.byte	0x1
	.quad	.LFB2
	.quad	.LFE2
	.long	.LLST2
	.long	0x21c
	.uleb128 0x12
	.string	"tid"
	.byte	0x1
	.byte	0x36
	.long	0x2d
	.byte	0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x10
	.long	.LASF22
	.byte	0x1
	.byte	0x38
	.long	0x148
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0x0
	.uleb128 0xe
	.byte	0x1
	.long	.LASF23
	.byte	0x1
	.byte	0x45
	.byte	0x1
	.long	0x25e
	.quad	.LFB3
	.quad	.LFE3
	.long	.LLST3
	.long	0x25e
	.uleb128 0x12
	.string	"tid"
	.byte	0x1
	.byte	0x45
	.long	0x2d
	.byte	0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x10
	.long	.LASF22
	.byte	0x1
	.byte	0x47
	.long	0x148
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0x0
	.uleb128 0x9
	.byte	0x8
	.long	0xde
	.uleb128 0x13
	.byte	0x1
	.long	.LASF24
	.byte	0x1
	.byte	0x52
	.long	0x2d
	.quad	.LFB4
	.quad	.LFE4
	.long	.LLST4
	.uleb128 0x13
	.byte	0x1
	.long	.LASF14
	.byte	0x1
	.byte	0x57
	.long	0x46
	.quad	.LFB5
	.quad	.LFE5
	.long	.LLST5
	.uleb128 0x14
	.long	.LASF32
	.byte	0x1
	.byte	0x67
	.long	0x148
	.quad	.LFB6
	.quad	.LFE6
	.long	.LLST6
	.long	0x2d3
	.uleb128 0x15
	.string	"i"
	.byte	0x1
	.byte	0x69
	.long	0x2d
	.byte	0x1
	.byte	0x53
	.byte	0x0
	.uleb128 0x16
	.long	.LASF33
	.byte	0x1
	.byte	0x72
	.byte	0x1
	.long	0x148
	.quad	.LFB7
	.quad	.LFE7
	.long	.LLST7
	.long	0x306
	.uleb128 0x12
	.string	"tid"
	.byte	0x1
	.byte	0x72
	.long	0x2d
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.byte	0x0
	.uleb128 0x17
	.long	0xe9
	.long	0x316
	.uleb128 0x18
	.long	0x157
	.byte	0x4
	.byte	0x0
	.uleb128 0x10
	.long	.LASF25
	.byte	0x1
	.byte	0xe
	.long	0x306
	.byte	0x9
	.byte	0x3
	.quad	tasks_base
	.uleb128 0x10
	.long	.LASF26
	.byte	0x1
	.byte	0xf
	.long	0x148
	.byte	0x9
	.byte	0x3
	.quad	cur_task
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
	.uleb128 0x49
	.uleb128 0x13
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
	.uleb128 0xe
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
	.uleb128 0xe
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
	.uleb128 0x12
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
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x14
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
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
	.uleb128 0x15
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
	.uleb128 0x16
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
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
	.uleb128 0x17
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x18
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",@progbits
	.long	0x81
	.value	0x2
	.long	.Ldebug_info0
	.long	0x341
	.long	0x168
	.string	"kpl_task_get_tid"
	.long	0x19c
	.string	"kpl_task_create"
	.long	0x1de
	.string	"kpl_task_exit"
	.long	0x21c
	.string	"kpl_task_get_port"
	.long	0x264
	.string	"kpl_task_get_current"
	.long	0x284
	.string	"main"
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
.LASF25:
	.string	"tasks_base"
.LASF21:
	.string	"kpl_task_create"
.LASF28:
	.string	"task.c"
.LASF22:
	.string	"task"
.LASF26:
	.string	"cur_task"
.LASF3:
	.string	"int8_t"
.LASF13:
	.string	"kpl_task"
.LASF9:
	.string	"data"
.LASF32:
	.string	"_task_get_free_slot"
.LASF31:
	.string	"kpl_task_exit"
.LASF5:
	.string	"unsigned char"
.LASF30:
	.string	"_kpl_task"
.LASF8:
	.string	"long unsigned int"
.LASF6:
	.string	"short unsigned int"
.LASF11:
	.string	"busy"
.LASF23:
	.string	"kpl_task_get_port"
.LASF1:
	.string	"short int"
.LASF14:
	.string	"main"
.LASF24:
	.string	"kpl_task_get_current"
.LASF7:
	.string	"unsigned int"
.LASF19:
	.string	"long long unsigned int"
.LASF4:
	.string	"uint8_t"
.LASF20:
	.string	"kpl_task_get_tid"
.LASF17:
	.string	"long long int"
.LASF27:
	.string	"GNU C 4.4.4"
.LASF10:
	.string	"kpl_msg"
.LASF15:
	.string	"port"
.LASF29:
	.string	"/home/felipe/ElectroLinux/k2.exp2/System/core"
.LASF33:
	.string	"_task_get"
.LASF12:
	.string	"kpl_port"
.LASF2:
	.string	"long int"
.LASF18:
	.string	"char"
.LASF0:
	.string	"signed char"
.LASF16:
	.string	"kpl_task_main"
	.ident	"GCC: (Debian 4.4.4-7) 4.4.4"
	.section	.note.GNU-stack,"",@progbits
