#include "taskfactory.h"

void break_pause(int signum){}
void task_factory(TASK_FACTORY *task);

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
	return 0;
}

int task_queue_out(TASK_QUEUE *head,TASK_QUEUE_NODE *data)
{
	TASK_QUEUE_NODE *temp;

	if(head->len <= 0)
		return TASK_FULL;

	temp=head->next;
	memcpy(data,temp,sizeof(TASK_QUEUE_NODE));
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
	pid_t pid;

	task=malloc(sizeof(TASK_FACTORY));
	task->max=task_max;
	task->len=0;
	task->head=task_queue_init(queue_max);
	task->shmid=shmget(IPC_PRIVATE,sizeof(pid_t),0666|IPC_CREAT);

	pid=fork();
	if(pid == -1)
		return NULL;
	if(pid == 0)
	{
		pid_t pid;

		pid=fork();

		if(pid == 0)
		{
			pid_t pid;
			char *addr;
			struct sigaction act;

			act.sa_flags=0;
			act.sa_handler=break_pause;
			sigaction(SIGUSR1,&act,NULL);
			addr=shmat(task->shmid,0,0);
			pid=setsid();
			memcpy(addr,&pid,sizeof(pid_t));
			shmdt(addr);

			while(1)
			{
				pause();
				task_factory(task);
			}
		}

		if(pid > 0)
			_exit(0);
	}
	
	waitpid(pid,NULL,0);
	sleep(1);
	return task;
}

int task_factory_add(TASK_FACTORY *task,
		task_callback task_func,void *data,
		unsigned int priority)
{
	pid_t pid;

	if(task_factory_is_full(task))
	{
		task_queue_add(task->head,task_func,data,priority);
		return TASK_ADD_QUEUE;
	}

	pid=fork();
	if(pid == -1)
		return TASK_FORK;
	if(pid == 0)
	{
		pid_t pid;
		char *addr;

		addr=shmat(task->shmid,0,0);
		memcpy(&pid,addr,sizeof(pid_t));
		setpgid(getpid(),pid);
		shmdt(addr);
		
		task_func(data);
		task_factory_finished(task);
		_exit(0);
	}

	++task->len;
	return TASK_OK;
}

void task_factory_finished(TASK_FACTORY *task)
{
	char *addr;
	pid_t pid;

	addr=shmat(task->shmid,0,0);
	memcpy(&pid,addr,sizeof(pid_t));
	kill(pid,SIGUSR1);
	shmdt(addr);
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
	char *addr;
	pid_t pid;

	task_queue_destroy(task->head);
	task->max=0;
	task->len=0;
	addr=shmat(task->shmid,0,0);
	memcpy(&pid,addr,sizeof(pid_t));
	killpg(pid,SIGKILL);
	shmdt(addr);
	free(task);
}

void task_factory(TASK_FACTORY *task)
{
	TASK_QUEUE_NODE data;

	task_queue_out(task->head,&data);
	task_factory_add(task,data.func,data.data,data.priority);
}
