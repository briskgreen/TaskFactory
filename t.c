#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <signal.h>

void task(int signum){}

void create_task(int shmid)
{
	pid_t t;

	t=fork();
	if(t == 0)
	{
		pid_t pid;

		pid=fork();
		if(pid == 0)
		{
			pid_t pid;
			char *addr;

			addr=shmat(shmid,0,0);
			pid=setsid();
			strncpy(addr,(char *)&pid,sizeof(pid_t));
			shmdt(addr);
			printf("in child(3) %d %d\n",pid,getpid());
			while(1);
		}
		if(pid > 0)
		{
			printf("in child(2) %d\n",getpid());
			_exit(0);
		}
	}

	if(t == -1)
		return;
	waitpid(t,NULL,0);
	printf("in create_task %d\n",getpid());
}

void finalsh(int shmid,pid_t man)
{
	pid_t pid;
	char *addr;

	addr=shmat(shmid,0,0);
	strncpy((char *)&pid,addr,sizeof(pid_t));
	printf("the task is %d\n",pid);
	killpg(pid,SIGKILL);
	shmdt(&pid);
	kill(man,SIGUSR1);
}

int main(void)
{
	int shmid;
	struct sigaction act;

	shmid=shmget(IPC_PRIVATE,sizeof(pid_t),0666|IPC_CREAT);
	act.sa_flags=0;
	act.sa_handler=task;
	sigaction(SIGUSR1,&act,NULL);
	create_task(shmid);
	printf("in main %d\n",getpid());
	finalsh(shmid,getpid());
	pause();
	printf("Exit . . .\n");

	return 0;
}
