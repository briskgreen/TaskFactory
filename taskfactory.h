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
}TASK_QUEUE;
typedef unsigned int bool;

char *task_error(int error_code);
TASK_QUEUE *task_queue_init(void);
int task_queue_add(TASK_QUEUE *head,void *data,unsigned int priority);
int task_queue_out(TASK_QUEUE *head,void *data);
bool task_queue_is_empty(void);
bool task_queue_is_full(void);
unsigned int task_queue_length(void);
void task_queue_set_max_task(int num);
unsigned int task_queue_get_max_task(void);
void task_queue_destroy(TASK_QUEUE *head,TASK_QUEUE *tail);

#endif
