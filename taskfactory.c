#include "taskfactory.h"

typedef struct
{
	TASK_FACTORY *task;
	task_callback task_func;
	void *data;
}DATA;
typedef struct
{
	int i;
	char *des;
}T;

pthread_mutex_t task_factory_mutex;

void break_pause(int signum){}
void task_factory(TASK_FACTORY *task);
void task_add(DATA *data);

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

int task_queue_add(TASK_QUEUE *head,task_callback task_func,
		void *data,unsigned int priority)
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
	node->func=task_func;

	if(temp->next == NULL)
		node->next=NULL;
	else
		node->next=temp->next;
	temp->next=node;
	++head->len;
	printf("in task_queue_add %d %s\n",((T *)node->data)->i,((T *)node->data)->des);
	return 0;
}

int task_queue_out(TASK_QUEUE *head,TASK_QUEUE_NODE *data)
{
	TASK_QUEUE_NODE *temp;

	if(head->len <= 0)
		return TASK_EMPTY;

	temp=head->next;
	memcpy(data,temp,sizeof(TASK_QUEUE_NODE));
	head->next=temp->next;
	printf("next %d %s\n",((T *)temp->next->next->data)->i,((T *)temp->next->next->data)->des);
	free(temp);
	--head->len;
	printf("in task_queue_out %d %s\n",((T *)data->data)->i,((T *)data->data)->des);
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
	struct sigaction act;

	act.sa_flags=0;
	act.sa_handler=break_pause;
	sigaction(SIGUSR1,&act,NULL);

	task=malloc(sizeof(TASK_FACTORY));
	task->max=task_max;
	task->len=0;
	task->head=task_queue_init(queue_max);
	pthread_mutex_init(&task_factory_mutex,NULL);
	if(pthread_create(&task->thread,NULL,(void *)task_factory,task) == -1) 
	{
		free(task);
		return NULL;
	}

	return task;
}

int task_factory_add(TASK_FACTORY *task,
		task_callback task_func,void *data,
		unsigned int priority)
{
	pthread_t thread;
	DATA d;

	if(task_factory_is_full(task))
	{
		//pthread_mutex_lock(&task_factory_mutex);
		task_queue_add(task->head,task_func,data,priority);
		//pthread_mutex_unlock(&task_factory_mutex);
		return TASK_ADD_QUEUE;
	}

	//pthread_mutex_lock(&task_factory_mutex);
	d.task=task;
	d.task_func=task_func;
	d.data=data;
	//pthread_mutex_unlock(&task_factory_mutex);

	if(pthread_create(&thread,NULL,(void *)task_add,&d) == -1)
		return TASK_THREAD;

	++task->len;
	return TASK_OK;
}

void task_factory_finished(TASK_FACTORY *task)
{
	pthread_kill(task->thread,SIGUSR1);
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
	pthread_cancel(task->thread);
	free(task);
}

void task_factory(TASK_FACTORY *task)
{
	TASK_QUEUE_NODE data;
	/*struct sigaction act;

	act.sa_flags=0;
	act.sa_handler=break_pause;
	sigaction(SIGUSR1,&act,NULL);*/

	while(1)
	{
		pause();

		if(task_queue_out(task->head,&data) == TASK_EMPTY)
			continue;

		//pthread_mutex_lock(&task_factory_mutex);
		task_factory_add(task,data.func,data.data,data.priority);
		//pthread_mutex_unlock(&task_factory_mutex);
	}
}

void task_add(DATA *data)
{
	pthread_mutex_lock(&task_factory_mutex);
	data->task_func(data->data);
	task_factory_finished(data->task);
	pthread_mutex_unlock(&task_factory_mutex);
	pthread_exit(NULL);
}
