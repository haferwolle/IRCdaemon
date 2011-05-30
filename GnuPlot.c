#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define COMMAND "gnuplot"
#define INSTRUCTION "set grid\n" \
		"plot 1,x,x*x\n"

int exec_command(const char* plot_command)
{
	int pipes[2];
	int pid;

	if(pipe(pipes)==-1)
	{
		return 0;
	}

	pid = fork();

	if ( pid != 0 )
	{
		_exit (1);
	}


	if(pid==0){
		close(pipes[1]);
		if(dup2(pipes[0],STDIN_FILENO)==-1){
			_exit(1);
		}
		close(pipes[0]);
		execlp(plot_command,plot_command,(const char*)NULL);
		_exit(1);
	}
	else{
		close(pipes[0]);
		return pipes[1];
	}
}

int main(void){
	int fh=exec_command(COMMAND);

	if(fh==-1){
		fprintf(stderr,"Error:cannot open pipe to command:%s\n",COMMAND);
		return 1;	
	}
	if(write(fh,INSTRUCTION,strlen(INSTRUCTION))!=strlen(INSTRUCTION)){
		fprintf(stderr,"Error:cannot write data to pipe");
		return 2;
	}
	close(fh);
	wait(NULL);
	return 0;
}
