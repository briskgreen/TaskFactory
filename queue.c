#include "taskfactory.h"
#include <stdio.h>

typedef struct
{
	int i;
	char *des;
}DATA;

int main(void)
{
	TASK_QUEUE *head;
	TASK_QUEUE_NODE node;
	DATA *data;
	char *des[20]={"123","456","789",
	"abc","bcd","dfgh","wdg","wer","oiu",
	"fdr","fdr","sfg","lkh","fet","dwr",NULL};
	int i;

	head=task_queue_init(20);
	for(i=0;des[i];++i)
	{
		data=malloc(sizeof(DATA));
		data->i=i;
		data->des=des[i];

		task_queue_add(head,NULL,data,5);
	}

	printf("len=%d max=%d\n",task_queue_length(head),
			task_queue_get_max_task(head));
	for(i=0;i<10;++i)
	{
		task_queue_out(head,&node);
		data=node.data;
		printf("%d ",data->i);
		printf("%s\n",data->des);
	}
	printf("len=%d des=%d\n",task_queue_length(head),
			task_queue_get_max_task(head));
	task_queue_destroy(head);

	return 0;
}
