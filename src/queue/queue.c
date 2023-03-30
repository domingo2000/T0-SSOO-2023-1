#include "queue.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

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

void enque(Queue *queue, Process *process)
{
}

void check_ready_processes(Queue *queue)
{
}

void check_waiting_processes(Queue *queue)
{
}

void check_enter_processes(Queue *queue, int time_start, Process **processes)
{
    double time_now = clock();
    int current_time = (int)((time_start - time_now) / CLOCKS_PER_SEC);
    int process_counter = 0;
    while (current_time < 5)
    {
        Process *process = processes[process_counter];
        if (current_time == process->start_time && process->state == none)
        {
            printf("Starting process %s at time %d\n ", process->name, current_time);
            process->state = ready;
            process_counter += 1;
        };

        time_now = clock();
        current_time = (int)((time_now - time_start) / CLOCKS_PER_SEC);
    }
}