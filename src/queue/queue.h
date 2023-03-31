#pragma once
#include "../process/process.h"
#include <stdbool.h>

typedef struct node
{
    struct node *prev;
    struct node *next;
    Process *data;
} Node;

typedef struct queue
{
    bool cpu_bussy;
    Process *running_process;
    int running_pid;
    double current_start_time;
    double current_running_time;
    int size;
    Node *head;
    Node *tail;
    /* data */
} Queue;

Node *node_init(Process *process);
void node_destroy(Node *node);

Queue *queue_init();
void queue_destroy(Queue *queue);
// void queue_print(Queue *queue);
void queue_append_left(Queue *queue, Process *process);
void queue_append_right(Queue *queue, Process *process);
Process *queue_pop_left(Queue *queue);
Process *queue_pop_right(Queue *queue);
Process *queue_pop_ready(Queue *queue);
Queue *merge_queues(Queue *queue_a, Queue *queue_b);

void enque(Queue *queue, Process *process);
void check_ready_processes(Queue *queue);
void check_running_process(Queue *queue);
void check_waiting_processes(Queue *queue);
void check_enter_processes(Queue *queue, int time_start, Process **processes, int n_processes);

/**
 * Flujo
 * check_enter_processes(queue);
 * check_running_processes(queue);
 * check_waiting_processes(queue);
 * check_ready_processes(queue)
 */

/**
 * Necesito una lista ligada donde pueda encolar procesos
 */