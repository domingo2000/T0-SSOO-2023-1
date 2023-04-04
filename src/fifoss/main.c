#include <stdio.h>	// FILE, fopen, fclose, etc.
#include <stdlib.h> // malloc, calloc, free, etc
#include <wait.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#include "../process/process.h"
#include "../queue/queue.h"
#include "../file_manager/manager.h"

#define SIZE_QUEUE 2048

int cmpfunc(const void *a, const void *b)
{
	Process *p1 = *(Process **)a;
	Process *p2 = *(Process **)b;

	if (p1->start_time < p2->start_time)
	{
		return -1;
	}
	else if (p1->start_time == p2->start_time)
	{
		if (p1->_id < p2->_id)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 1;
	}
}

bool are_all_processes_finished(Process **processes, int n_processes)
{
	Process *process;
	for (int i = 0; i < n_processes; i++)
	{
		process = processes[i];
		if (process->state != finished)
		{
			return false;
		}
	}

	return true;
}

Process **load_processes(InputFile *input_file, int n_processes)
{
	Process **processes = malloc(sizeof(Process *) * n_processes);

	for (int i = 0; i < input_file->len; ++i)
	{

		char *name = input_file->lines[i][0];
		enum state state = none;
		int start_time = atoi(input_file->lines[i][1]);
		int cpu_burst = atoi(input_file->lines[i][2]);
		int io_wait = atoi(input_file->lines[i][3]);
		char *path = input_file->lines[i][4];
		int n_args = atoi(input_file->lines[i][5]);
		char **args = malloc(sizeof(char *) * (n_args + 2));

		args[0] = path; // Name of the file is added to arg[0] by convention
		for (int j = 0; j < n_args; j++)
		{
			args[j + 1] = input_file->lines[i][j + 6];
		}
		args[n_args + 2 - 1] = (char *)NULL; // Null pointer added as final arg to comply execv function

		Process *process = process_init(i, name, state, start_time, cpu_burst, io_wait, path, n_args, args);
		processes[i] = process;
	}

	return processes;
}

int main(int argc, char const *argv[])
{
	/*Lectura del input*/
	char *file_name = (char *)argv[1];
	InputFile *input_file = read_file(file_name);
	FILE *output_file = fopen(argv[2], "w");

	/*Mostramos el archivo de input en consola*/
	printf("Nombre archivo: %s\n", file_name);
	printf("Cantidad de procesos: %d\n", input_file->len);
	// printf("Procesos:\n");

	int n_processes = input_file->len;
	Process **processes = load_processes(input_file, n_processes);
	// // Sort all based on start time
	// qsort(processes, n_processes, sizeof(Process *), cmpfunc);

	// // Print all the processes in the array and get the last that is going to run
	// int max_start_process_time = -1;
	// Process *last_process;
	// for (int i = 0; i < input_file->len; i++)
	// {
	// 	Process *process = processes[i];
	// 	process_print(process);
	// 	if (process->start_time > max_start_process_time)
	// 	{
	// 		last_process = process;
	// 		max_start_process_time = process->start_time;
	// 	}
	// }

	double time_start = get_timestamp();

	Queue *queue = queue_init();

	while (!are_all_processes_finished(processes, n_processes))
	{
		check_enter_processes(queue, time_start, processes, n_processes);
		check_waiting_processes(queue);
		check_running_process(queue);
		check_ready_processes(queue);
	}

	// Print stats and destroy the process instances
	for (int i = 0; i < input_file->len; i++)
	{
		process_print_stats(processes[i], output_file);
		process_destroy(processes[i]);
	}
	queue_destroy(queue);
	fclose(output_file);
	free(processes);
	input_file_destroy(input_file);

	return 0;
}

int main2()
{
	char **args0 = malloc(sizeof(char *) * 2);
	args0[0] = "1";
	args0[1] = "2";

	char **args1 = malloc(sizeof(char *) * 2);
	args1[0] = "1";
	args1[1] = "2";
	Process *process0 = process_init(0, "P0", waiting, 0, 10, 10, "/bin/echo", 1, args0);
	Process *process1 = process_init(1, "P1", ready, 0, 10, 10, "/bin/echo", 1, args1);

	Queue *queue = queue_init();
	queue_append_left(queue, process0);
	queue_append_left(queue, process1);

	queue_print(queue);
	process_print(queue_pop_ready(queue));

	process_destroy(process0);
	process_destroy(process1);

	return 0;
}