    .align 2
    .thumb_func
    .globl dispatch_entry

// called by the interrupt vector defined in boot/vector_table.c -> 14: Pend SV
dispatch_entry:
	// execution context -> stack
    push    {r4-r7}
    mov     r0, r8
    mov     r1, r9
    mov     r2, r10
    mov     r3, r11
    push    {r0-r3}

	// check the current task
    ldr     r0, =cur_task
    ldr     r1, [r0]
    cmp     r1, #0
    beq     disp_010    // if cur_task id == 0, goto disp_010

	// sp register -> ctx_table
    ldr     r0, =ctx_table
    sub     r1, #1
    lsl     r1, r1, #2
    mov     r2, sp
    str     r2, [r0, r1]

disp_010:
	// change the current task
    ldr     r0, =next_task
    ldr     r1, [r0]
    ldr     r0, =cur_task
    str     r1, [r0]

	// switch to a new stack
    ldr     r0, =ctx_table
    sub     r1, #1
    lsl     r1, r1, #2
    ldr     r2, [r0, r1]
    mov     sp, r2

	// restore execution context on stack
    pop	{r0-r3}
    mov	r11, r3
    mov	r10, r2
    mov	r9, r1
    mov	r8, r0
    pop	{r4-r7}
    bx	lr

