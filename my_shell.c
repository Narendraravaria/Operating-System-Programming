#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// Code prints and '$ ' and waits in while for command and argument
// while loop gets terminated when user type EOF character(^D) 
// Inside while loop: 
//					 parse the command and argument separately
//					 fork() the new process (to execute that commad) 
//					 exec() with parameter as command and argument list got from user
//					 wait() in parent to receive exot status of CHILD process and print it.
// Program calls malloc to allocate predetermined size of an array. 
// But unnecessary waste of memory
//		Other solution can be allocate small amount of memory using malloc (depending on avg number of argument). 
//		but inside the inner while loop check the value of n_size if it reaches value equal to size allocated using malloc
//		use realloc to increase the size of array by half or same ammount as passed in malloc
//		Other Solution is allocates a memory dynamically using realloc for each new value of command 

char *removeNewline(char *);
void pr_exit_status(int );
int main()
{
		
	//START OF SHELL TERMINAL....
	printf("$ ");
	fflush(stdout);

	char s[100];
	char **arg = malloc(64*sizeof(char*)) ; 
	char *command, *st;
	int n_size ,i;

	while(fgets ( s, sizeof(s), stdin ) != NULL)
	{
		n_size = 0;
		st =removeNewline(s);

		command = strtok(st," ");	

		while(command!=NULL)
		{
			arg[n_size++] = command; 
			command = strtok(NULL," ");
		}// INNER WHILE END

		arg[n_size++]= (char *)0; //null terminated array
 
		//for (i = 0; i < (n_size); ++i)
		//	printf ("arg[%d] = %s\n", i, arg[i]);

		
		//FORK, EXEC & WAIT ...
		pid_t pid;
		int status;
		
		if((pid = fork()) <0)
		{
			printf("fork failed!\n");
		}else if(pid == 0)
		{
			if(execvp(arg[0],arg) < 0)
				printf("exec error\n");
		}else if(pid >0)
		{
			if(wait(&status) != pid)
				printf("wait error\n");
			pr_exit_status(status);
		}// IF END

		//READY TO RECEIVE NEXT SHELL COMMAND.....		
		printf("$ ");
		fflush(stdout);
	}//OUTER WHILE END

	// FREE ALLOCATED MEMORY...
		free (arg);	

	printf("\n");
}// MAIN END



void pr_exit_status(int status)
{
	if(WIFEXITED(status))
		printf("Normal termination, exit status = %d\n", WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		printf("Termination due to signal, signal number = %d\n",WTERMSIG(status));
	else if (WCOREDUMP(status))
		printf("Core file for terminated child process is generated\n");
	else if (WIFSTOPPED(status))
		printf("Child stopped, signal number = %d\n", WSTOPSIG(status));

}

char *removeNewline(char *s)
{
	int slen = strlen(s);
	if(slen>0 && s[slen-1] == '\n')
	{
		s[slen-1] = '\0';
	}
	return s;
}
