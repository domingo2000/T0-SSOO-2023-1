#include "queue.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

Node *node_init(Process *process)
{
    Node *node = malloc(sizeof(Node));
    node->data = process;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void node_destroy(Node *node)
{
    free(node);
}

Queue *queue_init()
{
    Queue *queue = malloc(sizeof(Queue));
    queue->running_process = NULL;
    queue->current_running_time = -1;
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
        node_destroy(queue->tail);
        queue->tail = last;
    }
    free(queue);
}

void queue_append_left(Queue *queue, Process *process)
{
    Node *node = node_init(process);
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
    Node *node = node_init(process);
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
    node_destroy(node);
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
    node_destroy(node);
    queue->size -= 1;
    return process;
}

Process *queue_pop_ready(Queue *queue)
{
    Queue *stack = queue_init();
    Process *process = queue_pop_right(queue);

    while (process)
    {
        if (process->state == ready)
        {
            break;
        }
        else
        {
            queue_append_right(stack, process);
            process = queue_pop_right(queue);
        }
    }

    queue = merge_queues(queue, stack);
    return process;
}

/**
 * Merge two queues and delete the second queue
 * returns: queue_a updated with elements of queue_b
 */
Queue *merge_queues(Queue *queue_a, Queue *queue_b)
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
    queue->current_running_time = (clock() / CLOCKS_PER_SEC) - queue->current_start_time;
    int time_passed = (int)queue->current_running_time;
    return time_passed;
}

void enque(Queue *queue, Process *process)
{
    printf("ENQUEUEING | %s\n", process->name);
    queue_append_left(queue, process);
}

int queue_create_process(Queue *queue, Process *process)
{
    printf("FORKING | %s | parent pid: %d\n", process->name, getpid());
    int pid = fork();
    if (pid != 0)
    {
        process->pid = pid;
        process->created = true;
        return pid;
    }
    else
    {
        // TO DO: child process code
        sleep(60);
        exit(0);
        return 0;
    }
}

Process *queue_cpu_run(Queue *queue, Process *process)
{
    printf("LOADING TO CPU | %s\n", process->name);
    process->state = running;
    queue->running_process = process;
    queue->current_start_time = clock() / CLOCKS_PER_SEC;
    queue->current_running_time = 0;

    if (!process->created)
    {
        int pid = queue_create_process(queue, process);
        process->pid = pid;
    }
    else
    {
        // Send continue signal
    }
    return process;
}

void queue_send_process_to_wait(Queue *queue, Process *process)
{
    printf("STOPPING | %s \n", process->name);
    process->state = waiting;
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
        waitpid(queue->running_process->pid, &wstatus, WNOHANG);

        if (queue_get_current_running_time(queue) >= queue->running_process->cpu_burst)
        {
            queue_send_process_to_wait(queue, queue->running_process);
            queue->running_process = NULL;
        }
    }
}

void check_waiting_processes(Queue *queue)
{
}

void check_enter_processes(Queue *queue, int time_start, Process **processes, int n_processes)
{
    double time_now = clock();
    int current_time = (int)((time_start - time_now) / CLOCKS_PER_SEC);
    for (int i = 0; i < n_processes; i++)
    {
        Process *process = processes[i];
        if (current_time == process->start_time && process->state == none)
        {
            printf("STARTING | %s | time: %d\n ", process->name, current_time);
            process->state = ready;
            enque(queue, process);
        };

        time_now = clock();
        current_time = (int)((time_now - time_start) / CLOCKS_PER_SEC);
    }
}