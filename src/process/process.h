#pragma once

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