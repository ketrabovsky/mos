/* Standard libraries */
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* Libraries provided by user */
#include <error.h>
#include <thread.h>

/* Processor specific library */
#include <stm32f4xx.h>

#define MAX_THREADS 32

#define PENDSV_BIT (1 << 28)

/* Thread struct */
struct thread {
	uint8_t *sp;
	uint32_t tid;			/* Thread ID */
	uint8_t *stack;
};

/* Runq struct:
 * Stores info about all running threads
 */
struct thread_runq {
	uint32_t current;
	uint32_t thread_count;
	struct thread *queue[MAX_THREADS];
};

/*
 * This is global variable which stores information about threads
 */
static struct thread_runq runq;

/*
 * brief: Places word (4 bytes) on stack. Stack is full descending
 *
 * param: stackp - addres of stack pointer
 *
 * param: word - value to be placed on stack
 */
static void push_stack(uint8_t **stackp, uint32_t word)
{
	uint32_t *stack;

	stack = (uint32_t *)*stackp;
	stack--;
	*stack = word; 

	*stackp = (uint8_t *)stack;
}

/*
 * brief: Creates fake stack for thread. Stack looks like it was preempted by
 * 		  interrupt plus rest of registers. which are not pushed during ISR
 *
 * param: stackp - points to stack
 *
 * param: stackSize - size of stack to be faked
 *
 * retval: Returns pointer to top of faked stack
 */
static uint8_t * thread_fake_stack(threadfn fn,
							   	   uint8_t *stackp,
							       uint32_t stackSize)
{
	uint8_t *stack;

	/* Calculate top of empty stack which is at the end of allocated memory */
	stack = stackp + stackSize;

	/* Push registers on stack */
	push_stack(&stack, (1 << 24));			/* xPSR */
	push_stack(&stack, (uint32_t)fn);		/* PC */
	push_stack(&stack, 0);					/* LR (should be thread destroy function*/
	push_stack(&stack, 0);					/* R12 */
	push_stack(&stack, 0);					/* R3 */
	push_stack(&stack, 0);					/* R2 */
	push_stack(&stack, 0);					/* R1 */
	push_stack(&stack, 0);					/* R0 */

	/* Push rest of registers */
	push_stack(&stack, 0);					/* R4 */
	push_stack(&stack, 0);					/* R5 */
	push_stack(&stack, 0);					/* R6 */
	push_stack(&stack, 0);					/* R7 */

	return stack;
}

/*
 * brief: Declaration of function which loads context of first thread in runq
 * 		  it is executed only once 
 */
extern void load_context(uint8_t *sp) __attribute__((noreturn));

/*
 * brief: Adds new thread to runq
 *
 * param: thread - pointer to thread struct to be placed in runq
 *
 * retval: Returns 0 on succes, if queue is full returns QUEUE_FULL error code
 */
static int thread_add(struct thread *thread)
{
	int freeSpot;

	/* Error value if doesn't changed it means that runq is full */
	freeSpot = -1;

	/* Looks place for thraed in runq */
	for (int i = 0; i < MAX_THREADS; i++) {
		/* This means that thread in queue is free */
		if (!(runq.thread_count & (1 << i))) {
			freeSpot = i;
			break;
		}
	}

	/* If there is no place in runq return error code */
	if (-1 == freeSpot) {
		return ERROR_QUEUE_FULL;
	}

	/* Assign thread to place in runq */
	runq.thread_count |= (1 << freeSpot);
	runq.queue[freeSpot] = thread;

	return 0;
}

/*
 * biref: Sets pending bit in register to trigger PendSV interrupt */
static void set_pending(uint32_t irq)
{
	SCB->ICSR |= irq;
}

/*
static uint8_t* get_current_sp()
{
	return runq.queue[runq.current]->sp;
}*/

/*
 * brief: Runs scheduler
 *
 * Very simply round robin schedule, time privided for thread depends on
 * configuration of SysTick and for most cases is 1ms
 */
void thread_schedule(void)
{
	int index;
	uint8_t **oldsp;
	uint8_t **newsp;

	index = runq.current;
	oldsp = &runq.queue[runq.current]->sp;
	index++;

	while(1) {
		/* In case it may be krazyc w kolko */
		index %= MAX_THREADS;		
		if ((1 << index) & runq.thread_count) break;
		index++;
	}

	runq.current = index;

	newsp = &runq.queue[runq.current]->sp;

	/*
	 * Sets pending of PendSV exception which will be executed just after this
	 * one if there is no other exception with higher priority
	 */
	set_pending(PENDSV_BIT);

	/*
	 * Store addresses of new thraead stack and previous thread stacks in
	 * registers r0 and r1
	 */
	asm volatile("mov r0, %0" :: "r" (newsp));
	asm volatile("mov r1, %0" :: "r" (oldsp));
}

/*
 * brief: Initializes scheduler and loads first thread to be executed
 */
void start_schedule(void)
{
	/* Disables onterrupts. load_context will enable interrupts */
	__disable_irq();
	/* Check if in queue are threads */
	if (runq.queue[runq.current] == NULL) {
		return;
	}

	/* Sets current thraed to be 0 thread */
	runq.current = 0;
	/*
	 * Loads context of first thread to be executed
	 * sp is placed in r0
	 */
	load_context(runq.queue[runq.current]->sp);
}

/*
 * brief: Initializes thread module
 *
 * retval: Returns 0 on succes.
 */
int thread_module_setup(void)
{
	/* Configure PendSV interrupr */

	runq.current = 0;

	for (int i = 0; i < MAX_THREADS; i++) {
		runq.queue[i] = NULL;
	}
	
	runq.thread_count = 0;

	return 0;
}

/*
 * brief: Creates new thread, it is automatically added to runq
 *
 * param: fn - function to be executed as new thread
 *
 * param:  stackSize - size of stack which will be provided to thread
 */
int thread_create(threadfn fn, uint32_t stackSize)
{
	static int thread_id;
	int error;
	struct thread *thread;

	/* Allocate memmory for thread structure */
	thread = malloc(sizeof(*thread));

	/* If memory wasn't allocated for thread return with error code */
	if (!thread) {
		return ERROR_NO_MEM;
	}

	/* Asign thread id to struct and allocate memory for stack */
	thread->tid = thread_id;
	thread->stack = malloc(stackSize * sizeof(*thread->stack));

	/* 
	 * If memory for stack wasn't allocated free memory allocated for thread
	 * and return with error code
	 */
	if (!thread->stack) {
		free(thread);
		return ERROR_NO_MEM;
	}

	/* Asign faked stack to thread struct */
	thread->sp = thread_fake_stack(fn, thread->stack, stackSize);

	/* 
	 * Add thread to runq.
	 *
	 * If error occures free stack and thread memory and exit with error code
	 * */
	error = thread_add(thread);
	if (error) {
		free(thread->stack);
		free(thread);
		return 1;
	}
	
	/* Increment variable for asigning new thread ids */
	thread_id++;
	return 0;
}

/*
 * brief: Returns id of current thread
 *
 * retval: Returns ID of current thread
 */
int thread_get_current_id(void)
{
	return runq.queue[runq.current]->tid;	
}
