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
	bool created;
	int start_time;
	double wait_time;
	int cpu_burst;
	int io_wait;
	int n_args;
	char **args;
} Process;

Process *process_init(
	int _id,
	char *name,
	enum state state,
	int start_time,
	int cpu_burst,
	int io_wait,
	int n_args,
	char **args);

void process_destroy(Process *process);

void process_print(Process *process);

void process_set_state(Process *process, enum state state);

double process_get_wait_time(Process *process);
