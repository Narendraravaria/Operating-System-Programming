#include<stdio.h>
#include<stdlib.h>
#include<signal.h>

int termCount = 0;
int quitCount = 0;
int intCount = 0;
void noAbortQuitTerminate(int signo)
{
	setbuf(stdout,NULL);
	switch (signo)
	{
	case SIGINT:
		printf("C");
		intCount++;
		break;
	case SIGQUIT:
		printf("Q");
		quitCount++;
		break;
	case SIGTSTP:
		if(termCount != 2)
		{
			termCount++;
			printf("S");
		}
		else{
			termCount++;
			printf("S\nInterrupt: %d\nStop: %d\nQuit: %d\n",intCount,termCount,quitCount);
			exit(0);		
		}
		break;	
	}	
}

int main()
{
	struct sigaction action;

	action.sa_handler = &noAbortQuitTerminate;
	sigemptyset(&action.sa_mask);
	action.sa_flags =0;

	sigaction(SIGINT,&action,NULL);
	sigaction(SIGQUIT,&action,NULL);
	sigaction(SIGTSTP,&action,NULL);
	
	while(1)
		sleep(1);	

}
