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
		int path = atoi(input_file->lines[i][4]);
		int n_args = atoi(input_file->lines[i][5]);
		char *args[n_args];

		for (int j = 0; j < n_args; j++)
		{
			args[j] = input_file->lines[i][j + 6];
		}

		Process *process = process_init(i, name, state, start_time, cpu_burst, io_wait, n_args, args);
		processes[i] = process;
	}

	return processes;
}

int main(int argc, char const *argv[])
{
	/*Lectura del input*/
	char *file_name = (char *)argv[1];
	InputFile *input_file = read_file(file_name);

	/*Mostramos el archivo de input en consola*/
	printf("Nombre archivo: %s\n", file_name);
	printf("Cantidad de procesos: %d\n", input_file->len);
	printf("Procesos:\n");

	int n_processes = input_file->len;
	Process **processes = load_processes(input_file, n_processes);
	// // Sort all based on start time
	qsort(processes, n_processes, sizeof(Process *), cmpfunc);

	// Print all the processes in the array
	for (int i = 0; i < input_file->len; i++)
	{
		process_print(processes[i]);
	}

	double time_start = clock() / CLOCKS_PER_SEC;

	Queue *queue = queue_init();

	Process *last_process = processes[n_processes - 1];
	while (last_process->state != finished) // TO DO: Change to finished
	{
		check_enter_processes(queue, time_start, processes, n_processes);
		check_running_process(queue);
		check_ready_processes(queue);
	}

	// Print all the processes in the array
	for (int i = 0; i < input_file->len; i++)
	{
		process_destroy(processes[i]);
	}
	queue_destroy(queue);
	free(processes);
	input_file_destroy(input_file);

	return 0;
}

int main2()
{
	char *args[2] = {"1", "2"};
	Process *process0 = process_init(0, "P0", waiting, 0, 10, 10, 1, args);
	Process *process1 = process_init(1, "P1", ready, 0, 10, 10, 1, args);
	Process *process2 = process_init(2, "P2", waiting, 0, 10, 10, 1, args);

	Queue *queue = queue_init();
	queue_append_right(queue, process0);
	queue_append_right(queue, process1);
	queue_append_right(queue, process2);

	process_print(queue_pop_ready(queue));
	// process_print(queue_pop_left(queue));
	// process_print(queue_pop_right(queue));
	// process_print(queue_pop_right(queue));

	process_destroy(process0);
	process_destroy(process1);
	process_destroy(process2);
	queue_destroy(queue);

	return 0;
}