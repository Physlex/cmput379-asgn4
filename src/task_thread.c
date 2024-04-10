// LINKED
#include "task_thread.h"
#include "utils.h"

// UNIX
#include <errno.h>
#include <unistd.h>

// INCLUDE
#include "task_monitor.h"
#include "parser.h"

//==============================================================================
// PUBLIC

PUBLIC int32_t push_task_thread
(
    task_thread_t *new_task_thread,
    task_stack_t *task_stack
)
{
    if (task_stack->top >= NTASKS)
    {
        fprintf(stderr, "Cannot add new task to stack. Stack Overflow.\n");
        return -1;
    }

    const size_t top = task_stack->top;
    task_thread_t *new_task_ptr = &task_stack->task_thread_buffer[top];
    memcpy
    (
        new_task_ptr,
        new_task_thread,
        sizeof(task_thread_t)
    );

    (++task_stack->top);

    return 0;
}

PUBLIC int32_t pop_task_thread
(
    task_thread_t *return_task,
    task_stack_t *task_stack
)
{
    if (task_stack->top < 0)
    {
        fprintf(stderr, "Cannot remove task from stack. Stack Underflow.\n");
        return -1;
    }

    const uint8_t top = task_stack->top - 1;
    task_thread_t *old_task = &task_stack->task_thread_buffer[top];
    memcpy(return_task, old_task, sizeof(task_thread_t));

    memset(old_task, 0, sizeof(task_thread_t));
    (--task_stack->top);

    return 0;
}

PUBLIC int32_t initialize_task_thread
(
    task_thread_t *new_task_thread,
    task_stack_t *held_tasks
)
{
    return push_task_thread(new_task_thread, held_tasks);
}

PUBLIC int32_t task_thread_create
(
    task_thread_t *waiting_task,
    void *(*routine)(void*)
)
{
    if ( pthread_create(&waiting_task->task_thread, NULL, routine, waiting_task) < 0 )
    {
        fprintf
        (
            stderr,
            "%s: Failed to create pthread for task\n",
            strerror(errno)
        );
        return -1;
    }

    return 0;
}

PUBLIC int32_t task_thread_join(task_thread_t *running_task)
{
    if ( pthread_join(running_task->task_thread, NULL) < 0 )
    {
        fprintf(stderr, "%s: Failed to join pthread\n", strerror(errno));
        return -1;
    }

    return 0;
}
