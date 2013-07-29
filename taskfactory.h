#ifndef _TASKFACTORY_H
#define _TASKFACTORY_H

#include <unistd.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define TASK_FULL -1
#define TASK_EMPTY -2

typedef struct node
{
	void *data;
	unsigned int priority;
	struct node *next;
}TASK_QUEUE_NODE;
typedef struct 
{
	unsigned int max;
	unsigned int len;
	struct node *next;
}TASK_QUEUE;
typedef unsigned int bool;

char *task_error(int error_code);
TASK_QUEUE *task_queue_init(unsigned int max);
int task_queue_add(TASK_QUEUE *head,void *data,unsigned int priority);
int task_queue_out(TASK_QUEUE *head,void **data);
bool task_queue_is_empty(TASK_QUEUE *head);
bool task_queue_is_full(TASK_QUEUE *head);
unsigned int task_queue_length(TASK_QUEUE *head);
void task_queue_set_max_task(TASK_QUEUE *head,unsigned int max);
unsigned int task_queue_get_max_task(TASK_QUEUE *head);
void task_queue_destroy(TASK_QUEUE *head);

#endif
