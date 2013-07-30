#include "taskfactory.h"

char *task_error(int error_code)
{
	if(error_code == TASK_FULL)
		return "The Task Full!\n";
	if(error_code == TASK_EMPTY)
		return "The Task Empty!\n";
}

TASK_QUEUE *task_queue_init(unsigned int max)
{
	TASK_QUEUE *head;

	head=malloc(sizeof(TASK_QUEUE));
	head->max=max;
	head->len=0;
	head->next=NULL;

	return head;
}

int task_queue_add(TASK_QUEUE *head,void *data,unsigned int priority)
{
	TASK_QUEUE_NODE *temp;
	TASK_QUEUE_NODE *node;

	if(head->len >= head->max)
		return TASK_FULL;

	temp=head;
	while(temp->next != NULL)
	{
		if(temp->next->priority <= priority)
			temp=temp->next;
		else
			break;
	}

	node=malloc(sizeof(TASK_QUEUE_NODE));
	node->data=data;
	node->priority=priority;

	if(temp->next == NULL)
		node->next=NULL;
	else
		node->next=temp->next;
	temp->next=node;
	++head->len;
	return 0;
}

int task_queue_out(TASK_QUEUE *head,void **data)
{
	TASK_QUEUE_NODE *temp;

	if(head->len <= 0)
		return TASK_FULL;

	temp=head->next;
	*data=temp->data;
	head->next=temp->next;
	free(temp);
	--head->len;
	return 0;
}

bool task_queue_is_empty(TASK_QUEUE *head)
{
	if(head->len <= 0)
		return TRUE;
	else
		return FALSE;
}

bool task_queue_is_full(TASK_QUEUE *head)
{
	if(head->len >= head->max)
		return TRUE;
	else
		return FALSE;
}

unsigned int task_queue_length(TASK_QUEUE *head)
{
	return head->len;
}

void task_queue_set_max_task(TASK_QUEUE *head,unsigned int max)
{
	head->max=max;
}

unsigned int task_queue_get_max_task(TASK_QUEUE *head)
{
	return head->max;
}

void task_queue_destroy(TASK_QUEUE *head)
{
	TASK_QUEUE_NODE *temp;
	TASK_QUEUE_NODE *node;

	temp=head->next;

	while(temp != NULL)
	{
		node=temp;
		free(node);
		temp=temp->next;
	}

	free(head);
}

TASK_FACTORY *task_factory_init(unsigned int task_max,
		unsigned int queue_max)
{
	TASK_FACTORY *task;

	task=malloc(sizeof(TASK_FACTORY));
	task->max=task_max;
	task->len=0;
	task->head=task_queue_init(queue_max);

	return task;
}

int task_factory_add(TASK_FACTORY *task,
		task_callback *task_func,void *data)
{}

void task_factory_finished(TASK_FACTORY *task)
{
	if(task->len != 0)
		--task->len;
}

void task_factory_set_max_task(TASK_FACTORY *task,unsigned int max)
{
	task->max=max;
}

unsigned int task_factory_get_max_task(TASK_FACTORY *task)
{
	return task->max;
}

unsigned int task_factory_get_length(TASK_FACTORY *task)
{
	return task->len;
}

bool task_factory_is_full(TASK_FACTORY *task)
{
	if(task->len >= task->max)
		return TRUE;
	else
		return FALSE;
}

void task_factory_destroy(TASK_FACTORY *task)
{
	task_queue_destroy(task->head);
	task->max=0;
	task->len=0;
	free(task);
}
