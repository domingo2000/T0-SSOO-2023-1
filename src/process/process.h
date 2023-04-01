#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include "../time/_time.h"

enum state
{
	none,
	ready,
	running,
	waiting,
	finished
};

typedef struct Process
{
	int _id;
	char *name;
	int pid;
	enum state state;
	int start_time;
	int cpu_burst;
	int io_wait;
	char *path;
	int n_args;
	char **args;
	int stat_times_cpu;
	double stat_total_wait_time;
	double stat_total_ready_time;
	int stat_exit_status;
	double wait_initial_time;
	double ready_initial_time;
	double finish_time;
	double enter_time;
	double attention_time;
} Process;

Process *process_init(
	int _id,
	char *name,
	enum state state,
	int start_time,
	int cpu_burst,
	int io_wait,
	char *path,
	int n_args,
	char **args);

void process_destroy(Process *process);

void process_print(Process *process);

void process_print_stats(Process *process);

void process_set_state(Process *process, enum state state);

double process_get_delta_wait_time(Process *process);

double process_get_delta_ready_time(Process *process);

double process_get_turnaround_time(Process *process);

double process_get_response_time(Process *process);