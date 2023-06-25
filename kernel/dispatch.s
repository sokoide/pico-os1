    .align 2
    .thumb_func
    .globl dispatch_entry

// called by the interrupt vector defined in boot/vector_table.c -> 14: Pend SV
dispatch_entry:
	// disable interrupt
    mov		r0, #1
    msr		primask, r0
    ldr		r1, =dispatch_running
    str		r0, [r1]

    // execution context -> stack
    push    {r4-r7}
    mov     r0, r8
    mov     r1, r9
    mov     r2, r10
    mov     r3, r11
    push    {r0-r3}

    // check the current task
    ldr     r0, =curr_task
    ldr     r1, [r0]
    cmp     r1, #0
    beq     disp_010    // if curr_task id == 0, goto disp_010


	// execution context pointer -> current tasks's TaskControlBlock
	mov r2, sp
	str r2, [r1]

disp_010:
	// get the next task
	ldr r1, =scheduled_task
	ldr r2, [r1]
	cmp r2, #0
	bne disp_030

	// if the next task doesn't exist
	str r2, [r0]
disp_020:
    mov		r3, #0
    msr		primask, r3 	// enable interrupt
    mov		r3, #1
    msr		primask, r3     // disable interrupt

    ldr     r2, [r1]
    cmp     r2, #0
    beq     disp_020

	// change the current task
disp_030:
    str     r2, [r0]
    ldr     r0, [r2]
    mov     sp, r0

    // restore execution context on stack
    pop    {r0-r3}
    mov    r11, r3
    mov    r10, r2
    mov    r9, r1
    mov    r8, r0
    pop    {r4-r7}

// enable interrupt
    ldr     r0, =dispatch_running
    mov     r1, #0
    str     r1, [r0]
    msr     primask, r1

    bx    lr

