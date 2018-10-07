
.section .text

/* 
 * BUG Not using appriopiarte stack due to handler mode. (MSP not PSP)
 *
 * TODO There is bug, because of isr is handled in handler mode which uses main
 * stack, and all thread context is saved on process stack which cannot be
 * accesed in simple way from handler mode.
 *
 * During PendSV contexts of threads are being switched. Problem is with
 * pushing and poping registers from stack because in handler mode main sp is
 * used and threads context is stored in process sp. This leads to not poping
 * all of registers from faked stack (process stack), and pushing context on
 * main stack while register should be pushed on thread stack (process stack).
 *
 * This occurs on first switch of context. Faked stack contains about 12
 * registers which should be popped. They are popped at the end of ISR and only
 * exception stack frame. But in faked stack all first registers which wasn't
 * popped from this faked one. First registers are 0, this leads to popping 0
 * to PC and results in mem fault.
 * 
 * The most probably soloution for this will be restoring and switching context
 * by accessing stack (memory) directly from values passed in r0 and r1. 
 * This solution will add extra time to context switch
 */
.global mleko
.thumb
.type mleko, %function
mleko:
	/*
	 * This happens during ISR
	 * 
	 * In r0 and r1 are addresses of pointers to thread stack
	 */

	/* disable interrrupts */
	cpsid i


	/* 
	 * Save rest of stack frame
	 * 
	 * Push rest of registers, rest is pushed by entering handler mode.
	 */
	//push {r4-r7}

	/* push register r4 - r7 to process stack */
	ldr r3, [r1]

	sub r3, r3, #4
	str r4, [r3]		/* Store r4 on stack */
	
	sub r3, r3, #4
	str r5, [r3]		/* Store r5 on stack */

	sub r3, r3, #4
	str r6, [r3]		/* Store r6 on stack */

	sub r3, r3, #4
	str r7, [r3]		/* Store r7 on stack */

	/* Store sp on thread stack */
	str r3, [r1]
	

	/* load new stack pointer */
	ldr r3, [r0]


	/* Pop r4 - r7 from process stack pointer */
	ldr r7, [r3]
	add r3, r3, #4

	ldr r6, [r3]
	add r3, r3, #4

	ldr r5, [r3]
	add r3, r3, #4

	ldr r4, [r3]
	add r3, r3, #4

	/* save stack pointer on thread stack */
	str r3, [r0]

	/* Load new stack pointer */
	msr psp, r3

	/* 
	 * Restore part of thread context, rest is restored by quiting handler mode
	 */
	//pop {r4-r7}                      

	/*
	 * Load 0xfffffffd to lr to switch to thread stack
	 */
	mov r0, #0
	sub r0, r0, #3		/* Should be 0xfffffffd */
	mov lr, r0

	/* enable interrrupts */         
	cpsie i

	/* 
	 * return from ISR by branching to lr, and pop rest of registers
	 */
	bx lr

	/*
	 * End of ISR registers r0-r3, r12, lr, pc, xPSR are popped
	 */
	nop


.align 2
.size mleko, .-mleko

/*
 * This function loads context of first thread to be executed.
 * This function returns to address of function on stack which was forged on
 *     in thread_create function.
 *
 * This is executed in thread mode, it means that no interrupt is pending.
 *
 * This function is executed once. At the end of start_schedule function. 
 *
 *
 *   Stack looks lile this
 *   All register except xPSR, PC are 0
 *
 *              +------------+
 *              |    xPSR    |
 *              |     PC     |
 *              |     LR     |
 *              |     r12    |
 *              |     r3     |
 *              |     r2     |
 *              |     r1     |
 *              |     r0     |
 *              |     r4     |
 *              |     r5     |
 *              |     r6     |
 *        SP -> |     r7     |
 *              +------------+
 *
 * Registers from r0 to xPSR are for ISR.
 *
 * Stack is designed to be popped by end of ISR.
 *
 * High registers (r8 - r12) cannot be directly pushed and poped.
 *
 */
.global load_context
.thumb
.type load_context, %function
load_context:
	
	/* Interrupts are disabled */
	/* Don't care about previous stack */
	/* Because never returns to previus stack frame */

	msr psp, r0			/* Mov new stack pointer to psp register */
	mov r0, #2			/* Switches to thread stack pointer */
	msr control, r0
	isb					/* Changing stack pointer must be followed by this
						   intruction as it is said in Generic user guide */

	/* Pop thread context */
	pop {r4-r7}

	/* 
	 * These registers on stack are in reversed order
	 */
	pop {r0}
	pop {r1}
	pop {r2}
	pop {r3}

	/*
	 * Push r0 because it will be used to pop high registers
	 */
	push {r0}

	/* pop r12 */
	ldr r0, [sp, #4]	/* with offset because r0 is on stack */
	mov r12, r0
	//add sp, sp, #4

	/* pop lr */
	ldr r0, [sp, #8]	/* with offset becaues r0 is on stack */
	mov lr, r0
	//add sp, sp, #4

	/* Get xPSR */	
	ldr r0, [sp, #12] 	/* with offset becaues r0 is on stack */
	msr epsr, r0		/* store xpsr */

	pop {r0}			/* Finally pop r0 */

	add sp, sp, #8

	/* enable interrupts */
	cpsie i

	pop {pc}			/* starts running thread function which was on stack */
.align 2
.size load_context, .-load_context



.global PendSV_Handler
.thumb
.type PendSV_Handler, %function
PendSV_Handler:

	/* Disable interrupts */
	cpsid i

	/* Update stack pointer in thread control block */	
	mrs r2, psp
	str r2, [r1]

	/* Push rest of thraed context on thread stack */	
	ldr r2, [r1]		/* Get pointer to threads stack */

	sub r2, #4			/* Store r4 */
	str r4, [r2]

	sub r2, #4			/* Store r5 */
	str r5, [r2]

	sub r2, #4			/* Store r6 */
	str r6, [r2]

	sub r2, #4			/* Store r7 */
	str r7, [r2]

	str r2, [r1]		/* Store pointer in thread control block */

	/* Load new thread context */
	ldr r2, [r0]

	ldr r7, [r2]		/* Restore r7 */
	add r2, #4


	ldr r6, [r2]		/* Restore r6 */
	add r2, #4


	ldr r5, [r2]		/* Restore r5 */
	add r2, #4


	ldr r4, [r2]		/* Restore r4 */
	add r2, #4

	str r2, [r0]		/* Store pointer in thread control block */

	/* Change value in PSP to point to new thread stack */
	msr psp, r2

	/*
	 * Load 0xfffffffd to lr to switch to thread stack
	 */
	//mov r0, #0
	//sub r0, r0, #3		/* Should be 0xfffffffd */
	//mov lr, r0

	/* Enable interrupts */
	cpsie i

	/* 
	 * return from ISR by branching to lr, and pop rest of registers
	 */
	bx lr

.align 2
.size PendSV_Handler, .-PendSV_Handler
