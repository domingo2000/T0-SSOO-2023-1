#include <stdlib.h>
#include <stdio.h>
#include "process.h"

Process *process_init(
	int _id,
	char *name,
	enum state state,
	int start_time,
	int cpu_burst,
	int io_wait,
	int n_args,
	char **args)
{
	Process *process = malloc(sizeof(Process));
	process->_id = _id;
	process->name = name;
	process->pid = -1;
	process->state = state;
	process->start_time = start_time;
	process->cpu_burst = cpu_burst;
	process->io_wait = io_wait;
	process->n_args = n_args;
	process->args = args;
	return process;
}

void process_destroy(Process *process)
{
	free(process);
}

void process_print(Process *process)
{
	char state;
	switch (process->state)
	{
	case none:
		state = '-';
		break;
	case ready:
		state = 'R';
		break;
	case running:
		state = 'E';
		break;
	case waiting:
		state = 'S';
		break;
	case finished:
		state = 'F';
		break;
	default:
		state = ' ';
		break;
	}

	printf("Process(_id:%d,name:%s,pid:%d,state:%c,start_time:%d, cpu_burst:%d,io_wait:%d)\n",
		   process->_id,
		   process->name,
		   process->pid,
		   state,
		   process->start_time,
		   process->cpu_burst,
		   process->io_wait);
}
