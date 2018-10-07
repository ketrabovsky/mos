#ifndef __THREAD_H__
#define __THREAD_H__

#include <stdint.h>

struct thread;

typedef void(*threadfn)(void);

/*
 * 	@brief: Sets up thread module
 */
int thread_module_setup(void);

/*
 * @brief: This function start first thread in queue, based on created threads
 */
void start_schedule(void);

/*
 * @brief: Runs scheduler
 *
 * It is executed inside SysTick_Handler
 */
void thread_schedule(void);

/*
 * 	@brief: Creates new thread
 *
 *  @param: fn
 *  	- thread function which will be executed
 *
 *	@param: stacksize
 *		- stack size in bytes
 *
 * 	@retval: 0 on succes or error code
 */
int thread_create(threadfn fn, uint32_t stackSize);

/*
 * 	@brief: Retuns id of current thread
 */
int thread_get_current_id(void);

#endif /* __THREAD_H__ */
