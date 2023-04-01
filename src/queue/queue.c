#include "queue.h"

Node *_node_init(Process *process)
{
    Node *node = malloc(sizeof(Node));
    node->data = process;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void _node_destroy(Node *node)
{
    free(node);
}

Queue *queue_init()
{
    Queue *queue = malloc(sizeof(Queue));
    queue->running_process = NULL;
    queue->current_start_time = -1;
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

void queue_destroy(Queue *queue)
{

    Node *last;
    while (queue->tail)
    {
        last = queue->tail->next;
        _node_destroy(queue->tail);
        queue->tail = last;
    }
    free(queue);
}

void queue_append_left(Queue *queue, Process *process)
{
    Node *node = _node_init(process);
    if (queue->size == 0)
    {
        queue->tail = node;
        queue->head = node;
    }
    else
    {
        Node *first = queue->tail;
        queue->tail = node;
        node->next = first;
        first->prev = node;
    }
    queue->size += 1;
}

void queue_append_right(Queue *queue, Process *process)
{
    Node *node = _node_init(process);
    if (queue->size == 0)
    {
        queue->tail = node;
        queue->head = node;
    }
    else
    {
        Node *last = queue->head;
        queue->head = node;
        node->prev = last;
        last->next = node;
    }
    queue->size += 1;
}

Process *queue_pop_left(Queue *queue)
{
    if (queue->size == 0)
    {
        return NULL;
    }

    Node *node = queue->tail;
    Process *process = node->data;
    Node *first = node->next;
    if (first)
    {
        first->prev = NULL;
    }
    queue->tail = first;
    _node_destroy(node);
    queue->size -= 1;
    return process;
}

Process *queue_pop_right(Queue *queue)
{
    if (queue->size == 0)
    {
        return NULL;
    }

    Node *node = queue->head;
    Process *process = node->data;
    Node *last = node->prev;
    if (last)
    {
        last->next = NULL;
    }
    queue->head = last;
    _node_destroy(node);
    queue->size -= 1;
    return process;
}

Process *queue_pop_ready(Queue *queue)
{

    if (queue->size == 0)
    {
        return NULL;
    }

    Node *node = queue->tail;
    Process *process;
    for (int i = 0; i < queue->size; i++)
    {
        process = node->data;
        if (process->state == ready)
        {
            // Update the queue state
            if (queue->tail == node)
            {
                queue->tail = node->next;
            }
            if (queue->head == node)
            {
                queue->head = node->prev;
            }
            if (node->next)
            {
                node->next->prev = node->prev;
            }
            if (node->prev)
            {
                node->prev->next = node->next;
            }
            queue->size -= 1;
            break;
        }
        node = node->next;
    }

    // if (queue->size == 1)
    // {
    //     printf("  ");
    // }
    // if (queue->size == 2)
    // {
    //     printf("  ");
    // }
    return process;
}

/**
 * Merge two queues and delete the second queue
 * returns: queue_a updated with elements of queue_b
 */
Queue *_merge_queues(Queue *queue_a, Queue *queue_b)
{
    // Extend empty list does not change the state
    if (queue_b->size == 0)
    {
        return queue_a;
    }

    // If the queue_a is empty replace head and tail is enough
    if (queue_a->size == 0)
    {
        queue_a->tail = queue_b->tail;
        queue_a->head = queue_b->head;
        return queue_a;
    }

    // In other case I have nodes in queue_a and queue_b
    // Need to update the node linikns
    queue_a->head->next = queue_b->tail;
    queue_b->tail->prev = queue_a->head;
    // and the queue_a head
    queue_a->head = queue_b->head;
    free(queue_b);
    return queue_a;
}

int queue_get_current_running_time(Queue *queue)
{
    double current_running_time = get_time_interval(queue->current_start_time, get_timestamp());
    return current_running_time;
}

Process *queue_cpu_run(Queue *queue, Process *process)
{
    printf("LOADING TO CPU | %s\n", process->name);
    process_set_state(process, running);
    queue->running_process = process;
    queue->current_start_time = get_timestamp();
    return process;
}

void queue_send_process_to_wait(Queue *queue, Process *process)
{
    printf("STOPPING | %s \n", process->name);

    process_set_state(process, waiting);
}

void check_ready_processes(Queue *queue)
{
    if (!queue->running_process)
    {
        Process *process = queue_pop_ready(queue);
        if (process != NULL)
        {
            queue_cpu_run(queue, process);
        }
    }
}

void check_running_process(Queue *queue)
{
    int wstatus;
    if (queue->running_process)
    {
        // If the process send us the exit signal we listen that
        int child_pid = waitpid(queue->running_process->pid, &wstatus, WNOHANG);
        if (child_pid == queue->running_process->pid)
        {
            process_set_state(queue->running_process, finished);
            printf("CHILD EXITED | %s, pid=%d\n", queue->running_process->name, queue->running_process->pid);
        }

        // If the process is running more time than necesary we send it to wait
        if (queue_get_current_running_time(queue) >= queue->running_process->cpu_burst)
        {
            queue_send_process_to_wait(queue, queue->running_process);
            queue_append_left(queue, queue->running_process);
            queue->running_process = NULL;
        }
    }
}

void check_waiting_processes(Queue *queue)
{
    Node *node = queue->tail;

    while (node)
    {
        Process *process = node->data;

        if (process->state == waiting && process_get_wait_time(process) >= process->io_wait)
        {
            process_set_state(process, ready);
        }
        node = node->next;
    }
}

void check_enter_processes(Queue *queue, int time_start, Process **processes, int n_processes)
{
    int current_time = round_time(get_time_interval(time_start, get_timestamp()));
    for (int i = 0; i < n_processes; i++)
    {
        Process *process = processes[i];
        if (current_time == process->start_time && process->state == none)
        {
            printf("STARTING | %s | time: %d\n ", process->name, current_time);
            process_set_state(process, ready);

            printf("ENQUEUEING | %s\n", process->name);
            queue_append_left(queue, process);
        };
    }
}