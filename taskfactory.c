#include "taskfactory.h"

unsigned int task_queue_max;
unsigned int task_queue_len;
unsigned int task_max;
unsigned int task_len;

char *task_error(int error_code)
{
	if(error_code == TASK_FULL)
		return "The Task Full!\n";
	if(error_code == TASK_EMPTY)
		return "The Task Empty!\n";
}

TASK_QUEUE *task_queue_init(void)
{
	TASK_QUEUE *head;

	head=malloc(sizeof(TASK_QUEUE));
	head->data=NULL;
	head->priority=0;
	head->next=NULL;
	task_queue_len=0;
	task_queue_max=0;

	return head;
}

int task_queue_add(TASK_QUEUE *head,void *data,unsigned int priority)
{
	TASK_QUEUE *temp;

	if(task_queue_len >= task_queue_max)
		return TASK_FULL;

	while(head->next != NULL)
	{
		if(head->next->priority <= priority)
			head=head->next;
		else
			break;
	}

	temp=malloc(TASK_QUEUE);
	temp->data=data;
	temp->priority=priority;

	if(head->next == NULL)
		temp->next=NULL;
	else
		temp->next=head->next;
	head->next=temp;
	++task_queue_len;
}

int task_queue_out(TASK_QUEUE *head,void *data)
{
	TASK_QUEUE *temp;

	if(task_queue_len <= 0)
		return TASK_EMPTY;

	temp=head->next;
	data=temp->data;
	head=head->next->next;
	free(temp);
	--task_queue_len;
}

bool task_queue_is_empty(void)
{
	if(task_queue_len <= 0)
		return TRUE;
	else
		return FALSE;
}

bool task_queue_is_full(void)
{
	if(task_queue_len >= task_queue_max)
		return TRUE;
	else
		return FALSE;
}

unsigned int task_queue_length(void)
{
	return task_queue_len;
}

void task_queue_set_max_task(unsigned int max)
{
	task_queue_max=max;
}

unsigned int task_queue_get_max_task(void)
{
	return task_queue_max;
}

void task_queue_destroy(TASK_QUEUE *head)
{
	TASK_QUEUE *temp;

	while(head != NULL)
	{
		temp=head;
		head=head->next;
		free(temp);
	}
}
