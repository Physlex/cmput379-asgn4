// LINKED
#include "utils.h"
#include "task_monitor.h"

// C
#include <assert.h>

// UNIX
#include <errno.h>
#include <unistd.h>
#include <poll.h>

// LINUX
#include <sys/times.h>

// INCLUDE
#include "task_thread.h"

//==============================================================================
// INTERNAL

/* Typedef -------------------------------------------------------------------*/

// Holds all stacks in seperate memmory to currently running tasks
PRIVATE task_stack_t waiting_tasks;

// A stack of currently running threads
PRIVATE task_stack_t running_tasks;

// A resource buffer that holds all resources
PRIVATE task_monitor_resource_t locked_resources[NRES_TYPES];

PRIVATE pthread_t monitor_thread;
PRIVATE sem_t resource_lock;
PRIVATE sem_t monitor_lock;

PRIVATE size_t task_monitor_delay_ms = 0;

//==============================================================================
// PRIVATE

PRIVATE int32_t state_to_ascii
(
    char *state_buffer,
    const task_thread_state_t *state
)
{
    switch (*state)
    {
        case (WAIT):
        {
            strncpy(state_buffer, "WAIT", strlen("WAIT") + 1);
            break;
        }
        case (RUN):
        {
            strncpy(state_buffer, "RUN", strlen("RUN") + 1);
            break;
        }
        case (IDLE):
        {
            strncpy(state_buffer, "IDLE", strlen("IDLE") + 1);
            break;
        }
        default:
        {
            fprintf(stderr, "Catastophic switch failure! Invalid state\n");
            return -1;
        }
    }

    return 0;
}

PRIVATE int32_t wait(const uint32_t idle_time_ms)
{
    poll(NULL, 0, idle_time_ms); // Total hack, but F it. Honestly.

    return 0;
}

PRIVATE int32_t acquire(task_thread_t *my_task)
{
    task_monitor_resource_t desired_rcs[NRES_TYPES];
    memset(&desired_rcs[0], 0, NRES_TYPES);

    for (int i = 0; i < NRES_TYPES; ++i)
    {
        parser_resource_t *my_rsc = &my_task->task.resources[i];

        const char *rcs_name = &my_rsc->name[0];
        strncpy(&desired_rcs[i].name[0], rcs_name, strlen(rcs_name));

        desired_rcs[i].amnt = atoi(&my_rsc->value[0]);
    }

    // Find resource that is occupied
    for (int j = 0; j < NRES_TYPES; ++j)
    {
        task_monitor_resource_t *desired_ptr = &desired_rcs[j];
        task_monitor_resource_t *locked_ptr = &locked_resources[j];
        const size_t len = strlen(&locked_ptr->name[0]);
        if ( strncmp(&desired_ptr->name[0], &locked_ptr->name[0], len) == 0)
        {
            if (locked_ptr->amnt < desired_ptr->amnt)
            {
                return 1; // Try again later, when rcs might be free
            }
        }
    }

    // If no resources occupied, take them
    for (int j = 0; j < NRES_TYPES; ++j)
    {
        task_monitor_resource_t *desired_ptr = &desired_rcs[j];
        task_monitor_resource_t *locked_ptr = &locked_resources[j];
        const size_t len = strlen(&locked_ptr->name[0]);
        if ( strncmp(&desired_ptr->name[0], &locked_ptr->name[0], len) == 0)
        {
            // Reduce resources by desired
            locked_ptr->amnt -= desired_ptr->amnt;
        }
    }

    return 0;
}

PRIVATE int32_t release(task_thread_t *my_task)
{   
    task_monitor_resource_t desired_rcs[NRES_TYPES];
    memset(&desired_rcs[0], 0, NRES_TYPES);

    for (int i = 0; i < NRES_TYPES; ++i)
    {
        parser_resource_t *my_rsc = &my_task->task.resources[i];

        const char *rcs_name = &my_rsc->name[0];
        strncpy(&desired_rcs[i].name[0], rcs_name, strlen(rcs_name));

        desired_rcs[i].amnt = atoi(&my_rsc->value[0]);
    }

    for (int j = 0; j < NRES_TYPES; ++j)
    {
        task_monitor_resource_t *desired_ptr = &desired_rcs[j];
        task_monitor_resource_t *locked_ptr = &locked_resources[j];
        const size_t len = strlen(&locked_ptr->name[0]);
        if ( strncmp(&desired_ptr->name[0], &locked_ptr->name[0], len) == 0)
        {
            locked_ptr->amnt += desired_ptr->amnt;
        }
    }

    return 0;
}

PRIVATE void *monitor_tasks(void *args)
{
    while (1)
    {
        wait(task_monitor_delay_ms);

        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

        // WAIT SEMAPHORE
    
        if ( sem_wait(&monitor_lock) < 0 )
        {
            fprintf(stderr, "Failed to lock MONITOR semaphore\n");
            pthread_exit(NULL);
        }

        task_thread_t *tasks = &running_tasks.task_thread_buffer[0];
        printf("\nmonitor: ");

        printf("[WAIT] ");
        for (int i = 0; i < running_tasks.top; ++i)
        {
            if (tasks[i].state == WAIT)
            {
                printf("%s ", &tasks[i].task.name[0]);
            }
        }
        printf("\n");

        printf("\t [RUN] ");
        for (int i = 0; i < running_tasks.top; ++i)
        {
            if (tasks[i].state == RUN)
            {
                printf("%s ", &tasks[i].task.name[0]);
            }
        }
        printf("\n");

        printf("\t [IDLE] ");
        for (int i = 0; i < running_tasks.top; ++i)
        {
            if (tasks[i].state == IDLE)
            {
                printf("%s ", &tasks[i].task.name[0]);
            }
        }
        printf("\n\n");

        // SIGNAL SEMAPHORE
        if ( sem_post(&monitor_lock) < 0 )
        {
            fprintf(stderr, "Failed to post MONITOR semaphore\n");
            pthread_exit(NULL);
        }

        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    }

    pthread_exit(NULL);
}

PRIVATE void *task_routine_thread(void *args)
{
    assert(args);

    task_thread_t *task_thread = (task_thread_t *)args;
    const long ticks = sysconf(_SC_CLK_TCK);

    for (int i = 0; i < task_thread->num_iters; ++i)
    {
        clock_t real_start_ms = times(NULL);
        clock_t time_waiting_start_tk = times(NULL);

        // WAIT SEMAPHORE

        if ( sem_wait(&resource_lock) < 0 )
        {
            fprintf(stderr, "Failed to lock WAIT semaphore\n");
            pthread_exit(NULL);
        }

        if ( sem_wait(&monitor_lock) < 0 )
        {
            fprintf(stderr, "Failed to lock WAIT semaphore\n");
            pthread_exit(NULL);
        }

        clock_t time_waiting_end_tk = times(NULL);
        long time_waiting_delta = time_waiting_end_tk - time_waiting_start_tk;
        long time_waiting_ms = time_waiting_delta / ticks * 1000;
        long time_waiting_ms_rem = time_waiting_delta % ticks * 10;

        task_thread->wait_time_ms += time_waiting_ms + time_waiting_ms_rem;
        task_thread->state = WAIT;

        int retry = 1;
        while (retry == 1)
        {
            if ( (retry = acquire(task_thread)) < 0 )
            {
                fprintf(stderr, "Failed to acquire resources\n");
                break;
            }

            wait(10);
        }

        // SIGNAL SEMAPHORE

        if ( sem_post(&monitor_lock) < 0 )
        {
            fprintf(stderr, "Failed to lock WAIT semaphore\n");
            pthread_exit(NULL);
        }

        if ( sem_post(&resource_lock) < 0 )
        {
            fprintf(stderr, "Failed to post WAIT semaphore\n");
            pthread_exit(NULL);
        }

        task_thread->state = RUN;
        wait(task_thread->task.busy_time);

        // WAIT SEMAPHORE

        time_waiting_start_tk = times(NULL);

        if ( sem_wait(&resource_lock) < 0 )
        {
            fprintf(stderr, "Failed to lock IDLE semaphore\n");
            pthread_exit(NULL);
        }

        if ( sem_wait(&monitor_lock) < 0 )
        {
            fprintf(stderr, "Failed to lock WAIT semaphore\n");
            pthread_exit(NULL);
        }

        time_waiting_end_tk = times(NULL);
        time_waiting_delta = time_waiting_end_tk - time_waiting_start_tk;
        time_waiting_ms = time_waiting_delta / ticks * 1000;
        time_waiting_ms_rem = time_waiting_delta % ticks * 10;

        task_thread->wait_time_ms += time_waiting_ms + time_waiting_ms_rem;

        if ( (retry = release(task_thread)) < 0 )
        {
            fprintf(stderr, "Failed to release resources\n");
            break;
        }

        task_thread->state = IDLE;
        wait(task_thread->task.idle_time);

        // SIGNAL SEMAPHORE

        if ( sem_post(&monitor_lock) < 0 )
        {
            fprintf(stderr, "Failed to lock WAIT semaphore\n");
            pthread_exit(NULL);
        }

        if ( sem_post(&resource_lock) < 0 )
        {
            fprintf(stderr, "Failed to post IDLE semaphore\n");
            pthread_exit(NULL);
        }

        clock_t real_end_ms = times(NULL);

        long real_delta_base_ms = (real_end_ms - real_start_ms) / ticks * 1000;
        long real_delta_rem_ms = ((real_end_ms - real_start_ms) % ticks) * 10;

        printf
        (
            "task: %s (tid= 0x%lx, iter= %d, time= %ldms)\n",
            &task_thread->task.name[0],
            pthread_self(),
            i + 1,
            real_delta_base_ms + real_delta_rem_ms
        );

        wait(6); // Allow OS to print line
    }

    pthread_exit(NULL);
}

//==============================================================================
// PUBLIC

PUBLIC int32_t lock_resources(parser_resource_t *resources)
{
    if ( sem_init(&monitor_lock, ONE_PROCESS_FL, BIN_LOCK_SET) < 0 )
    {
        fprintf(stderr, "Failed to create resource lock\n");
        return -1;
    }

    if ( sem_init(&resource_lock, ONE_PROCESS_FL, BIN_LOCK_SET) < 0 )
    {
        fprintf(stderr, "Failed to create resource lock\n");
        return -1;
    }

    assert(resources);
    assert(locked_resources);

    for (int i = 0; i < NRES_TYPES; ++i)
    {
        parser_resource_t *new_rsc = &resources[i];
        task_monitor_resource_t *curr_locked_rsc = &locked_resources[i];

        strncpy
        (
            &curr_locked_rsc->name[0],
            &new_rsc->name[0],
            strlen(&new_rsc->name[0])
        );

        curr_locked_rsc->amnt = atoi(&new_rsc->value[0]);
        if (curr_locked_rsc->amnt < 0)
        {
            fprintf(
                stderr,
                "Error: %s must have positive integral value\n",
                &curr_locked_rsc->name[0]
            );

            return -1;
        }
    }

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

    memcpy(&new_task.task, task, sizeof(parser_task_t));
    new_task.state = WAIT;
    new_task.num_iters = num_iters;

    if (strlen(&new_task.task.name[0]) == 0)
    {
        return 0;
    }

    if ( push_task_thread(&new_task, &waiting_tasks) < 0 )
    {
        fprintf(stderr, "Failed to add task to running tasks stack\b");
        return -1;
    }

    return 0;
}

PUBLIC int32_t dispatch_monitor_thread(const size_t delay_ms)
{
    task_monitor_delay_ms = delay_ms;

    const int error = pthread_create
    (
        &monitor_thread,
        NULL,
        monitor_tasks,
        NULL
    );

    if ( error < 0 )
    {
        fprintf(stderr, "Failed to create monitor thread\n");
        return -1;
    }

    return 0;
}

PUBLIC int32_t dispatch_task_thread(void)
{
    task_thread_t task;

    if (waiting_tasks.top == 0)
    {
        return 0;
    }

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
    const int32_t error = pthread_create
    (
        &running_tasks.task_thread_buffer[top].task_thread,
        NULL,
        task_routine_thread,
        &running_tasks.task_thread_buffer[top]
    );

    if ( error < 0 )
    {
        fprintf(stderr, "Failed to create running thread\n");
        return -1;
    }

    return 0;
}

PUBLIC int32_t wall_tasks()
{    
    for (int i = 0; i < NTASKS; ++i)
    {
        const size_t top = running_tasks.top - 1;

        if ( task_thread_join(&running_tasks.task_thread_buffer[top]) < 0 )
        {
            fprintf(stderr, "Failed to join with task thread\n");
            return -1;
        }

        if (top == 0)
        {
            break; // No more threads
        }
    }

    if ( pthread_cancel(monitor_thread) < 0 )
    {
        fprintf(stderr, "Monitor thread failed to join\n");
        return  -1;
    }

    return 0;
}

PUBLIC int32_t display_task_information(parser_resource_t *resources_config)
{
    printf("System Resources:\n");
    for (int i = 0; i < NRES_TYPES; ++i)
    {
        const int32_t max_resources = atoi(&resources_config[i].value[0]);
        const int32_t returned_amnt =  max_resources - locked_resources[i].amnt;

        if (strlen(&locked_resources[i].name[0]) == 0)
        {
            break; // Empty resources from here on
        }
        else
        {
            printf
            (
                "%s: (maxAvail=\t%s, held=\t%d)\n",
                &resources_config[i].name[0],
                &resources_config[i].value[0],
                returned_amnt
            );
        }
    }

    printf("\nSystem Tasks:\n");
    for (int i = 0; i < NTASKS; ++i)
    {
        const int32_t max_resources = atoi(&resources_config[i].value[0]);
        const int32_t returned_amnt =  max_resources - locked_resources[i].amnt;

        task_thread_t dying_task;
        memset(&dying_task, 0, sizeof(task_thread_t));

        if (running_tasks.top == 0)
        {
            break; // Empty now
        }

        if ( pop_task_thread(&dying_task, &running_tasks) < 0 )
        {
            fprintf(stderr, "Task %s failed death\n", &dying_task.task.name[0]);
            return -1;
        }

        char state[TOKEN_LEN];
        memset(&state[0], 0, TOKEN_LEN);

        if ( state_to_ascii(&state[0], &dying_task.state) < 0 )
        {
            fprintf
            (
                stderr,
                "Failed to recognize task %s state\n",
                &dying_task.task.name[0]
            );

            return -1;
        }

        printf
        (
            "[%d] %s (%s, runTime= %d msec, idleTime= %d msec):\n",
            i, &dying_task.task.name[0], &state[0],
            dying_task.task.busy_time,
            dying_task.task.idle_time
        );

        printf("\t(tid= 0x%lx)\n", dying_task.task_thread);

        for (int i = 0; i < NRES_TYPES; ++i)
        {
            parser_resource_t *desired_rsc = &dying_task.task.resources[i];

            if (strlen(&desired_rsc->name[0]) == 0)
            {
                break; // No more desired resources from here
            }

            printf
            (
                "\t%s: (needed=\t%s, held=\t%d)\n",
                &desired_rsc->name[0],
                &desired_rsc->value[0],
                returned_amnt // Used a blackboard model, so this is arb.
            );
        }

        printf
        (
            "\t(RUN: %ld times, WAIT: %ld msec)\n",
            dying_task.num_iters,
            dying_task.wait_time_ms
        );
    }

    return 0;
}
