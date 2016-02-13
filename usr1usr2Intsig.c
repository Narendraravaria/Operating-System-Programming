#include<stdio.h>
#include<stdlib.h>
#include<signal.h>

int usr1sig = 0;
void usr1SigHandler(int signo, siginfo_t *siginfo, void *context)
{

	switch(signo)	
	{
		case SIGUSR1:
			usr1sig++;
			kill(siginfo->si_pid, SIGUSR2);
			break;
		case SIGINT:
			printf("\nSignal: %d\n",usr1sig);
			fflush(stdout);
			kill(siginfo->si_pid, SIGKILL);
			exit(0);
			break;
	}

}

int main()
{

	struct sigaction action;

	action.sa_sigaction = &usr1SigHandler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_SIGINFO;

	sigaction(SIGUSR1, &action,NULL);
	sigaction(SIGINT, &action,NULL);
	while(1)
		sleep(1);
}
