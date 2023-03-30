#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <stdbool.h>

#include "./src/process/process.h"

int cmpfunc(const void *a, const void *b)
{
	return (*(int *)a - *(int *)b);
}

int main()
{

	int a[5] = {1, 6, 1, 5, 8};
	qsort(a, 5, sizeof(int), cmpfunc);

	for (int i = 0; i < 5; i++)
	{
		printf("%d\n", a[i]);
	}

	// Process *process = process_init("p1", 1, ready, 10, 10);
	// process_print(process);
	// process_destroy(process);
	return 0;
}
