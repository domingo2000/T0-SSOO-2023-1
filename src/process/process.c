#include "process.h"

Process *process_init(
	int _id,
	char *name,
	enum state state,
	int start_time,
	int cpu_burst,
	int io_wait,
	char *path,
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
	process->cpu_burst = cpu_burst;
	process->io_wait = io_wait;
	process->path = path;
	process->n_args = n_args;
	process->args = args;
	process->stat_times_cpu = 0;
	process->stat_total_wait_time = 0;
	return process;
}

void process_destroy(Process *process)
{
	free(process->args);
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

void process_print_stats(Process *process)
{
	printf("------------\n\
NAME: %s\n\
PID: %d\n\
TIMES_CPU: %d\n\
TURNAROUND_TIME:-\n\
RESPONSE_TIME:-\n\
WAITING_TIME:%lf\n\
EXIT_CODE:%d\n\
",
		   process->name,
		   process->pid,
		   process->stat_times_cpu,
		   process->stat_total_wait_time,
		   process->stat_exit_status);
}

bool process_is_created(Process *process)
{
	if (process->pid == -1)
	{
		return false;
	}
	return true;
}

void process_set_state(Process *process, enum state state)
{
	switch (state)
	{
	case waiting:
		process->wait_start_time = get_timestamp();
		kill(process->pid, SIGSTOP);
		break;
	case ready:
		process->stat_total_wait_time += process_get_delta_wait_time(process);
		break;
	case running:
		process->stat_times_cpu += 1;
		if (process_is_created(process))
		{
			kill(process->pid, SIGCONT);
		}
		else
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
				execv(process->path, process->args);
			}
		}
		break;
	default:
		break;
	}

	process->state = state;
}

double process_get_delta_wait_time(Process *process)
{
	return get_time_interval(get_timestamp(), process->wait_start_time);
}
