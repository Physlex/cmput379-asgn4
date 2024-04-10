// LINKED
#include "utils.h"
#include "task_monitor.h"

// C
#include <assert.h>

// UNIX
#include <errno.h>
#include <unistd.h>

// LINUX
#include <sys/times.h>

// INCLUDE
#include "task_thread.h"

//==============================================================================
// EXTERNAL

// Holds all stacks in seperate memmory to currently running tasks
PRIVATE task_stack_t waiting_tasks;

// A stack of currently running threads
PRIVATE task_stack_t running_tasks;

// A resource buffer that holds all resources along a bin semaphore for locking
PRIVATE task_monitor_resource_lock_t locked_resources[NRES_TYPES];

//==============================================================================
// PRIVATE

PRIVATE int32_t acquire(task_thread_t *task_thread)
{
    return 0;
}

PRIVATE int32_t release(task_thread_t *task_thread)
{
    return 0;
}

PRIVATE void *task_routine_thread(void *args)
{
    task_thread_t *task_thread = (task_thread_t *)args;

    for (int i = 0; i < task_thread->num_iters; ++i)
    {
        clock_t real_start_ms = times(NULL);

        // Start critical section

        if ( acquire(task_thread) < 0 )
        {
            fprintf(stderr, "Failed to acquire resources\n");
            break;
        }

        if ( release(task_thread) < 0 )
        {
            fprintf(stderr, "Failed to release resources\n");
            break;
        }

        // Start remainder section

        clock_t real_end_ms = times(NULL);

        clock_t real_delta_ms = real_end_ms - real_start_ms;
        uint64_t ticks = sysconf(_SC_CLK_TCK);
        real_delta_ms /= ticks;

        printf
        (
            "task: %s (tid= 0x%lx, iter= %ld, time= %ld)\n",
            &task_thread->task->name[0],
            pthread_self(),
            task_thread->num_iters,
            real_delta_ms
        );
    }

    pthread_exit(NULL);
}

//==============================================================================
// PUBLIC

PUBLIC int32_t lock_resources(parser_resource_t *resources)
{
    assert(resources);
    assert(locked_resources);

    for (int i = 0; i < NRES_TYPES; ++i)
    {
        parser_resource_t *new_rsc = &resources[i];
        task_monitor_resource_lock_t *curr_locked_rsc = &locked_resources[i];

        memcpy
        (
            &curr_locked_rsc->resource,
            new_rsc,
            sizeof(parser_resource_t)
        );

        sem_t *lock = &locked_resources[i].available_resource;

        if ( sem_init(lock, ONE_PROCESS_FL, BIN_LOCK_SET) < 0 )
        {
            char error_msg[STD_MSG_LEN];
            memset(&error_msg[0], 0, STD_MSG_LEN);

            sprintf
            (
                &error_msg[0],
                "%s, a semphore failed to be initialized",
                strerror(errno)
            );
            fprintf(stderr, "%s\n", &error_msg[0]);

            return -1;
        }
        else if ( strlen(&curr_locked_rsc->resource.name[0]) > 0 )
        {
            printf
            (
                "locked resource: %s:%s\n",
                &curr_locked_rsc->resource.name[0],
                &curr_locked_rsc->resource.value[0]
            );
        }
    }

    printf("\n");

    return 0;
}

PUBLIC int32_t push_task
(
    parser_task_t *task,
    const uint64_t num_iters
)
{
    task_thread_t new_task;
    memset(&new_task, 0, sizeof(new_task));
    new_task.task = task;
    new_task.state = WAIT;
    new_task.num_iters = num_iters;

    if ( push_task_thread(&new_task, &waiting_tasks) < 0 )
    {
        fprintf(stderr, "Failed to add task to running tasks stack\b");
        return -1;
    }

    return 0;
}

PUBLIC int32_t dispatch_task_thread(void)
{
    task_thread_t task;

    if ( pop_task_thread(&task, &waiting_tasks) < 0 )
    {
        fprintf(stderr, "Failed to pop task from waiting stack\n");
        return -1;
    }

    if ( push_task_thread(&task, &running_tasks) < 0 )
    {
        fprintf(stderr, "Failed to push task into running stack\n");
        return -1;
    }

    const size_t top = running_tasks.top - 1;
    const int32_t error = task_thread_create
    (
        &running_tasks.task_thread_buffer[top],
        task_routine_thread
    );

    if ( error < 0 )
    {
        fprintf(stderr, "Failed to create running thread\n");
        return -1;
    }
    else
    {
        printf("Dispatched task %s\n\n", &task.task->name[0]);
    }

    return 0;
}

PUBLIC int32_t wall_tasks()
{    
    for (int i = 0; i < NTASKS; ++i)
    {
        task_thread_t running_task_ptr;

        if ( pop_task_thread(&running_task_ptr, &running_tasks) < 0 )
        {
            fprintf(stderr, "Failed to remove waiting task thread\n");
            return -1;
        }

        if ( push_task_thread(&running_task_ptr, &waiting_tasks) )
        {
            fprintf(stderr, "Failed to push task thread into waiting stack\n");
            return -1;
        }

        const size_t top = waiting_tasks.top - 1;
        if ( task_thread_join(&waiting_tasks.task_thread_buffer[top]) < 0 )
        {
            fprintf(stderr, "Failed to join with task thread\n");
            return -1;
        }
        else
        {
            task_thread_t *task_buffer = &waiting_tasks.task_thread_buffer[0];
            parser_task_t *ll_task_ptr = task_buffer[top].task;

            printf
            (
                "Successfully joined with task thread %s\n",
                &ll_task_ptr->name[0]
            );
        }
    }

    return 0;
}
