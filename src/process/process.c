#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include "process.h"
#include <stdbool.h>

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
	process->created = false;
	process->start_time = start_time;
	process->wait_time = -1;
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

void process_set_state(Process *process, enum state state)
{
	switch (state)
	{
	case waiting:
		process->wait_time = 0;
		kill(process->pid, SIGSTOP);
		break;
	case running:
		process->wait_time = 0;
		if (!process->created)
		{
			printf("FORKING | %s | parent pid: %d\n", process->name, getpid());
			int pid = fork();
			process->pid = pid;
			if (pid != 0)
			{
				process->pid = pid;
				process->created = true;
			}
			else
			{
				sleep(10);
				exit(0);
			}
		}
		else
		{
			kill(process->pid, SIGCONT);
		}
		process->wait_time = 0;
		break;
	default:
		break;
	}

	process->state = state;
}

double process_get_wait_time(Process *process)
{
	return get_time_interval(get_timestamp(), process->wait_time);
}
