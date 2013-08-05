#include "taskfactory.h"
#include <stdio.h>

typedef struct
{
	int i;
	char *des;
}DATA;

#define TO_DATA(node) \
{\
	*((DATA *)node.data);\
}

void print(void *data)
{
	DATA *d;
	static int i=0;
	printf("in %d\n",++i);

	d=(DATA *)data;
	printf("%d %s\n",d->i,d->des);
	//printf("%s\n",data);
}

int main(void)
{
	TASK_FACTORY *task;
	DATA *data;
	char *des[]={"123","456","789",
		"abc","bcd","dfgh","wdg","wer","oiu",
		"fdr","fdr","sfg","lkh","fet","dwr","fuck",
		"brisk","test","joke","what","the","comon",
		NULL};
	int i;

	task=task_factory_init(7,100);
	for(i=0;des[i];++i)
	{
		data=malloc(sizeof(DATA));
		data->i=i;
		data->des=des[i];

		task_factory_add(task,print,data,5);
	}


	while(1);
	task_factory_destroy(task);
	printf("OK\n");

	return 0;
}
