	.file	"msg.c"
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.text
.Ltext0:
.globl kpl_msg_send
	.type	kpl_msg_send, @function
kpl_msg_send:
.LFB0:
	.file 1 "msg.c"
	.loc 1 11 0
	.cfi_startproc
	pushq	%rbp
.LCFI0:
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
.LCFI1:
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, %edx
	movl	%esi, %eax
	movb	%dl, -20(%rbp)
	movb	%al, -24(%rbp)
	.loc 1 12 0
	movsbl	-20(%rbp),%eax
	movl	%eax, %edi
	call	kpl_task_get_port
	movq	%rax, -8(%rbp)
	.loc 1 14 0
	cmpq	$0, -8(%rbp)
	je	.L2
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	.L3
.L2:
	.loc 1 15 0
	movl	$0, %eax
	jmp	.L4
.L3:
	.loc 1 17 0
	movl	$0, %eax
	call	kpl_task_get_current
	movq	-8(%rbp), %rdx
	movb	%al, 1(%rdx)
	.loc 1 18 0
	movq	-8(%rbp), %rax
	movzbl	-20(%rbp), %edx
	movb	%dl, 2(%rax)
	.loc 1 19 0
	movq	-8(%rbp), %rax
	movzbl	-24(%rbp), %edx
	movb	%dl, 3(%rax)
	.loc 1 20 0
	movq	-8(%rbp), %rax
	movb	$1, (%rax)
	.loc 1 22 0
	movl	$1, %eax
.L4:
	.loc 1 23 0
	leave
	ret
	.cfi_endproc
.LFE0:
	.size	kpl_msg_send, .-kpl_msg_send
.globl kpl_msg_recv
	.type	kpl_msg_recv, @function
kpl_msg_recv:
.LFB1:
	.loc 1 26 0
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
	.loc 1 27 0
	movl	$0, %eax
	call	kpl_task_get_current
	movsbl	%al,%eax
	movl	%eax, %edi
	call	kpl_task_get_port
	movq	%rax, -8(%rbp)
	.loc 1 29 0
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L7
	.loc 1 30 0
	movl	$0, %eax
	jmp	.L8
.L7:
	.loc 1 32 0
	movq	-8(%rbp), %rax
	movb	$0, (%rax)
	.loc 1 33 0
	movq	-8(%rbp), %rax
	leaq	1(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, (%rax)
	.loc 1 34 0
	movl	$1, %eax
.L8:
	.loc 1 35 0
	leave
	ret
	.cfi_endproc
.LFE1:
	.size	kpl_msg_recv, .-kpl_msg_recv
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
	.file 2 "/usr/include/stdint.h"
	.file 3 "../include/msg.h"
	.file 4 "../include/port.h"
	.section	.debug_info
	.long	0x1a2
	.value	0x2
	.long	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.long	.LASF19
	.byte	0x1
	.long	.LASF20
	.long	.LASF21
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
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.long	.LASF13
	.uleb128 0x8
	.byte	0x8
	.byte	0x7
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.long	.LASF14
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.long	.LASF15
	.uleb128 0x9
	.byte	0x1
	.long	.LASF16
	.byte	0x1
	.byte	0xa
	.byte	0x1
	.long	0x54
	.quad	.LFB0
	.quad	.LFE0
	.long	.LLST0
	.long	0x151
	.uleb128 0xa
	.string	"dst"
	.byte	0x1
	.byte	0xa
	.long	0x2d
	.byte	0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0xb
	.long	.LASF9
	.byte	0x1
	.byte	0xa
	.long	0x54
	.byte	0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0xc
	.long	.LASF18
	.byte	0x1
	.byte	0xc
	.long	0x151
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0x0
	.uleb128 0xd
	.byte	0x8
	.long	0xde
	.uleb128 0x9
	.byte	0x1
	.long	.LASF17
	.byte	0x1
	.byte	0x19
	.byte	0x1
	.long	0x54
	.quad	.LFB1
	.quad	.LFE1
	.long	.LLST1
	.long	0x199
	.uleb128 0xa
	.string	"msg"
	.byte	0x1
	.byte	0x19
	.long	0x199
	.byte	0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0xc
	.long	.LASF18
	.byte	0x1
	.byte	0x1b
	.long	0x151
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.byte	0x0
	.uleb128 0xd
	.byte	0x8
	.long	0x19f
	.uleb128 0xd
	.byte	0x8
	.long	0xae
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
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x9
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
	.uleb128 0xa
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
	.uleb128 0xb
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
	.uleb128 0xc
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
	.uleb128 0xd
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",@progbits
	.long	0x30
	.value	0x2
	.long	.Ldebug_info0
	.long	0x1a6
	.long	0x101
	.string	"kpl_msg_send"
	.long	0x157
	.string	"kpl_msg_recv"
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
.LASF13:
	.string	"long long int"
.LASF6:
	.string	"short unsigned int"
.LASF7:
	.string	"unsigned int"
.LASF8:
	.string	"long unsigned int"
.LASF17:
	.string	"kpl_msg_recv"
.LASF15:
	.string	"long long unsigned int"
.LASF20:
	.string	"msg.c"
.LASF12:
	.string	"kpl_port"
.LASF11:
	.string	"busy"
.LASF4:
	.string	"uint8_t"
.LASF5:
	.string	"unsigned char"
.LASF19:
	.string	"GNU C 4.4.4"
.LASF9:
	.string	"data"
.LASF3:
	.string	"int8_t"
.LASF2:
	.string	"long int"
.LASF18:
	.string	"port"
.LASF21:
	.string	"/home/felipe/ElectroLinux/k2.exp2/System/core"
.LASF0:
	.string	"signed char"
.LASF10:
	.string	"kpl_msg"
.LASF1:
	.string	"short int"
.LASF14:
	.string	"char"
.LASF16:
	.string	"kpl_msg_send"
	.ident	"GCC: (Debian 4.4.4-7) 4.4.4"
	.section	.note.GNU-stack,"",@progbits
